module doublesync
#(parameter WIDTH = 1) (
	indata,
	outdata,
	clk,
	reset
);

input [WIDTH-1:0] indata;
input clk;
input reset;

output [WIDTH-1:0] outdata;

reg [WIDTH-1:0] reg1;
reg [WIDTH-1:0] reg2;

always @(posedge clk or posedge reset)
begin
	 if (reset) begin
	 	reg1 <= {WIDTH{1'b0}};
		reg2 <= {WIDTH{1'b0}};
	 end

	 else begin
	 	reg1 <= indata;
		reg2 <= reg1;
	 end
end

assign outdata = reg2;

endmodule
