    .ORIG x3000
    ADD R1, R2, R7;
 A  ADD R4, R5, #8;
    AND R6, R7, R0;
    AND R1, R2, #8;
    BR A;
    BRn B;
    BRnz A;
    BRnzp B;
    BRz A;
    BRzp B;
    BRp A;
    BRnp B;
    JMP R2;
    RET;
    JSR B
B   JSRR R3;
    LDB R2, R3, #16;
    LDW R5, R2, #16;
    LEA R0, B;
    NOT R1, R2;
    LSHF R2, R3, #15
    RSHFL R0, R1, #15
    RSHFA R4, R5, #15
    STB R6, R7, #15
    STW R4, R5, #30;
    TRAP x25
    XOR R3, R4, R7;
    XOR R6, R1, #11;
    .FILL x4000;
    .FILL x2101;
    .END