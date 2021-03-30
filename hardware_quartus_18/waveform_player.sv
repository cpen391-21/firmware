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

    // Avalon Memory-Mapped SDRAM controller connections
assign sdram_addr = 1'b0;
assign sdram_byteenable_n = 1'b0;
assign sdram_chipselect = 1'b1;
assign sdram_writedata = 1'b0;
assign sdram_read_n = 1'b1;
assign sdram_write_n = 1'b1;

// Currently unsafe. Doublesync this as well.
logic [31:0] max_audio_index;
logic start;

initial start = 0;

initial l_audio_data = 0;
initial r_audio_data = 0;

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

// BEGIN: Audio player FSM
assign sdram_byteenable_n = 2'b11;
assign sdram_chipselect   = 1'b1;
assign sdram_writedata    = 16'b0;
assign sdram_write_n      = 1'b1;

// Inputs to FSM
logic        start_sync;
logic [15:0] sdram_readdata_sync;
logic        sdram_readdata_valid_sync;
logic        sdram_waitrequest_sync;

// FSM outputs
logic        [25:0] sdram_addr_sync;
logic        sdram_read_n_sync;

// UNSAFE!!
assign sdram_readdata_sync = sdram_readdata;
assign sdram_addr = sdram_addr_sync;

// Inputs to 12Mhz FSM
doublesync doublesync_inst(
    .indata(start),
    .outdata(start_sync),
    .clk(audio_clock),
    .reset(audio_reset)
);

parameter WAITING       = 3'b000;
parameter WRITE_AUDIO   = 3'b001;
parameter UPDATE_SAMPLE = 3'b010;

logic [2:0] state;

initial state = WAITING;

assign l_audio_valid = state[0];
assign r_audio_valid = state[0];

logic [31:0] audio_index;
initial audio_index = 1;

always_ff @(posedge audio_clock) begin

  if (audio_reset == 1) begin
      l_audio_data <= 0;
      r_audio_data <= 0;
      audio_index  <= 0;
  end

  // If we have not received the start flag from the waveform_player's slave
  // interface, we do nothing!
  else if (!start_sync) begin
      audio_index <= 0;
      state <= WAITING;
  end

  else case (state)
      WAITING: if (l_audio_ready && r_audio_ready) state <= WRITE_AUDIO;

      WRITE_AUDIO: state <= UPDATE_SAMPLE;

      UPDATE_SAMPLE: begin
          if (audio_index+1 >= max_audio_index) begin
              audio_index <= 0;
              l_audio_data <= ~l_audio_data;
              r_audio_data <= ~r_audio_data;
          end

          else begin
              audio_index <= audio_index + 1'b1;
          end

          state <= WAITING;
      end
  endcase

end

endmodule