`timescale 1ns/10ps

module doublesync_tb();

	reg clock;
	reg audio_clock;

    initial begin
        clock = 0;
        audio_clock = 0;
    end

    // Main clock (50Mhz) has a period of 20ns
    always begin
        #10 clock = ~clock;
    end

    // Audio clock (12Mhz) has a period of 83.33ns
    always begin
        #41.666 audio_clock = ~audio_clock;
    end

    reg indata;
    reg outdata;

	doublesync single(
        .indata(indata),
        .outdata(outdata),
        .clk(audio_clock),
        .reset(1'b0)
	);

    reg [7:0] multi_in;
    reg [7:0] multi_out;

	doublesync #(.WIDTH(8)) multi(
        .indata(multi_in),
        .outdata(multi_out),
        .clk(audio_clock),
        .reset(1'b0)
	);

    initial begin
        #20;

        indata = 1;
        multi_in = 8'b11110001;

        #60;

        assert (multi_in === multi_out);
        assert (indata === outdata);
    end

endmodule
