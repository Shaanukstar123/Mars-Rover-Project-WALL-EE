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

	//VSPI interface
	SPI_dataout,
	SPI_write_valid,
	SPI_write_ready,
	SPI_datain,
	SPI_read_ready,
	SPI_read_valid
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

//SPI interface - write
output reg [7:0] SPI_dataout;
output reg SPI_write_valid;
input SPI_write_ready;
//Read
input [7:0] SPI_datain;
input SPI_read_valid;
output reg SPI_read_ready;
reg [7:0] SPI_dataretain, dataIndex;
reg cycleNo;

reg [15:0] angleCalc;
//reg [4:0] angleSlice;
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
reg [23:0] colourOutput;
//assign {red_out, green_out, blue_out} = (mode & ~sop & packet_video)? {newRed, newGreen, newBlue} : {red,green,blue};
assign {red_out, green_out, blue_out} = (mode & ~sop & packet_video)? colourOutput : {red,green,blue};
//HSV calculations
wire [8:0] Hue;
wire [15:0] Saturation, Luminance;  
wire [7:0] maxVal, minVal, Value, delta;
wire isRedMax, isGreenMax, isBlueMax;
wire isRedMin, isGreenMin, isBlueMin;

//reg [24:0] dataBuffer[639:0][2:0] ;//This contains the data of the last three rows in the image in HSV *(9 + 8 + 8 = 25)
reg [23:0] runningValueTotal;
// wire [15:0] zPartial, zPartial2, zPartial3, mHSV;
// wire [13:0] HRed, HGreen, HBlue;
// reg [8:0] finalHue, zmSum;
// wire [8:0] outputHue;
// wire [7:0] outputSat, outputVal, newRed, newGreen, newBlue;
// reg [7:0] finalSat, finalVal;
reg [10:0] x, y;
////////////////////////////////////////////////////////////////////////

assign gray = green[7:1] + red[7:2] + blue[7:2]; //Grey = green/2 + red/4 + blue/4 (average of all colours to produce grayscale)
//Pixels enter sequentially

//Convert to HSV:
//Max value
assign isRedMax = ((red > blue) && (red > green)) ? 1 : 0; //One bit
assign isGreenMax = ((green > blue) && (green > red)) ? 1 : 0;
assign isBlueMax = ((blue > red) && (blue > green)) ? 1 : 0;
assign maxVal = isRedMax ? red : (isBlueMax ? blue : green);
//Min val
assign isRedMin = ((red < blue) && (red < green)) ? 1 : 0; //One bit
assign isGreenMin = ((green < blue) && (green < red)) ? 1 : 0;
assign isBlueMin = ((blue < red) && (blue < green)) ? 1 : 0;
assign minVal = isRedMin ? red : (isBlueMin ? blue : green);
//Intermediate values
assign delta = maxVal-minVal; //8 bits
//RGB to HSV calculations - Assume correct
// assign Hue = (delta == 0) ? 14'd0 :
// ((isRedMax && isBlueMin) ? ((60*(green-blue))/delta) :
// ((isGreenMax && isBlueMin) ? (60+((60*(red-blue))/delta)) :
// ((isGreenMax && isRedMin) ? (120+((60*(blue-red))/delta)) :
// ((isBlueMax && isRedMin) ? (180+((60*(green-red))/delta)) :
// ((isBlueMax && isGreenMin) ? (240+((60*(red-green))/delta)) :
// (300+((60*(blue-green))/delta)))))));

//Previous implementation was incorrect
assign Hue = (delta == 0) ? 14'd0 :
((isRedMax && isBlueMin) ? ((60*(green-blue))/delta) :
((isGreenMax && isBlueMin) ? (120-((60*(red-blue))/delta)) :
((isGreenMax && isRedMin) ? (120+((60*(blue-red))/delta)) :
((isBlueMax && isRedMin) ? (240-((60*(green-red))/delta)) :
((isBlueMax && isGreenMin) ? (240+((60*(red-green))/delta)) :
(300+((60*(blue-green))/delta)))))));


