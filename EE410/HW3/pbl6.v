module top_module ();
  initial `probe_start; // Start the timing diagram / waveform probe

  // Inputs to DUT (registers)
  reg  [3:0] a;
  reg  [3:0] b;

  // Outputs from DUT (wires)
  wire [4:0] c;  // 5 bits to capture carry-out: {cout, s}

  // Verification variables
  integer i, j;
  integer error_count = 0;

  // Instantiate the adder module
  pbl6 test (
    .a(a),
    .b(b),
    .c(c)
  );

  initial begin
    // Loop over all 16 values of 'a' (0 to 15)
    for (i = 0; i < 16; i = i + 1) begin
      // Loop over all 16 values of 'b' (0 to 15)
      for (j = 0; j < 16; j = j + 1) begin
        a = i;
        b = j;
        #10; // Wait for combinational logic to settle

        // Validate DUT output (c) against expected math (i + j)
        if (c !== (i + j)) begin
          $display("ERROR at time %0t: a = %0d, b = %0d | Expected: %0d, Got: %0d", 
                    $time, a, b, (i + j), c);
          error_count = error_count + 1;
        end
      end
    end

    if (error_count == 0) begin
      $display("SUCCESS: All 256 test cases passed!");
    end else begin
      $display("FAILURE: %0d errors detected out of 256 cases.", error_count);
    end

    $finish;
  end
    
    
  `probe(a);
  `probe(b);
  `probe(c);

endmodule


module pbl6 (
    input  [3:0] a,
    input  [3:0] b,
    output [4:0] c
);

    assign c = a + b;

endmodule
