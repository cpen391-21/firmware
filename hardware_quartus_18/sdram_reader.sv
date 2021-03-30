module sdram_reader(

    // 50MHz clock signals
    input clock_50,
    input reset_50,

    output [25:0] sdram_addr,
    output [1:0]  sdram_byteenable_n,
    output        sdram_chipselect,
    output [15:0] sdram_writedata,
    output        sdram_read_n,
    output        sdram_write_n,

    input  [15:0] sdram_readdata,
    input         sdram_readdata_valid,
    input         sdram_waitrequest,


    // 12MHz signals will be prepended with _12 for clarity.
    input clock_12,
    input reset_12,
    input [25:0] address_12,
    input request_12,
    output logic done_12,
    output logic [15:0] readdata_12
);


// 12 MHz to 50MHz sync =============================================
logic request;

doublesync d0(
    .indata(request_12),
    .outdata(request),
    .clk(clock_50),
    .reset(reset_50)
);

doublesync #(.WIDTH(26)) d1(
    .indata(address_12),
    .outdata(sdram_addr),
    .clk(clock_50),
    .reset(reset_50)
);


// 50MHz to 12MHz sync ==============================================
logic done;
logic [15:0] readdata;

doublesync d2(
    .indata(done),
    .outdata(done_12),
    .clk(clock_12),
    .reset(reset_12)
);

doublesync #(.WIDTH(16)) d3(
    .indata(readdata),
    .outdata(readdata_12),
    .clk(clock_12),
    .reset(reset_12)
);

// 50MHz SDRAM FSM ==================================================
logic read;
assign sdram_byteenable_n = 2'b0;
assign sdram_chipselect   = 1'b1;
assign sdram_writedata    = 16'b0;
assign sdram_write_n      = 1'b1;
assign sdram_read_n       = ~read;


enum {IDLE, WAIT_REQ, WAIT_DATA_VALID, STORE_DATA, DONE} state;
initial state = IDLE;
initial read = 1'b0;
initial done = 0;
initial readdata = 0;

parameter max_done_cycles = 10;
int unsigned done_cycles;
initial done_cycles = 0;

always_ff @(posedge clock_50) begin
case (state)
    IDLE: if (request) begin
        state <= WAIT_REQ;
        read <= 1;
        done <= 0;
    end

    // If sdram_waitrequest is low, then we HAVE successfuly sent
    // the request to SDRAM.
    WAIT_REQ: if (!sdram_waitrequest) begin
        state <= WAIT_DATA_VALID;
        read  <= 0;
    end

    WAIT_DATA_VALID: if (sdram_readdata_valid) begin
        state <= DONE;
        done <= 1;
        readdata <= sdram_readdata; // METASTABLE!
    end

    DONE: if (done_cycles + 1 == max_done_cycles) begin
        state <= IDLE;
        done <= 0;
        done_cycles <= 0;
    end else begin
        done_cycles <= done_cycles + 1;
    end

endcase
end



endmodule