//assign Hue = isRedMax ? HRed[8:0] : 
//(isBlueMax ? HBlue[8:0] : HGreen[8:0]); //(0-360) 9 bits
assign Saturation = (maxVal == 0) ? 16'd0 : ((delta * 256)/maxVal); //(0-255) //8Bits, max 16 bits
assign Value = maxVal; //(0-255)
assign Luminance = (maxVal + minVal)/2;
//Convert back to RGB - might not be needed
// assign outputHue = (Hue % 360);
// assign outputSat = Saturation[7:0];
// assign outputVal = Value;
// assign mHSV = (outputVal - ((outputSat*outputVal)>>8)); //8bits
// assign zPartial = (((outputHue << 7)/60) % 256); //Ranges from 0 to 255
// assign zPartial2 = (zPartial > 127) ? (zPartial-128) : (127-zPartial); //Must be positive : ranges from 0 to 127
// assign zPartial3 = ((outputSat*zPartial2[7:0]) >> 7);
// assign zmSum = (outputSat > zPartial3) ? ((outputSat-zPartial3)+mHSV) : (mHSV-(zPartial3-outputSat));
// //
// assign newRed = (outputHue < 60) ? outputVal : 
// ((outputHue < 120) ? zmSum[7:0] :  
// ((outputHue < 240) ? mHSV[7:0] :
// ((outputHue < 300) ? zmSum[7:0] : outputVal )));

// assign newGreen = (outputHue < 60) ? zmSum[7:0]: 
// ((outputHue < 180) ? outputVal :
// ((outputHue < 240) ? zmSum[7:0] : mHSV[7:0]));

// assign newBlue = (outputHue < 120) ? mHSV[7:0] : 
// ((outputHue < 180) ?  zmSum[7:0] : 
// ((outputHue < 300) ?  outputVal: zmSum[7:0]));

//////////////////// Threshold sections
wire red_detect, green_detect, blue_detect, lightgreen_detect, pink_detect, yellow_detect;
wire pink_detect1, pink_detect2;
////////////////////
//assign red_detect = ((compHue > 330) || (compHue < 35)) ? ((compSat > 150) ? ((compVal > 60) ? 1 : 0) : 0) : 0;
// assign red_detect = ((Hue > 330) || (Hue < 25)) ? ((Saturation > 155) ? ((Value > 80) ? 1 : 0) : 0) : 0;X
// assign blue_detect = ((Hue > 110) && (Hue < 185)) ? ((Saturation < 128) ? ((Value > 25) ? 1 : 0) : 0) : 0;
// assign green_detect = ((Hue > 110) && (Hue < 185)) ? ((Saturation > 115 && Saturation < 204) ? ((Value > 25) ? 1 : 0) : 0) : 0;
// assign pink_detect = ((Hue > 330) || (Hue < 35)) ? ((Saturation > 135 && Saturation < 195) ? ((Value > 100) ? 1 : 0) : 0) : 0;
// assign gray_detect = 0;
//assign red_detect =  (Hue < 40) ? ((Saturation > 185) ? ((Value > 105) ? 1 : 0) : 0) : 0;
//assign red_detect =  ((Hue < 14) || (Hue > 350)) ? ((Saturation > 140) ? ((Value > 40) ? 1 : 0) : 0) : 0;
//assign red_detect =  ((Hue < 16) || (Hue > 350)) ? ((Saturation > 60) ? ((Value > 30) ? 1 : 0) : 0) : 0;


//For lab
//assign red_detect = (Hue < 38) ? ((Saturation > 145) ? ((Value > 60) ? 1 : 0) : 0) : 0;  //Somewhat includes the pink
assign red_detect = (Hue < 30) ? ((Saturation > 160) ? ((Value > 47) ? 1 : 0) : 0) : 0; //Room
//assign green_detect =  ((Hue > 120) && (Hue < 190)) ? ((Saturation > 80)? ((Value > 30) ? 1 : 0) : 0) : 0; //Somwhat includes lightgreen
assign green_detect =  ((Hue > 110) && (Hue < 190)) ? ((Saturation > 75)? ((Value > 30) ? 1 : 0) : 0) : 0; //Somwhat includes lightgreen
//assign blue_detect =  (Hue > 178 && Hue < 260) ? ((Saturation > 60) ? ((Value > 5) ? 1 : 0) : 0) : 0; //Somwhat includes green
assign blue_detect =  (Hue > 178 && Hue < 260) ? ((Saturation > 50) ? ((Value > 5) ? 1 : 0) : 0) : 0; //Somwhat includes green
//assign yellow_detect = ((Hue > 51) && (Hue < 62)) ? ((Saturation > 128) ? ((Value > 180) ? 1 : 0) : 0) : 0; //Uniquely defines the yellow ball
assign yellow_detect = ((Hue > 50) && (Hue < 68)) ? ((Saturation > 150) ? ((Value > 150) ? 1 : 0) : 0) : 0; //Uniquely defines the yellow ball
//assign pink_detect = (Hue < 30) ? (((Saturation > 100) && (Saturation < 190)) ? ((Value > 120) ? 1 : 0) : 0) : 0;//Uniuely defines the pink ball

