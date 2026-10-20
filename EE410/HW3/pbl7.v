module top_module ();
// Start the waveform tracing/timing diagram
  initial `probe_start;

  // DUT inputs (registers)
  reg a;
  reg b;
  reg c;
  reg d;

  // DUT output (wire)
  wire e;

  // Expected logic evaluation
  reg expected_e;

  // Loop index & error tracker
  integer i;
  integer error_count = 0;

  // Probe all input and output signals for waveform dump
  `probe(a);
  `probe(b);
  `probe(c);
  `probe(d);
  `probe(e);

  // Instantiate the Device Under Test (DUT)
  com_and_or uut (
    .a(a),
    .b(b),
    .c(c),
    .d(d),
    .e(e)
  );

  initial begin
    // Loop through all 16 combinations (0000 to 1111)
    for (i = 0; i < 16; i = i + 1) begin
      {a, b, c, d} = i[3:0];
      expected_e = (a & b) | (c | d);

      #10; // Allow combinational propagation

      if (e !== expected_e) begin
        $display("ERROR at %0t: a=%b b=%b c=%b d=%b | Got: e=%b, Expected: %b",
                 $time, a, b, c, d, e, expected_e);
        error_count = error_count + 1;
      end else begin
        $display("PASS at %0t: a=%b b=%b c=%b d=%b | e=%b",
                 $time, a, b, c, d, e);
      end
    end

    // Summary report
    $display("--------------------------------------------------");
    if (error_count == 0) begin
      $display("SUCCESS: All 16 combinations passed without errors.");
    end else begin
      $display("FAILURE: %0d error(s) detected out of 16 tests.", error_count);
    end
    $display("--------------------------------------------------");

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
