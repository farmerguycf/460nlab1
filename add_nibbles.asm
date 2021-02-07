        .orig x3000 ; x3000
        lea r0, mem
        ldw r0, r0, #0 ;x3050
        ldb r1, r0, #0
        and r2, r1, x000f
        and r3, r1, x00f0
        add r2, r2, r3
        stb r2, r0, #0
        ; some comment
        halt
        ; another comment
mem     .fill x3050
        .end