//assign pink_detect = (Hue < 30) ? ((Saturation < 175) ? ((Value > 200) ? 1 : 0) : 0) : 0;//Uniuely defines the pink ball
//assign pink_detect1 = (Hue < 18) ? ((Saturation > 150) ? ((Value > 200) ? 1 : 0) : 0) : 0; //Room
assign pink_detect = (Hue < 30) ? (((Saturation > 128 && Saturation < 180)) ? ((Value > 120) ? 1 : 0) : 0) : 0;
//assign pink_detect = (pink_detect1 || pink_detect2);
//assign pink_detect = 0;
assign lightgreen_detect = ((Hue > 105) && (Hue < 136)) ? ((Saturation > 90) ? ((Value > 90) ? 1 : 0) : 0) : 0; //uniqeuly defines the green ball - perfect

//assign pink_detect = 0;
//assign blue_detect = 0;
//assign red_detect =  (Hue < 28) ? ((Saturation > 224) ? ((Value > 75 && Value < 160) ? 1 : 0) : 0) : 0;

//assign green_detect =  (Hue > 115 && Hue < 170) ? (((Saturation > 52) && (Saturation < 213))? (((Value > 20) && (Value < 123))? 1 : 0) : 0) : 0; //includes light_green
//assign blue_detect =  (Hue > 160 && Hue < 313) ? (((Saturation > 16) && (Saturation < 194))? (((Value > 5) && (Value < 117)) ? 1 : 0) : 0) : 0; //Includes dark_green


//For room
//assign red_detect = (Hue < 38) ? ((Saturation > 145) ? ((Value > 47) ? 1 : 0) : 0) : 0;
//assign red_detect = 0;
//assign green_detect = 0;
//assign green_detect = (Hue > 96 && Hue < 224) ? ((Saturation > 100) ? ((Value > 5) ? 1 : 0) : 0) : 0;
// assign blue_detect = 0;
// assign yellow_detect = 0;
// assign pink_detect = 0;
// assign lightgreen_detect = 0;

//
////////////////////
//Red = 0, Green = 1, Blue = 2, 3 = Orange, 4 = Pink, 5 = Gray
localparam pixelRange = 32;
localparam detectionThreshold = 12;
wire detectionArray [5:0];
//assign detectionArray = {red_detect, green_detect, blue_detect, orange_detect, pink_detect, gray_detect};
//assign detectionArray[0] = (red_detect && !pink_detect); //Red overlaps with pink
assign detectionArray[0] = (red_detect  && !pink_detect);
assign detectionArray[1] = (green_detect && !lightgreen_detect); //Green overlaps with light green
//assign detectionArray[2] = (blue_detect && !green_detect); //blue overlaps with green
assign detectionArray[2] = blue_detect;
assign detectionArray[3] = yellow_detect;
//assign detectionArray[4] = pink_detect;
assign detectionArray[4] = (pink_detect && !red_detect);
assign detectionArray[5] = lightgreen_detect;
wire [23:0] colourCodes [5:0]; //Holds output colour codes for all balls
assign colourCodes[0] = 24'hff0000; //Red
assign colourCodes[1] = 24'h00df00; //Green
assign colourCodes[2] = 24'h0000ff; //Blue
assign colourCodes[3] = 24'hffff00; //Yellow
assign colourCodes[4] = 24'hff00ff; //Pink
assign colourCodes[5] = 24'h00ff80; //Light green
reg [10:0] tempCount; //Allow up to 128 detected pixels
//Used for largest contour identification in bounds drawing
reg [10:0] tempXMax [5:0]; //640
reg [10:0] tempPixelWidth [5:0]; //640
reg [pixelRange-1:0] pixelBuffer [5:0]; //Shift reg 
reg [10:0] xMin [5:0]; //640
reg [10:0] xMax [5:0]; 
reg [10:0] pixelWidth [5:0]; //640
//Bounding boxes for next frame
reg [15:0] left [5:0]; //640
reg [15:0] right [5:0]; //640


