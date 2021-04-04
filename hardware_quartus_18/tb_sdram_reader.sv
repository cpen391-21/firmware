`timescale 1ns/10ps

module tb_sdram_reader();

    logic clock_50;
    logic reset_50;

    logic [25:0] sdram_addr;
    logic [1:0]  sdram_byteenable_n;
    logic        sdram_chipselect;
    logic [15:0] sdram_writedata;
    logic        sdram_read_n;
    logic        sdram_write_n;

    logic  [15:0] sdram_readdata;
    logic         sdram_readdata_valid;
    logic         sdram_waitrequest;

    logic clock_12;
    logic reset_12;
    logic [25:0] address_12;
    logic request_12;
    logic done_12;
    logic [15:0] readdata_12;

    initial begin
        clock_50 = 0;
        clock_12 = 0;
    end

    // Main clock (50Mhz) has a period of 20ns
    always begin
        #10 clock_50 = ~clock_50;
    end

    // Audio clock (12Mhz) has a period of 83.33ns
    always begin
        #41.666 clock_12 = ~clock_12;
    end

    sdram_reader dut(.*);

    initial begin
        reset_50 = 1'b0;
        clock_50 = 1'b0;
        #5;

        sdram_readdata = 16'h010F;
        sdram_readdata_valid = 0;
        sdram_waitrequest = 1;

        address_12 = 26'hFF02;
        request_12 = 1;
        #20;

        request_12 = 0;

        #20;

        #100;
        sdram_waitrequest = 0;
        #60;
        sdram_waitrequest = 1;
        sdram_readdata_valid = 1;
        #20 sdram_readdata_valid = 0;

        // We should now have read the data
        assert(sdram_readdata === readdata_12);

    end

endmodule