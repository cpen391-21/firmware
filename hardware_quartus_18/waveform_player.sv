/*
 *  Memory-mapped programming instructions:
 *    * The first 32-bits, corresponding to address 0, is a WRITE
 *      Field to tell this module how many samples it should play.
 *
 *    * The second 32-bits, corresponding to address 1, contains the
 *      START(S) and RESTART(R) flags. This field is WRITE only.
 *      Bits 2-31 are reserved for future use.
 *
 *
 * Offset          3                   2                   1                   0
 * (bits):       1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *              +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * address = 0: |           length (number of 16-bit samples to play)           |
 *              +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * address = 1: |                Reserved for Future use                    |R|S|
 *              +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */
module waveform_player(

    // Avalon Memory-Mapped Connections
    input clock,
    input reset_n,

    input unsigned address,
    input write,

    input unsigned [31:0] writedata,


    // Avalon Memory-Mapped SDRAM controller connections
    output [25:0] sdram_addr,
    output [1:0]  sdram_byteenable_n,
    output        sdram_chipselect,
    output [15:0] sdram_writedata,
    output        sdram_read_n,
    output        sdram_write_n,

    input  [15:0] sdram_readdata,
    input         sdram_readdata_valid,
    input         sdram_waitrequest,

    // Avalaon Streaming Connections
    // (To Audio Core)

    // Avalon streaming interfaces must be on the same clock! So we will sync
    // Commands to the clocks here.
    input audio_clock,
    input audio_reset,

    output logic unsigned [15:0]  l_audio_data,
    output logic                  l_audio_valid,
    input                         l_audio_ready,

    output logic unsigned [15:0]  r_audio_data,
    output logic                  r_audio_valid,
    input                         r_audio_ready
);


// Avalon Memory-Mapped Slave controller (50MHz FSM) ================
logic [31:0] max_audio_index;
logic start;

initial start = 0;

always_ff @(posedge clock) begin
    if (reset_n == 0) begin
        start <= 0;
    end

    // memory mapped interface
    else if (write) begin
        if (address == 0) begin
            max_audio_index <= writedata;
        end

        else begin
            start <= writedata[0];
        end
    end
end


// Inputs to 12MHz FSM from 50MHz FSM ===============================
logic        start_sync;

doublesync d0(
    .indata(start),
    .outdata(start_sync),
    .clk(audio_clock),
    .reset(audio_reset)
);

logic [31:0] max_index_sync;

doublesync #(.WIDTH(32)) d1(
    .indata(max_audio_index),
    .outdata(max_index_sync),
    .clk(audio_clock),
    .reset(audio_reset)
);

// Syncronization and control of SDRAM module from 12MHz FSM ========
logic [25:0] sdram_address;
logic request;
logic done;
logic [15:0] readdata;

sdram_reader sdram_reader_inst(
    .clock_50(clock),
    .reset_50(~reset_n),

    .sdram_addr(sdram_addr),
    .sdram_byteenable_n(sdram_byteenable_n),
    .sdram_chipselect(sdram_chipselect),
    .sdram_writedata(sdram_writedata),
    .sdram_read_n(sdram_read_n),
    .sdram_write_n(sdram_write_n),

    .sdram_readdata(sdram_readdata),
    .sdram_readdata_valid(sdram_readdata_valid),
    .sdram_waitrequest(sdram_waitrequest),

    .clock_12(audio_clock),
    .reset_12(audio_reset),
    .address_12(sdram_address),
    .request_12(request),
    .done_12(done),
    .readdata_12(readdata)
);



// 12MHz FSM ========================================================
enum {IDLE, WRITE_AUDIO, UPDATE_INDEX, SDRAM_REQ, SDRAM_DELAY,
      SDRAM_WAIT} state;

initial state = UPDATE_INDEX;

initial sdram_address = 0;

always_ff @(posedge audio_clock) begin

  // If we have not received the start flag from the waveform_player's slave
  // interface, we do nothing!
  if (audio_reset == 1 || !start_sync) begin
      l_audio_valid <= 0;
      r_audio_valid <= 0;

      request <= 0;

      sdram_address  <= 0;

      state <= UPDATE_INDEX;
  end

  else case (state)

    IDLE: if (l_audio_ready && r_audio_ready) begin
        l_audio_valid <= 1;
        r_audio_valid <= 1;
        state <= WRITE_AUDIO;
    end

    WRITE_AUDIO: begin
      l_audio_valid <= 0;
      r_audio_valid <= 0;
      state <= UPDATE_INDEX;
    end

    UPDATE_INDEX: begin
      if (sdram_address+1 >= max_index_sync) begin
        sdram_address <= 0;
      end

      else begin
        sdram_address <= sdram_address + 1'b1;
      end

      state <= SDRAM_REQ;
      request <= 1;
    end

    SDRAM_REQ: begin
      state <= SDRAM_DELAY;
      request <= 0;
    end

    // One clock delay so done variable
    // has time to initialize past the doublesync
    SDRAM_DELAY: state <= SDRAM_WAIT;

    SDRAM_WAIT: if (done) begin
      l_audio_data <= readdata;
      r_audio_data <= readdata;
      state <= IDLE;
    end

  endcase

end

endmodule