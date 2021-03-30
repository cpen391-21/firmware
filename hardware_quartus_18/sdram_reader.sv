module sdram_reader(

    // 50MHz clock signals
    input clock_50,

    output [25:0] sdram_addr,
    output [1:0]  sdram_byteenable_n,
    output        sdram_chipselect,
    output [15:0] sdram_writedata,
    output        sdram_read_n,
    output        sdram_write_n,

    input  [15:0] sdram_readdata,
    input         sdram_readdata_valid,
    input         sdram_waitrequest,


    // 12MHz signals
    input clock_12,
    input reset_12,
    input [25:0] address,
    input request,
    output done,
    output [15:0] readdata,
);

logic sdram_readdata_valid_12;
logic sdram_waitrequest_12;
logic sdram_read_n_12;

doublesync doublesync_inst(
    .indata(sdram_readdata_valid),
    .outdata(sdram_readdata_valid_12),
    .clk(audio_clock),
    .reset(audio_reset)
);

doublesync doublesync_inst(
    .indata(sdram_waitrequest),
    .outdata(sdram_waitrequest_12),
    .clk(audio_clock),
    .reset(audio_reset)
);

// Outputs to 50Mhz FSM
doublesync doublesync_inst(
    .indata(sdram_read_n),
    .outdata(sdram_read_n_12),
    .clk(clock),
    .reset(reset)
);


enum {IDLE, WAITREQ, DONE} state;
initial state = IDLE;

always_ff @(posedge clock_12) begin

end



endmodule