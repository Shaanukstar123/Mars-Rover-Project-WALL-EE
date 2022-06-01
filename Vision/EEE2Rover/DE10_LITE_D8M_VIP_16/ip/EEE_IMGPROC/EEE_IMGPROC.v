module EEE_IMGPROC(
	// global clock & reset
	clk,
	reset_n,
	
	// mm slave
	s_chipselect,
	s_read,
	s_write,
	s_readdata,
	s_writedata,
	s_address,

	// stream sink
	sink_data,
	sink_valid,
	sink_ready,
	sink_sop,
	sink_eop,
	
	// streaming source
	source_data,
	source_valid,
	source_ready,
	source_sop,
	source_eop,
	
	// conduit
	mode,
	thresholdsHue, //Posterization thresholds to be accessed as MM by NIOS2
	numThresholds, //Thresholds to use ^^
	thresholdSat,
	thresholdVal
);


// global clock & reset
input	clk;
input	reset_n;

// mm slave
input							s_chipselect;
input							s_read;
input							s_write;
output	reg	[31:0]	s_readdata;
input	[31:0]				s_writedata;
input	[2:0]					s_address;


// streaming sink
input	[23:0]            	sink_data;
input								sink_valid;
output							sink_ready;
input								sink_sop;
input								sink_eop;

// streaming source
output	[23:0]			  	   source_data;
output								source_valid;
input									source_ready;
output								source_sop;
output								source_eop;

// conduit export
input                         mode; //externally connected to switch 0
input [143:0] thresholdsHue; //Posterization thresholds to be accessed as MM by NIOS2 (16 x 9 bit coefficients)
input [3:0] numThresholds; //Thresholds to use ^^
input [7:0] thresholdSat;
input [7:0] thresholdVal;

////////////////////////////////////////////////////////////////////////
//
parameter IMAGE_W = 11'd640;
parameter IMAGE_H = 11'd480;
parameter MESSAGE_BUF_MAX = 256;
parameter MSG_INTERVAL = 6;
parameter BB_COL_DEFAULT = 24'h00ff00;


wire [7:0]   red, green, blue, gray;
wire [7:0]   red_out, green_out, blue_out;

wire         sop, eop, in_valid, out_ready;
//HSV calculations
wire [143:0] tempThresholdsHue;
wire [15:0] Saturation;
wire [8:0] Hue;
wire [7:0] maxVal, minVal, Value, finalSat, finalVal, tempThresholdSat, tempThresholdVal;
wire [3:0] tempNumThresholds;
wire isRedMax, isGreenMax, isBlueMax;
wire isRedMin, isGreenMin, isBlueMin;
wire satThresholdMet, valThresholdMet;

reg [23:0] runningValueTotal;
reg [15:0] zPartial, zPartial2, zPartial3;
reg [13:0] HRed, HGreen, HBlue;
reg [8:0] finalHue, zHSV, zmSum;
reg [7:0] newRed, newGreen, newBlue, averageVal, mHSV, delta;

assign tempNumThresholds = 4'd5;
assign tempThresholdSat = 8'd128;
assign tempThresholdVal = 8'd128;
assign tempThresholdsHue = {9'd0, 9'd80, 9'd180, 9'd250, 9'd310, 99'd0}; //Red, green, cyan, blue, pink
////////////////////////////////////////////////////////////////////////

assign gray = green[7:1] + red[7:2] + blue[7:2]; //Grey = green/2 + red/4 + blue/4 (average of all colours to produce grayscale)
//Pixels enter sequentially
//Convert to HSV:
//Max value
assign isRedMax = (red > blue) & (red > green) ? 1 : 0; //One bit
assign isGreenMax = (green > blue) & (green > red) ? 1 : 0;
assign isBlueMax = (blue > red) & (blue > green) ? 1 : 0;
assign maxVal = isRedMax ? red : (isBlueMax ? blue : green);

//Min val
assign isRedMin = (red < blue) & (red < green) ? 1 : 0; //One bit
assign isGreenMin = (green < blue) & (green < red) ? 1 : 0;
assign isBlueMin = (blue < red) & (blue < green) ? 1 : 0;
assign minVal = isRedMin ? red : (isBlueMin ? blue : green);
////
assign Hue = isRedMax ? HRed : (isGreenMax ? HGreen : HBlue); //(0-360) 9 bits
assign Saturation = (delta << 8)/maxVal; //(0-255) //8Bits, max 16 bits
assign Value = maxVal; //(0-255)

