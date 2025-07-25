// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Version: 2022.2
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
`timescale 1 ns / 1 ps

module sobel_filter_sdiv_18ns_17ns_10_22_1_divider
#(parameter
    in0_WIDTH = 32,
    in1_WIDTH = 32,
    out_WIDTH = 32
)
(
    input                       clk,
    input                       reset,
    input                       ce,
    input       [in0_WIDTH-1:0] dividend,
    input       [in1_WIDTH-1:0] divisor,
    input       [1:0]           sign_i,
    output wire [1:0]           sign_o,
    output wire [out_WIDTH-1:0] quot,
    output wire [out_WIDTH-1:0] remd
);

localparam cal_WIDTH = (in0_WIDTH > in1_WIDTH)? in0_WIDTH : in1_WIDTH;

//------------------------Local signal-------------------
reg  [in0_WIDTH-1:0] dividend_tmp[0:in0_WIDTH];
reg  [in1_WIDTH-1:0] divisor_tmp[0:in0_WIDTH];
reg  [in0_WIDTH-1:0] remd_tmp[0:in0_WIDTH];
wire [in0_WIDTH-1:0] comb_tmp[0:in0_WIDTH-1];
wire [cal_WIDTH:0]   cal_tmp[0:in0_WIDTH-1];
reg  [1:0]           sign_tmp[0:in0_WIDTH];
//------------------------Body---------------------------
assign  quot    = dividend_tmp[in0_WIDTH];
assign  remd    = remd_tmp[in0_WIDTH];
assign  sign_o  = sign_tmp[in0_WIDTH];

// dividend_tmp[0], divisor_tmp[0], remd_tmp[0]
always @(posedge clk)
begin
    if (ce) begin
        dividend_tmp[0] <= dividend;
        divisor_tmp[0]  <= divisor;
        sign_tmp[0]     <= sign_i;
        remd_tmp[0]     <= 1'b0;
    end
end

genvar i;
generate 
    for (i = 0; i < in0_WIDTH; i = i + 1)
    begin : loop
        if (in0_WIDTH == 1) assign  comb_tmp[i]     = dividend_tmp[i][0];
        else                assign  comb_tmp[i]     = {remd_tmp[i][in0_WIDTH-2:0], dividend_tmp[i][in0_WIDTH-1]};
        assign  cal_tmp[i]      = {1'b0, comb_tmp[i]} - {1'b0, divisor_tmp[i]};

        always @(posedge clk)
        begin
            if (ce) begin
                if (in0_WIDTH == 1) dividend_tmp[i+1] <= ~cal_tmp[i][cal_WIDTH];
                else                dividend_tmp[i+1] <= {dividend_tmp[i][in0_WIDTH-2:0], ~cal_tmp[i][cal_WIDTH]};
                divisor_tmp[i+1]  <= divisor_tmp[i];
                remd_tmp[i+1]     <= cal_tmp[i][cal_WIDTH]? comb_tmp[i] : cal_tmp[i][in0_WIDTH-1:0];
                sign_tmp[i+1]     <= sign_tmp[i];
            end
        end
    end
endgenerate

endmodule

module sobel_filter_sdiv_18ns_17ns_10_22_1 
#(parameter
        ID   = 1,
        NUM_STAGE   = 2,
        din0_WIDTH   = 32,
        din1_WIDTH   = 32,
        dout_WIDTH   = 32
)
(
        input                           clk,
        input                           reset,
        input                           ce,
        input           [din0_WIDTH-1:0] din0,
        input           [din1_WIDTH-1:0] din1,
        output          [dout_WIDTH-1:0] dout
);
//------------------------Local signal-------------------
reg     [din0_WIDTH-1:0] dividend0;
reg     [din1_WIDTH-1:0] divisor0;
wire    [din0_WIDTH-1:0] dividend_u;
wire    [din1_WIDTH-1:0] divisor_u;
wire    [dout_WIDTH-1:0] quot_u;
wire    [dout_WIDTH-1:0] remd_u;
reg     [dout_WIDTH-1:0] quot;
reg     [dout_WIDTH-1:0] remd;
wire    [1:0]   sign_i;
wire    [1:0]   sign_o;
//------------------------Instantiation------------------
sobel_filter_sdiv_18ns_17ns_10_22_1_divider #(
    .in0_WIDTH      ( din0_WIDTH ),
    .in1_WIDTH      ( din1_WIDTH ),
    .out_WIDTH      ( dout_WIDTH )
) sobel_filter_sdiv_18ns_17ns_10_22_1_divider_u (
    .clk      ( clk ),
    .reset    ( reset ),
    .ce       ( ce ),
    .dividend ( dividend_u ),
    .divisor  ( divisor_u ),
    .sign_i   ( sign_i ),
    .sign_o   ( sign_o ),
    .quot     ( quot_u ),
    .remd     ( remd_u )
);
//------------------------Body---------------------------
assign sign_i     = {dividend0[din0_WIDTH-1] ^ divisor0[din1_WIDTH-1], dividend0[din0_WIDTH-1]};
assign dividend_u = dividend0[din0_WIDTH-1]? ~dividend0[din0_WIDTH-1:0] + 1'b1 :
                                              dividend0[din0_WIDTH-1:0];
assign divisor_u  = divisor0[din1_WIDTH-1]?  ~divisor0[din1_WIDTH-1:0] + 1'b1 :
                                              divisor0[din1_WIDTH-1:0];

always @(posedge clk)
begin
    if (ce) begin
        dividend0 <= din0;
        divisor0  <= din1;
    end
end

always @(posedge clk)
begin
    if (ce) begin
        if (sign_o[1])
            quot <= ~quot_u + 1'b1;
        else
            quot <= quot_u;
    end
end

always @(posedge clk)
begin
    if (ce) begin
        if (sign_o[0])
            remd <= ~remd_u + 1'b1;
        else
            remd <= remd_u;
    end
end

assign dout = quot;

endmodule


