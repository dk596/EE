module top_module();
initial `probe_start;

  // DUT inputs
  reg clk;
  reg rst;
  reg a, b, c, d, e, f, g;

  // DUT output
  wire n;

  // Golden model calculation variables
  reg exp_h, exp_i, exp_j, exp_k, exp_l, exp_m;
  
  integer idx;
  integer error_count = 0;

  // Probes
  `probe(clk);
  `probe(rst);
  `probe(a); `probe(b); `probe(c); `probe(d);
  `probe(e); `probe(f); `probe(g);
  `probe(n);

  // Instantiate the top system
  seq_inst_and_or test (
    .clk(clk),
    .rst(rst),
    .a(a), .b(b), .c(c), .d(d),
    .e(e), .f(f), .g(g),
    .n(n)
  );

  // Clock generation 
  initial clk = 0;
  always #5 clk = ~clk;

  initial begin
    // ----------------------------------------------------------------
    // Step 1: Verify Asynchronous Reset
    // ----------------------------------------------------------------
    rst = 0; // Assert active-low reset
    {a, b, c, d, e, f, g} = 7'b0;
    #15;
    rst = 1; // Release reset
    #5;

    if (n !== 1'b0) begin
      $display("RESET ERROR: Output n did not reset to 0!");
      error_count = error_count + 1;
    end

    // ----------------------------------------------------------------
    // Step 2: Test all 128 input combinations across 2 pipeline stages
    // ----------------------------------------------------------------
    for (idx = 0; idx < 128; idx = idx + 1) begin
      // Apply input pattern on falling edge to guarantee setup margin
      @(negedge clk);
      {a, b, c, d, e, f, g} = idx[6:0];

      // --- Golden Reference Stage 1 ---
      exp_h = (a & b) | (c | d);
      exp_i = (c & d);

      // Advance to Clock Edge 1 (registers test3 and test4 latch h and i)
      @(posedge clk);
      exp_j = exp_h;
      exp_k = exp_i;

      // Combinational logic between stage 1 and stage 2 resolves
      exp_l = (exp_k & e) | (f | g);
      exp_m = exp_j ^ exp_l;

      // Advance to Clock Edge 2 (register test5 latches m into n)
      @(posedge clk);
      #1; // Allow delta delay for register output to settle

      // Validate DUT output n against golden reference exp_m
      if (n !== exp_m) begin
        $display("FAIL at time %0t: inputs {a..g}=%b | Got n=%b, Expected=%b",
                 $time, idx[6:0], n, exp_m);
        error_count = error_count + 1;
      end
    end

    // ----------------------------------------------------------------
    // Step 3: Verification Verdict
    // ----------------------------------------------------------------
    $display("--------------------------------------------------");
    if (error_count == 0) begin
      $display("SUCCESS: Exhaustive verification complete. All 128 pipeline cases passed!");
    end else begin
      $display("FAILURE: %0d error(s) detected out of 128 vectors.", error_count);
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

module seq_inst_and_or (
    input  clk,
    input  rst,
    input a,
    input b,
    input c,
    input d,
    input e,
    input f,
    input g,
    output n
);

    wire h;
    wire i;
    wire j;
    wire k;
    wire l;
    wire m;
    com_and_or test1 (.a(a), .b(b), .c(c), .d(d), .e(h));
    com_and_or test2 (.a(k), .b(e), .c(f), .d(g), .e(l));
    assign i = c & d;
    register test3 (.clk(clk), .rst(rst), .d(h), .q(j));
    register test4 (.clk(clk), .rst(rst), .d(i), .q(k));
    register test5 (.clk(clk), .rst(rst), .d(m), .q(n));
    assign m = j + l;


endmodule

module register (
    input  wire clk,
    input  wire rst,
    input  wire d,
    output reg  q
);

    always @(posedge clk or negedge rst) begin
        if (!rst)
            q <= 1'b0;
        else
            q <= d;
    end

endmodule
