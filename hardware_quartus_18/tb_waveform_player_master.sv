`timescale 1ns/10ps

module tb_waveform_player();

    reg clock;
    reg audio_clock;

    reg reset_n;
    reg audio_reset;

    // Inputs to waveform player
    reg unsigned address;
    reg write;
    reg unsigned [31:0] writedata;

    reg                         l_audio_ready;
    reg                         r_audio_ready;

    // Outputs from waveform player
    reg unsigned [15:0]  l_audio_data;
    reg                  l_audio_valid;

    reg unsigned [15:0]  r_audio_data;
    reg                  r_audio_valid;


    // Avalon Memory-Mapped SDRAM controller connections
    reg [25:0] sdram_addr;
    reg [1:0]  sdram_byteenable_n;
    reg        sdram_chipselect;
    reg [15:0] sdram_writedata;
    reg        sdram_read_n;
    reg        sdram_write_n;

    reg  [15:0] sdram_readdata;
    reg         sdram_readdata_valid;
    reg         sdram_waitrequest;

    waveform_player dut(.*);

    initial begin
        clock = 0;
        audio_clock = 0;
        reset_n = 1'b1;
        audio_reset = 1'b0;
    end

    // Main clock (50Mhz) has a period of 20ns
    always begin
        #10 clock = ~clock;
    end

    // Audio clock (12Mhz) has a period of 83.33ns
    always begin
        #41.666 audio_clock = ~audio_clock;
    end

    // Audio will be ready for one audio_clock cycle out of 10
    always begin
        #83.33
        l_audio_ready = 0;
        r_audio_ready = 0;
        #(83.33 * 9)
        l_audio_ready = 1;
        r_audio_ready = 1;
    end

    initial begin
        #20;

        address = 0;
        write = 1;
        writedata = 32'h3;

        #20;

        assert (dut.max_audio_index === writedata);
        address = 1;
        writedata = 32'b1;

        #20;
        $write("Address: %u", dut.address);
        write = 0;

        #2000;

        write = 1;
        writedata = 32'b0;

        #20 write = 0;
        #2000;

        write = 1;
        writedata = 32'b1;

        #20;

        write = 0;




    end




endmodule