//Building Detection
localparam stripLength = 32;
localparam detectionWidth = 12;
reg [10:0] buildingtempXMax;
reg [10:0] buildingtempXMin;
reg [10:0] buildingtempWidth;
reg [stripLength-1:0] buildingWhitePixelBuffer;
reg [stripLength-1:0] buildingBlackPixelBuffer;
reg [10:0] buildingXMax;
reg [10:0] buildingXMin;
reg [10:0] buildingWidth;
reg [15:0] buildingLeft;
reg [15:0] buildingRight;
wire black_detect, white_detect;
//assign black_detect = 0;
//assign white_detect = 0;
assign black_detect = (Luminance < 60) ? 1 : 0;
assign white_detect = (Luminance > 140) ? 1 : 0;
//reg blackMet, whiteMet;
reg buildingDetect;
reg [6:0] alternatingCount;
reg [6:0] alternatingCountTemp;
reg [6:0] whiteCount;
reg [6:0] blackCount;


//New implementation using detection as mode filter
//This implementation requires looser requirements on the HSV thresholds
always @(posedge clk) begin
	integer i;
	integer j;
	colourOutput = {gray, gray, gray};
	//Reset all at start of frame
	if (sop & in_valid) begin
		for(i = 0; i < 6; i = i + 1) begin
			xMin[i] = 0;
			xMax[i] = 0;
			pixelWidth[i] = 0;
			//yMin[i] = 0;
			//yMax[i] = 0;
			//Replace current xmin and xmax
			//tempXMin[i] = IMAGE_W-11'h1;
		end
		buildingXMin = 0;
		buildingXMax = 0;
		buildingWidth = 0;
		alternatingCount = 0;
	end
	//Reset buffer at start of line
	if (x < pixelRange && in_valid) begin
		for(i = 0; i < 6; i = i + 1) begin
			pixelBuffer[i] = 0;
			tempXMax[i] = 0;
			tempPixelWidth[i] = 0;
		end
		//Buildings
		buildingWhitePixelBuffer = 0;
		buildingBlackPixelBuffer = 0;
		buildingtempXMax = 0;
		buildingtempXMin = 0;
		buildingtempWidth = 0;
		buildingDetect = 0;
		alternatingCountTemp = 0;
	end
	if ((x > pixelRange) && (x < IMAGE_W+1) && (y < IMAGE_H) && (in_valid)) begin
		//For each colour
		for(i = 0; i < 6; i = i + 1) begin
			//If a colour pixel was detected, add to buffers for that colour
			if(detectionArray[i] == 1) begin
				pixelBuffer[i][0] = 1;
			end
			//Count how many detected pixels 1s in buffer, if at least half the pixelrange is detected consider the pixel detected (mode filtering)
			tempCount = 0;
			for(j = 0; j < pixelRange; j = j + 1) begin
				if(pixelBuffer[i][j] == 1) begin
					tempCount = tempCount + 1;
				end
			end
			//Shift all buffers by 1 (shift register)
			pixelBuffer[i] = pixelBuffer[i]*2;
			//Mode filtering and contour detection
			if ((tempCount > detectionThreshold) && (x < IMAGE_W-1)) begin
				//whiteMet = 0;
				//blackMet = 0;
				//buildingWhitePixelBuffer = 0;
				//buildingBlackPixelBuffer = 0;
				//alternatingCount = 0;
				//If no pixels have been detected in the current row yet, set min to first detected pixel
				// if (tempXMin[i] > x) begin
				// 	tempXMin[i] = x;
 				// end
				tempPixelWidth[i] = tempPixelWidth[i] + 1;
				//Set x max to current pixel
				tempXMax[i] = x;
				//Colour output
				colourOutput = colourCodes[i];
			end else begin
				//Ball no longer detected, reset longest contour detection
				//If end of current detection
				if (tempPixelWidth[i] > pixelWidth[i]) begin
					xMin[i] = tempXMax[i]-tempPixelWidth[i];
					xMax[i] = tempXMax[i];
					pixelWidth[i] = tempPixelWidth[i];
				end
				//tempXMin[i] = 50;
				// tempXMax[i] = pixelRange;
				tempPixelWidth[i] = 0;
				// xMin[i] = (tempPixelWidth[i] > pixelWidth[i]) ? tempXMin[i] : xMin[i];
				// xMax[i] = (tempPixelWidth[i] > pixelWidth[i]) ? tempXMax[i] : xMax[i];
				// pixelWidth[i] = (tempPixelWidth[i] > pixelWidth[i]) ? tempPixelWidth[i] : pixelWidth[i]; 
				// //tempXMin[i] = IMAGE_W-1'b1;
				//tempXMax[i] = pixelRange;
				//tempPixelWidth[i] = 0;
				// tempMin[i] = 11'd0;
				// pixelWidth[i] = 11'd0;
			end
			//Should only draw at end of frame as y keeps changing
			//If the current coordinate is an xMin, xMax, yMin or yMax of another ball, colour accordingly
		end
		//Bulldings
		if(white_detect == 1) begin
			buildingWhitePixelBuffer[0] = 1;
		end
		if(black_detect == 1) begin
			buildingBlackPixelBuffer[0] = 1;
		end
		//Counting whitePixelBuffer
		whiteCount = 0;
		for(i = 0; i < stripLength; i = i + 1) begin
			if(buildingWhitePixelBuffer[i] == 1) begin
				whiteCount = whiteCount + 1;
			end
		end
		//Counting blackPixelBuffer
		blackCount = 0;
		for(i = 0; i < stripLength; i = i + 1) begin
			if(buildingBlackPixelBuffer[i] == 1) begin
				blackCount = blackCount + 1;
			end
		end
		// Shift registers
		buildingWhitePixelBuffer = buildingWhitePixelBuffer*2;
		buildingBlackPixelBuffer = buildingBlackPixelBuffer*2;
		//Overlap area between white and black pixels
		if ((blackCount > detectionWidth) && (whiteCount > detectionWidth)) begin
			colourOutput = 24'hffb652;
			if (buildingDetect == 1) begin
				alternatingCountTemp = alternatingCountTemp + 1;
				buildingtempXMax = x;
			end else begin
				buildingDetect = 1;
				alternatingCountTemp = 1;
				buildingtempXMin = x;
				buildingtempWidth = 0;
			end
		end
		//If either black or white pixels being detected and a building is being scanned, add to width
		if ((blackCount > detectionWidth) || (whiteCount > detectionWidth)) begin
			if (buildingDetect == 1) begin
				buildingtempWidth = buildingtempWidth + 1;
			end 
		end
		//If neither colour pixels have been found recently, a building is not being scanned
		if ((whiteCount < detectionWidth) && (blackCount < detectionWidth)) begin
			if(alternatingCountTemp > alternatingCount) begin
				//buildingXMin = buildingtempXMax - buildingtempWidth;
				buildingXMin = buildingtempXMin;
				buildingXMax = buildingtempXMax;
				buildingWidth = buildingtempWidth;
				alternatingCount = alternatingCountTemp;
			end
			buildingDetect = 0;
			buildingtempWidth = 0;
		end
		// //If the current coordinate is an xMin, xMax, yMin or yMax of another ball, colour accordingly
		for(i = 0; i < 6; i = i + 1) begin
			if ((left[i] == x) ^ (right[i] == x)) begin
				colourOutput = colourCodes[i];
			end
		end
		if ((buildingLeft == x) ^ (buildingRight == x)) begin
				colourOutput = 24'hFFFFFF;
			end
	end
