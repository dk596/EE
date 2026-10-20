module top_module ();
// Start the waveform tracing/timing diagram
initial `probe_start;

  reg clk;
  reg rst;
  reg a, b, c, d;
  wire f;

  integer i;
  integer error_count = 0;

  `probe(clk);
  `probe(rst);
  `probe(a);
  `probe(b);
  `probe(c);
  `probe(d);
  `probe(f);

  // Instantiate DUT
  seq_and_or uut (
    .clk(clk),
    .rst(rst),
    .a(a),
    .b(b),
    .c(c),
    .d(d),
    .f(f)
  );

  // Generate continuous clock
  initial clk = 0;
  always #5 clk = ~clk;

  initial begin
    // 1. Reset check
    rst = 0;
    a = 0; b = 0; c = 0; d = 0;
    #10;
    rst = 1;

    // 2. Test all 16 input combinations
    for (i = 0; i < 16; i = i + 1) begin
      {a, b, c, d} = i[3:0];

      // Wait 1 clock cycle for register to capture input
      #10;

      // Check if output matches (a & b) | (c | d)
      if (f !== ((a & b) | (c | d))) begin
        $display("ERROR: a=%b b=%b c=%b d=%b | Got: %b, Expected: %b", 
                 a, b, c, d, f, ((a & b) | (c | d)));
        error_count = error_count + 1;
      end
    end

    // Summary
    if (error_count == 0)
      $display("SUCCESS: Reset and all 16 cases passed!");
    else
      $display("FAILURE: %0d error(s) found.", error_count);

    $finish;
  end

endmodule


module com_and_or (
    input a,
    input b,
    input c,
    input d,
    output e
);

    wire w1;
    wire w2;
    assign w1 = a & b;
    assign w2 = c | d;
    assign e = w1 | w2;

endmodule

module seq_and_or (
    input  clk,
    input  rst,
    input a,
    input b,
    input c,
    input d,
    output reg f
);

    wire e;
    com_and_or test2 (.a(a), .b(b), .c(c), .d(d), .e(e));
    
    always @(posedge clk or negedge rst) begin
        if (!rst)
            f <= 1'b0;
        else
            f <= e;
    end

endmodule
