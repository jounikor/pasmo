

foof    EQU 10


testt    MACRO sBANK,src
        DW  sBANK,src
        ENDM

    ENDS

        STRUCT foo
buz     db  3
xxx     db  "nnnnn"
yyyy    defb 1,2,3,4
bar     dw  1
zzz     ds foof
        ENDS




    ORG $8000





main:
    ld  hl,main
    ld  a,(test.foo)
    ret
    ret
    ret
 
test.foo:


    BANK    3
    ORG     $c000
bank3:
    ld  bc,bank3
    ret
    
    BANK    4
bank4:
    ld  bc,bank4
    ret
    
    BANK    2
    ;ORG     $c000
bank5:
    ld  bc,bank5
    ld  ix,dataa
    ld  a,(ix+foo.zzz)
    
    
    ret



dataa:
    ds  foo



    END main