end

//Send to ESP32
always@(posedge clk) begin
	//Receive any data from the ESP32
	if (SPI_read_valid) begin
		SPI_read_ready = 1;
	end else begin
		SPI_read_ready = 0;
		SPI_write_valid = 0;
		//SPI_dataretain = 0;
		SPI_dataout = 0;
	end
	//Recieved a command to output ball  data
	if (SPI_datain != 0) begin
		SPI_write_valid = 1;
		//SPI_dataretain = SPI_datain;
		//dataIndex = (SPI_dataretain-1)>>1;
		//Balls
		// if (dataIndex < 6) begin
		// 	angleCalc = ((left[dataIndex[2:0]]+right[dataIndex[2:0]])/40);
		// 	//SPI_dataout = ((SPI_dataretain % 2) == 1) ?  {dataIndex[2:0], pixelWidth[dataIndex][7:3]} : {pixelWidth[dataIndex][2:0], angleCalc[4:0]};
		// 	if((SPI_dataretain % 2) == 0) begin
		// 		//First half of packet
		// 		if (pixelWidth[dataIndex[2:0]] > 60) begin
		// 			SPI_dataout = {dataIndex[2:0]+1, pixelWidth[dataIndex[2:0]][8:4]};
		// 		end else begin
		// 			SPI_dataout = 0;
		// 		end
		// 	end else begin
		// 		//Second half of packet
		// 		if (pixelWidth[dataIndex[2:0]] > 60) begin
		// 			SPI_dataout = {pixelWidth[dataIndex[2:0]][3:1], 5'b11111};
		// 		end else begin
		// 			SPI_dataout = 0;
		// 		end
		// 	end
		// end
		if(SPI_datain == 1) begin //Red ball
			SPI_dataout = (pixelWidth[0] > 75) ? {3'b001, pixelWidth[0][8:4]} : 8'd0;
		end else if(SPI_datain == 2) begin
			angleCalc = ((left[0]+right[0])/40);
			SPI_dataout = (pixelWidth[0] > 75) ? {pixelWidth[0][3:1], angleCalc[4:0]} : 8'd0;
		end else if(SPI_datain == 3) begin //Green ball
			SPI_dataout = (pixelWidth[1] > 75) ? {3'b010, pixelWidth[1][8:4]} : 8'd0;
		end else if(SPI_datain == 4) begin
			angleCalc = ((left[1]+right[1])/40);
			SPI_dataout = (pixelWidth[1] > 75) ? {pixelWidth[1][3:1], angleCalc[4:0]} : 8'd0;
		end else if(SPI_datain == 5) begin //Blue ball
			SPI_dataout = (pixelWidth[2] > 75) ? {3'b011, pixelWidth[2][8:4]} : 8'd0;
		end else if(SPI_datain == 6) begin
			angleCalc = ((left[2]+right[2])/40);
			SPI_dataout = (pixelWidth[2] > 75) ? {pixelWidth[2][3:1], angleCalc[4:0]} : 8'd0;
		end else if(SPI_datain == 7) begin //Yellow
			SPI_dataout = (pixelWidth[3] > 75) ? {3'b100, pixelWidth[3][8:4]} : 8'd0;
		end else if(SPI_datain == 8) begin
			angleCalc = ((left[3]+right[3])/40);
			SPI_dataout = (pixelWidth[3] > 75) ? {pixelWidth[3][3:1], angleCalc[4:0]} : 8'd0;
		end else if(SPI_datain == 9) begin //Pink
			SPI_dataout = (pixelWidth[4] > 75) ? {3'b101, pixelWidth[4][8:4]} : 8'd0;
		end else if(SPI_datain == 10) begin
			angleCalc = ((left[4]+right[4])/40);
			SPI_dataout = (pixelWidth[4] > 75) ? {pixelWidth[4][3:1], angleCalc[4:0]} : 8'd0;
		end else if(SPI_datain == 11) begin //Light green
			SPI_dataout = (pixelWidth[5] > 75) ? {3'b110, pixelWidth[5][8:4]} : 8'd0;
		end else if(SPI_datain == 12) begin
			angleCalc = ((left[5]+right[5])/40);
			SPI_dataout = (pixelWidth[5] > 75) ? {pixelWidth[5][3:1], angleCalc[4:0]} : 8'd0;
		end else if(SPI_datain == 14) begin  //Buildings
			SPI_dataout = ((buildingWidth > 75) && (alternatingCount > 6)) ? {3'b111, buildingWidth[8:4]} : 8'd0;
		end else if(SPI_datain == 15) begin
			angleCalc = ((buildingLeft+buildingRight)/40);
			SPI_dataout = (pixelWidth[1] > 75) ? {buildingWidth[3:1], angleCalc[4:0]} : 8'd0;
		end else begin
			SPI_dataout = 0;
		end
	end else begin
		SPI_dataout = 0;
	end
end

// reg [8:0] hueDataBuffer [4:0]; //Store the last 5 pixel Hues
// reg [8:0] hueStack [4:0];
// reg [7:0] saturationDataBuffer [4:0]; 
// reg [7:0] saturationStack [4:0];
// reg [7:0] valueDataBuffer [4:0]; 
// reg [7:0] valueStack [4:0];
// reg [8:0] tempRegHue, compHue;
// reg [7:0] tempRegSat, tempRegVal, compSat, compVal;
// reg hueReplaced, satReplaced, valReplaced;
// localparam HueSteps = 16;
// localparam SatSteps = 4;
// localparam ValSteps = 2; 
// localparam HueIncr = 22;//360/HueSteps;
// localparam SatIncr = 63; //255/SatSteps;
// localparam ValIncr = 127; //255/ValSteps;


//Saving to data buffer
// always @(*) begin
// 	integer i;
// 	//Reset buffers at start of frame
// 	// if (sop) begin
// 	// 	for(i = 0; i < 5; i = i + 1) begin
// 	// 		hueDataBuffer[i] = 9'd0;
// 	// 		saturationDataBuffer[i] = 8'd0;
// 	// 		valueDataBuffer[i] = 8'd0;
// 	// 		hueStack[i] = 9'd0;
// 	// 		saturationStack[i] = 8'd0;
// 	// 		valueStack[i] = 8'd0;
// 	// 	end
// 	// end
// 	//Posterize/quantise values
// 	//If inbetween these two bounds, set it to whichever it is closer to
// 	// for(i = 0; i < HueSteps; i = i + 1) begin
// 	// 	//Hue
// 	// 	if ((Hue > i*HueIncr) && (Hue < (i+1)*HueIncr)) begin
// 	// 		if (Hue > (i*HueIncr)+HueIncr/2) begin
// 	// 			finalHue = (i+1)*HueIncr;
// 	// 		end else begin
// 	// 			finalHue = i*HueIncr;
// 	// 		end
// 	// 	end
// 	// 	//Sat
// 	// 	if(i < SatSteps) begin
// 	// 		if ((Saturation > i*SatIncr) && (Saturation < (i+1)*SatIncr)) begin
// 	// 			if (Saturation > (i*SatIncr)+SatIncr/2) begin
// 	// 				finalSat = (i+1)*SatIncr;
// 	// 			end else begin
// 	// 				finalSat = i*SatIncr;
// 	// 			end
// 	// 		end
// 	// 	end
// 	// 	//Val
// 	// 	if (i < ValSteps) begin
// 	// 		if ((Value > i*ValIncr) && (Value < (i+1)*ValIncr)) begin
// 	// 			if (Value > (i*ValIncr)+ValIncr/2) begin
// 	// 				finalVal = (i+1)*ValIncr;
// 	// 			end else begin
// 	// 				finalVal = i*ValIncr;
// 	// 			end
// 	// 		end
// 	// 	end
// 	// end
// 	//Save data to stacks and buffers
// 	hueReplaced = 0;
// 	satReplaced = 0;
// 	valReplaced = 0;
// 	finalHue = Hue;
// 	finalSat = Saturation[7:0];
// 	finalVal = Value;
// 	for(i = 0; i < 5; i = i + 1) begin
// 		if ((!hueReplaced) && (hueDataBuffer[i] == hueStack[x % 5])) begin
// 			hueDataBuffer[i] = finalHue;
// 			hueReplaced = 1;
// 		end
// 		if ((!satReplaced) && (saturationDataBuffer[i] == saturationStack[x % 5])) begin
// 			saturationDataBuffer[i] = finalSat;
// 			satReplaced = 1;
// 		end
// 		if ((!valReplaced) && (valueDataBuffer[i] == valueStack[x % 5])) begin
// 			valueDataBuffer[i] = finalVal;
// 			valReplaced = 1;
// 		end
// 	end 
// 	hueStack[x % 5] = finalHue;
// 	saturationStack[x % 5] = finalSat;
// 	valueStack[x % 5] = finalVal;
// 	for(i = 0; i < 4; i = i + 1) begin
// 		//Do one sweep of the arrays rightward and leftward to ensure they are sorted
// 		if(hueDataBuffer[i] > hueDataBuffer[i+1]) begin
// 			tempRegHue = hueDataBuffer[i];
// 			hueDataBuffer[i] = hueDataBuffer[i+1];
// 			hueDataBuffer[i+1] = tempRegHue;
// 		end
// 		if(saturationDataBuffer[i] > saturationDataBuffer[i+1]) begin
// 			tempRegSat = saturationDataBuffer[i];
// 			saturationDataBuffer[i] = saturationDataBuffer[i+1];
// 			saturationDataBuffer[i+1] = tempRegSat;
// 		end
// 		if(valueDataBuffer[i] > valueDataBuffer[i+1]) begin
// 			tempRegVal = valueDataBuffer[i];
// 			valueDataBuffer[i] = valueDataBuffer[i+1];
// 			valueDataBuffer[i+1] = tempRegVal;
// 		end
// 	end
// 	for(i = 4; i > 0; i = i - 1) begin
// 		//Do one sweep of the arrays rightward and leftward to ensure they are sorted
// 		if(hueDataBuffer[i] < hueDataBuffer[i-1]) begin
// 			tempRegHue = hueDataBuffer[i];
// 			hueDataBuffer[i] = hueDataBuffer[i-1];
// 			hueDataBuffer[i-1] = tempRegHue;
// 		end
// 		if(saturationDataBuffer[i] < saturationDataBuffer[i-1]) begin
// 			tempRegSat = saturationDataBuffer[i];
// 			saturationDataBuffer[i] = saturationDataBuffer[i-1];
// 			saturationDataBuffer[i-1] = tempRegSat;
// 		end
// 		if(valueDataBuffer[i] < valueDataBuffer[i-1]) begin
// 			tempRegVal = valueDataBuffer[i];
// 			valueDataBuffer[i] = valueDataBuffer[i-1];
// 			valueDataBuffer[i-1] = tempRegVal;
// 		end
// 	end
// 	//Data is now sorted across all HSV arrays, middle value is median, use threshold detection on that
// 	compHue = hueDataBuffer[2];
// 	compSat = saturationDataBuffer[2];
// 	compVal = valueDataBuffer[2];
// end
//Count valid pixels to tget the image coordinates. Reset and detect packet type on Start of Packet.
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
			//Sample every 4 pixels
			if((x % 4) == 0) begin 
				runningValueTotal <= runningValueTotal + Value;
			end
		end
	end
end

reg [1:0] msg_state;
reg [7:0] frame_count;
always@(posedge clk) begin
	integer i;
	//At end of frame
	if (eop & in_valid & packet_video) begin  //Ignore non-video packets
		//Start message writer FSM once every MSG_INTERVAL frames, if there is room in the FIFO
		frame_count <= frame_count - 1;
		if (frame_count == 0 && msg_buf_size < MESSAGE_BUF_MAX - 3) begin
			msg_state <= 2'b01;
			frame_count <= MSG_INTERVAL-1;
		end
		//Save all bounding boxes to show next frame
		//Simple low pass filter to prevent quick jumping around
		for(i = 0; i < 6; i = i + 1) begin
			if ((xMax[i] > xMin[i]) & (pixelWidth[i] > 75)) begin
				left[i] <= (left[i]+xMin[i])/2;
				right[i] <= (right[i]+xMax[i])/2;
			end
		end
		//Building bounds, only show if there are at least two white and black strips together
		if ((buildingWidth > 75) && (alternatingCount > 6)) begin
			buildingLeft <= (buildingLeft + buildingXMin)/2;
			buildingRight <= (buildingRight + buildingXMax)/2;
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