//Thresholds - might need to encase in clocked logic
assign satThresholdMet = (tempThresholdSat < Saturation) ? 1 : 0;
assign valThresholdMet = (tempThresholdVal < Saturation) ? 1 : 0;

//Final values for S and V
assign finalSat = satThresholdMet ? 255 : Saturation;
assign finalVal = valThresholdMet ? 255 : Value;

assign {red_out, green_out, blue_out} = (mode & ~sop & packet_video)? {newRed, newGreen, newBlue} : {red,green,blue};

always @(posedge clk) begin
	//HSV calculations
	delta <= maxVal-minVal; //8 bits
	HRed <= ((60 * (green-blue))/delta) % 6; //Max 14 bits
	HGreen <= (((60 *(blue-red))/delta) + 120) % 255;
	HBlue <= (((60 *(red-green))/delta) + 240) % 255;
	//Convert back to RGB
	mHSV <= finalVal - finalSat; //8bits
	zPartial <= (((finalHue << 7)/60) % 256) - 128; //Ranges from -128 to 127
	zPartial2 <= zPartial[15] ? -zPartial : zPartial; //Must be positive : ranges from 0 to 127
	zPartial3 <= ((finalSat*zPartial2[7:0]) >> 7);
	zHSV <= finalSat - zPartial3; //Obtain final value for z (8 bits)
	zmSum <= zHSV + mHSV;
	//Set finalHue to nearest threshold value
	if (satThresholdMet & valThresholdMet) begin
		integer i;
		for(i = 1; i < tempNumThresholds; i = i + 1) begin
			if (tempThresholdsHue[9*(i-1)+:9] > Hue) begin
				if (i > 1) begin
					finalHue <= tempThresholdsHue[9*(i-2)+:9];
				end else begin
					finalHue <= 9'b0;
				end
			end
		end
		if (finalHue < 60) begin
			newRed <= finalVal;
			newGreen <= zmSum;
			newBlue <= mHSV;
		end	else if (finalHue < 120) begin
			newRed <= zmSum;
			newGreen <= finalVal;
			newBlue <= mHSV;
		end	else if (finalHue < 180) begin
			newRed <= mHSV;
			newGreen <= finalVal;
			newBlue <= zmSum;
		end	else if (finalHue < 240) begin
			newRed <= mHSV;
			newGreen <= zmSum;
			newBlue <= finalVal;
		end	else if (finalHue < 300) begin
			newRed <= zmSum;
			newGreen <= mHSV;
			newBlue <= finalVal;
		end	else if (finalHue < 360) begin
			newRed <= finalVal;
			newGreen <= mHSV;
			newBlue <= zmSum;
		end
	//If pixels do not meet the required levels of saturation and value, reduce them to grayscale
	end else begin
		newRed <= gray;
		newGreen <= gray;
		newBlue <= gray;
	end
end

