        .orig x3000 ; x3000
        lea r0, mem
        ldw r0, r0, #0 ;x3050
        ldb r1, r0, #0
        and r2, r1, x000f
        and r6, r6, #0
        add r6, r6, xf
        lshf r6, r6, #4
        and r3, r1, r6
        add r2, r2, r3
        stb r2, r0, #0
        nop ; comment
        rti ; comment
        ret ;comment
        ; some comment
        halt
        ; another comment
mem     .fill x3050
        .end
