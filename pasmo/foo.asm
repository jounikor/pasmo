

foof    EQU 	10
koe	EQU	-10-30
koe1	EQU	0-10-30
koe2	EQU	10-30



testt    MACRO sBANK,src
        DW  sBANK,src
        ENDM


        STRUCT foo
buz     db  3
xxx     db  "nnnnn"
yyyy    defb 1,2,3,4
bar     dw  1
zzz     ds foof
        ENDS




    ORG $8000       ; -> fixed at BANK2


    STACK $6000

main:
    ld  ix,main
    ld  a,(ix+0-2)
    ld  a,(ix+2)
    ld  a,(iy-2)
    ld  a,(ix+2-1-3-2-4-3)
    ld  a,(ix-10)
    ld  a,(ix+10-5-10+foo.bar)
    ld  a,(iy-10-5-10)
    ld  a,(iy-10+5-10)

    ld  hl,main
    ld  a,(test.foo)
nn  jr  nn
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
   
    if 1 
    BANK    6
    ORG     $c028
bank5:
    ld  bc,bank5
    ld  ix,dataa
    ld  a,(ix+foo.zzz)
    
    
    ret
    endif


dataa:
    ds  foo
    ds  foo
    ds  foo
    ds  foo

    dg  x x x x x x x x - G - G - g - g
    dg  "x x x x x x x x - a - b - c - d"
    dg  --xx..xx__XX--@@....xxxx
    dg  xxxx....x.x.x.x.x.x.x.x.
    dg  xxxx.... x.x.x.x. x.x.x.x.
    dg  "--xx--xx"
    dg  .x.x.x.x
    DG  X-X-X-X-
    dg  "--xx--xx"

    ds  foo

    END main