//Count valid pixels to tget the image coordinates. Reset and detect packet type on Start of Packet.
reg [10:0] x, y;
reg packet_video;
always@(posedge clk) begin
	if (sop) begin
		x <= 11'h0;
		y <= 11'h0;
		runningValueTotal <= 24'b0; //reset at the end of every frame
		packet_video <= (blue[3:0] == 3'h0);
	end
	else if (in_valid) begin
		if (x == IMAGE_W-1) begin
			x <= 11'h0;
			y <= y + 11'h1;
		end
		else begin
			x <= x + 11'h1;
			//Sample every 4 bits
			if((x % 4) == 0) begin 
				runningValueTotal <= runningValueTotal + Value;
			end
		end
	end
end

//Process bounding box at the end of the frame.
reg [1:0] msg_state;
reg [7:0] frame_count;
always@(posedge clk) begin
	if (eop & in_valid & packet_video) begin  //Ignore non-video packets
		//Start message writer FSM once every MSG_INTERVAL frames, if there is room in the FIFO
		frame_count <= frame_count - 1;
		//Calculate average value of the frame
		averageVal <=  runningValueTotal >> 16;
		if (frame_count == 0 && msg_buf_size < MESSAGE_BUF_MAX - 3) begin
			msg_state <= 2'b01;
			frame_count <= MSG_INTERVAL-1;
		end
	end
	
	//Cycle through message writer states once started
	if (msg_state != 2'b00) msg_state <= msg_state + 2'b01;

end
	
//Generate output messages for CPU
reg [31:0] msg_buf_in; 
wire [31:0] msg_buf_out;
reg msg_buf_wr;
wire msg_buf_rd, msg_buf_flush;
wire [7:0] msg_buf_size;
wire msg_buf_empty;

`define RED_BOX_MSG_ID "RBB" //this is a macro

//Use to communicate with the NIOS processor
always@(*) begin	//Write words to FIFO as state machine advances
	case(msg_state)
		2'b00: begin
			msg_buf_in = 32'b0;
			msg_buf_wr = 1'b0;
		end
		default: begin
			//Communicate V with the average value
			msg_buf_in = {8'h56, runningValueTotal};
			msg_buf_wr = 1'b1;
		end
	endcase
end


//Output message FIFO
MSG_FIFO	MSG_FIFO_inst (
	.clock (clk),
	.data (msg_buf_in),
	.rdreq (msg_buf_rd),
	.sclr (~reset_n | msg_buf_flush),
	.wrreq (msg_buf_wr),
	.q (msg_buf_out),
	.usedw (msg_buf_size),
	.empty (msg_buf_empty)
	);


//Streaming registers to buffer video signal
STREAM_REG #(.DATA_WIDTH(26)) in_reg (
	.clk(clk),
	.rst_n(reset_n),
	.ready_out(sink_ready),
	.valid_out(in_valid),
	.data_out({red,green,blue,sop,eop}),
	.ready_in(out_ready),
	.valid_in(sink_valid),
	.data_in({sink_data,sink_sop,sink_eop})
);

STREAM_REG #(.DATA_WIDTH(26)) out_reg (
	.clk(clk),
	.rst_n(reset_n),
	.ready_out(out_ready),
	.valid_out(source_valid),
	.data_out({source_data,source_sop,source_eop}),
	.ready_in(source_ready),
	.valid_in(in_valid),
	.data_in({red_out, green_out, blue_out, sop, eop})
);


/////////////////////////////////
/// Memory-mapped port		 /////
/////////////////////////////////

// Addresses
`define REG_STATUS    			0
`define READ_MSG    			1
`define READ_ID    				2
`define REG_BBCOL				3

//Status register bits
// 31:16 - unimplemented
// 15:8 - number of words in message buffer (read only)
// 7:5 - unused
// 4 - flush message buffer (write only - read as 0)
// 3:0 - unused


// Process write

reg  [7:0]   reg_status;
reg	[23:0]	bb_col;

always @ (posedge clk)
begin
	if (~reset_n)
	begin
		reg_status <= 8'b0;
		bb_col <= BB_COL_DEFAULT;
	end
	else begin
		if(s_chipselect & s_write) begin
		   if      (s_address == `REG_STATUS)	reg_status <= s_writedata[7:0];
		   if      (s_address == `REG_BBCOL)	bb_col <= s_writedata[23:0];
		end
	end
end


//Flush the message buffer if 1 is written to status register bit 4
assign msg_buf_flush = (s_chipselect & s_write & (s_address == `REG_STATUS) & s_writedata[4]);


// Process reads
reg read_d; //Store the read signal for correct updating of the message buffer

// Copy the requested word to the output port when there is a read.
always @ (posedge clk)
begin
   if (~reset_n) begin
	   s_readdata <= {32'b0};
		read_d <= 1'b0;
	end
	
	else if (s_chipselect & s_read) begin
		if   (s_address == `REG_STATUS) s_readdata <= {16'b0,msg_buf_size,reg_status};
		if   (s_address == `READ_MSG) s_readdata <= {msg_buf_out};
		if   (s_address == `READ_ID) s_readdata <= 32'h1234EEE2;
		if   (s_address == `REG_BBCOL) s_readdata <= {8'h0, bb_col};
	end
	
	read_d <= s_read;
end

//Fetch next word from message buffer after read from READ_MSG
assign msg_buf_rd = s_chipselect & s_read & ~read_d & ~msg_buf_empty & (s_address == `READ_MSG);
						


endmodule

