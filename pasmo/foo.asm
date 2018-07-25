

    ORG $8000

main:
    ld  hl,main
    ld  a,(main)
    ret
    ret
    ret
    
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
    ret

    END main

