// Trying to merge the MyComputer_Verilog and rhe UART control from the example ??

module MyComputer_Verilog_UART (
		/////////////////////////////////////////////
		// FPGA Pins
		/////////////////////////////////////////////
	
		// Clock pins
		input CLOCK_50,CLOCK2_50,CLOCK3_50,CLOCK4_50	, 
		
		// Seven Segment Displays
		// These are the names of the 6 seven segment display on the DE1 and those in the PIN Planner,
		//	so stick to these names.
		output unsigned [6:0] HEX0,HEX1,HEX2,HEX3,HEX4,HEX5,
	
		// Pushbuttons
		input unsigned [3:0] KEY,
	
		// LEDs
		output unsigned [9:0] LEDR,
	
		// Slider Switches
		input unsigned [9:0] SW,
		
		// VGA/Graphics Signals
		output VGA_BLANK_N,
		output VGA_SYNC_N,
		output VGA_CLK,
		output VGA_HS,
		output VGA_VS,
		
		output unsigned [7:0] VGA_B,
		output unsigned [7:0] VGA_G,
		output unsigned [7:0] VGA_R,
	
		// SDRAM on FPGA Side
		output unsigned [12:0] DRAM_ADDR,
		output unsigned [1:0] DRAM_BA,
		output DRAM_CAS_N,
		output DRAM_CKE,
		output DRAM_CLK,
		output DRAM_CS_N,
		inout unsigned [15:0] DRAM_DQ,
		output DRAM_LDQM,
		output DRAM_RAS_N,
		output DRAM_UDQM,
		output DRAM_WE_N,
		
		// 40-Pin Headers
		inout unsigned [35:0] GPIO_0,
		inout unsigned [35:0] GPIO_1,
		

		//////////////////////////////////////////////////////
		// HPS Pins
		//////////////////////////////////////////////////////
		
		// DDR3 SDRAM
		output unsigned [14:0] HPS_DDR3_ADDR,
		output unsigned [2:0] HPS_DDR3_BA,
		output HPS_DDR3_CAS_N,
		output HPS_DDR3_CKE,
		output HPS_DDR3_CK_N,
		output HPS_DDR3_CK_P,
		output HPS_DDR3_CS_N,
		output unsigned [3:0] HPS_DDR3_DM,
		inout unsigned [31:0] HPS_DDR3_DQ,
		inout unsigned [3:0] HPS_DDR3_DQS_N,
		inout unsigned [3:0] HPS_DDR3_DQS_P,
		output HPS_DDR3_ODT,
		output HPS_DDR3_RAS_N,
		output HPS_DDR3_RESET_N	,
		input HPS_DDR3_RZQ,
		output HPS_DDR3_WE_N,
		
		// Ethernet
		output HPS_ENET_GTX_CLK,
		inout HPS_ENET_INT_N,
		output HPS_ENET_MDC,
		inout HPS_ENET_MDIO,
		input HPS_ENET_RX_CLK,
		input unsigned [3:0] HPS_ENET_RX_DATA,
		input HPS_ENET_RX_DV,
		output unsigned [3:0] HPS_ENET_TX_DATA,
		output HPS_ENET_TX_EN,
	
		// Flash
		inout unsigned [3:0] HPS_FLASH_DATA,
		output HPS_FLASH_DCLK,
		output HPS_FLASH_NCSO,
	
		// Accelerometer
		inout HPS_GSENSOR_INT,
			
		// General Purpose I/O
		inout unsigned [1:0] HPS_GPIO,
		
		// I2C
		inout HPS_I2C_CONTROL,
		inout HPS_I2C1_SCLK,
		inout HPS_I2C1_SDAT,
		inout HPS_I2C2_SCLK,
		inout HPS_I2C2_SDAT,
	
		// Pushbutton
		inout HPS_KEY,
	
		// LED
		inout HPS_LED,
			
		// SD Card
		output HPS_SD_CLK,
		inout HPS_SD_CMD,
		inout unsigned [3:0] HPS_SD_DATA,
	
		// SPI
		output HPS_SPIM_CLK,
		input HPS_SPIM_MISO,
		output HPS_SPIM_MOSI,
		inout HPS_SPIM_SS,
	
		// UART
		input HPS_UART_RX,
		output HPS_UART_TX,
	
		// USB
		inout HPS_CONV_USB_N,
		input HPS_USB_CLKOUT,
		inout unsigned [7:0] HPS_USB_DATA,
		input HPS_USB_DIR,
		input HPS_USB_NXT,
		output HPS_USB_STP 
	 );


// BEGIN bluetooth
//=======================================================
//  REG/WIRE declarations
//=======================================================

wire BT_UART_TX, BT_UART_RX;
assign BT_UART_TX = GPIO_1[18]; // physical pin 21
assign BT_UART_RX = GPIO_1[19]; // physical pin 22

wire       rts; // request to send
wire       cts; // clear to send
wire       rxd;
wire       txd;
wire [7:0] uart_data;
wire       rdempty;
wire       write;
reg	       read;
reg	       cnt;

reg [9:0] LED;
assign LEDR = LED;
//=======================================================
//  Structural coding
//=======================================================

// UART Controller
uart_control UART0(

	.clk(CLOCK_50),
	.reset_n(KEY[0]),
	// tx
	.write(write),
	.writedata(uart_data),

	// rx
	.read(read),
	.readdata(uart_data),
	.rdempty(rdempty),
	//
	.uart_clk_25m(cnt),
	.uart_tx(BT_UART_TX),
	.uart_rx(BT_UART_RX)
);

//read
always@(posedge CLOCK_50)
begin
  if (~rdempty)
		read <= 1;
  else
		read <= 0;
end
assign  write = ( read & (~rdempty) );

always@(posedge CLOCK_50 or negedge KEY[0])
begin
  if(!KEY[0])
    LED <= 0;
  else if(KEY[0] & write)
  begin
    case(uart_data)
	 10'h30:LED <= LED | 8'd1;
	 10'h31:LED <= LED | 8'd2;
	 10'h32:LED <= LED | 8'd4;
	 10'h33:LED <= LED | 8'd8;
	 10'h34:LED <= LED & 8'he;
	 10'h35:LED <= LED & 8'hd;
	 10'h36:LED <= LED & 8'hb;
	 10'h37:LED <= LED & 8'h7;
	 10'h38:LED <= LED | 8'hf;
	 10'h39:LED <= LED & 8'h0;
	 default : LED <= LED;
	 endcase
  end
  else
    LED <= LED;
end

always@(posedge CLOCK_50)
	cnt <= cnt + 1;
// END bluetooth
 

	
endmodule
