
module Qsys (
	alt_vip_itc_0_clocked_video_vid_clk,
	alt_vip_itc_0_clocked_video_vid_data,
	alt_vip_itc_0_clocked_video_underflow,
	alt_vip_itc_0_clocked_video_vid_datavalid,
	alt_vip_itc_0_clocked_video_vid_v_sync,
	alt_vip_itc_0_clocked_video_vid_h_sync,
	alt_vip_itc_0_clocked_video_vid_f,
	alt_vip_itc_0_clocked_video_vid_h,
	alt_vip_itc_0_clocked_video_vid_v,
	altpll_0_areset_conduit_export,
	altpll_0_locked_conduit_export,
	clk_clk,
	clk_sdram_clk,
	clk_vga_clk,
	d8m_xclkin_clk,
	eee_imgproc_0_conduit_mode_1_new_signal,
	i2c_opencores_camera_export_scl_pad_io,
	i2c_opencores_camera_export_sda_pad_io,
	i2c_opencores_mipi_export_scl_pad_io,
	i2c_opencores_mipi_export_sda_pad_io,
	key_external_connection_export,
	led_external_connection_export,
	mipi_pwdn_n_external_connection_export,
	mipi_reset_n_external_connection_export,
	reset_reset_n,
	sdram_wire_addr,
	sdram_wire_ba,
	sdram_wire_cas_n,
	sdram_wire_cke,
	sdram_wire_cs_n,
	sdram_wire_dq,
	sdram_wire_dqm,
	sdram_wire_ras_n,
	sdram_wire_we_n,
	spi_rx_fifo_out_data,
	spi_rx_fifo_out_valid,
	spi_rx_fifo_out_ready,
	spi_tx_fifo_in_data,
	spi_tx_fifo_in_valid,
	spi_tx_fifo_in_ready,
	spislave_0_export_0_mosi,
	spislave_0_export_0_nss,
	spislave_0_export_0_miso,
	spislave_0_export_0_sclk,
	sw_external_connection_export,
	terasic_auto_focus_0_conduit_vcm_i2c_sda,
	terasic_auto_focus_0_conduit_clk50,
	terasic_auto_focus_0_conduit_vcm_i2c_scl,
	terasic_camera_0_conduit_end_D,
	terasic_camera_0_conduit_end_FVAL,
	terasic_camera_0_conduit_end_LVAL,
	terasic_camera_0_conduit_end_PIXCLK,
	uart_0_rx_tx_rxd,
	uart_0_rx_tx_txd,
	eee_imgproc_0_conduit_spi_new_signal,
	eee_imgproc_0_conduit_spi_new_signal_1,
	eee_imgproc_0_conduit_spi_new_signal_2);	

	input		alt_vip_itc_0_clocked_video_vid_clk;
	output	[23:0]	alt_vip_itc_0_clocked_video_vid_data;
	output		alt_vip_itc_0_clocked_video_underflow;
	output		alt_vip_itc_0_clocked_video_vid_datavalid;
	output		alt_vip_itc_0_clocked_video_vid_v_sync;
	output		alt_vip_itc_0_clocked_video_vid_h_sync;
	output		alt_vip_itc_0_clocked_video_vid_f;
	output		alt_vip_itc_0_clocked_video_vid_h;
	output		alt_vip_itc_0_clocked_video_vid_v;
	input		altpll_0_areset_conduit_export;
	output		altpll_0_locked_conduit_export;
	input		clk_clk;
	output		clk_sdram_clk;
	output		clk_vga_clk;
	output		d8m_xclkin_clk;
	input		eee_imgproc_0_conduit_mode_1_new_signal;
	inout		i2c_opencores_camera_export_scl_pad_io;
	inout		i2c_opencores_camera_export_sda_pad_io;
	inout		i2c_opencores_mipi_export_scl_pad_io;
	inout		i2c_opencores_mipi_export_sda_pad_io;
	input	[1:0]	key_external_connection_export;
	output	[9:0]	led_external_connection_export;
	output		mipi_pwdn_n_external_connection_export;
	output		mipi_reset_n_external_connection_export;
	input		reset_reset_n;
	output	[12:0]	sdram_wire_addr;
	output	[1:0]	sdram_wire_ba;
	output		sdram_wire_cas_n;
	output		sdram_wire_cke;
	output		sdram_wire_cs_n;
	inout	[15:0]	sdram_wire_dq;
	output	[1:0]	sdram_wire_dqm;
	output		sdram_wire_ras_n;
	output		sdram_wire_we_n;
	output	[7:0]	spi_rx_fifo_out_data;
	output		spi_rx_fifo_out_valid;
	input		spi_rx_fifo_out_ready;
	input	[7:0]	spi_tx_fifo_in_data;
	input		spi_tx_fifo_in_valid;
	output		spi_tx_fifo_in_ready;
	input		spislave_0_export_0_mosi;
	input		spislave_0_export_0_nss;
	inout		spislave_0_export_0_miso;
	input		spislave_0_export_0_sclk;
	input	[9:0]	sw_external_connection_export;
	inout		terasic_auto_focus_0_conduit_vcm_i2c_sda;
	input		terasic_auto_focus_0_conduit_clk50;
	inout		terasic_auto_focus_0_conduit_vcm_i2c_scl;
	input	[11:0]	terasic_camera_0_conduit_end_D;
	input		terasic_camera_0_conduit_end_FVAL;
	input		terasic_camera_0_conduit_end_LVAL;
	input		terasic_camera_0_conduit_end_PIXCLK;
	input		uart_0_rx_tx_rxd;
	output		uart_0_rx_tx_txd;
	output	[7:0]	eee_imgproc_0_conduit_spi_new_signal;
	output		eee_imgproc_0_conduit_spi_new_signal_1;
	input		eee_imgproc_0_conduit_spi_new_signal_2;
endmodule
