; King Zora Hacks

.headersize(0x80AD5D60 - 0x00E55BA0)

;===================================================================================================
; Prevent the trade quest timer to start if you get the Zora Tunic item from King Zora with
; Eyeball Frog in inventory.
;===================================================================================================

; First, keep track that we're trading the eyeball frog
.org 0x80ad63dc
; Replaces:
;   sh      t7, 0x10e(s0)
;   sb      r0, 0x1f8(s0)
    jal     kz_store_is_trading
    sh      t7, 0x10e(s0) ; Replaced code

; Check the flag when starting the timer

; Replaces     lh      t6, 0x01D0(a1)
;              addiu   $at, $zero, 0x0003
.org 0x80ad6cf8
    jal     kz_no_timer
    addiu   $at, $zero, 0x0003
