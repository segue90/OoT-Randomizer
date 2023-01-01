; Manipulates the save context language bit based
; on the segment value for the requested text ID.
; Message lookup is moved here from the vanilla
; branches.
; t6 used for branching to JP/EN file loading
; in parent function func_800DC838 at VRAM 0x800DCB68
; 0 == JP
; 1 == EN
set_message_file_to_search:
    ; displaced code
    lhu     a1, 0x0046($sp)
    lw      a0, 0x0040($sp)

    ; Saved by vanilla code prior to jump to
    ; message lookup function. Called here so
    ; we can safely store $ra.
    sw      t0, 0x002C($sp)

    ; Message lookup saves to the stack without
    ; changing the stack pointer. Save to an
    ; unused variable to avoid changing the stack.
    or      t4, $zero, $ra

    ; call JP message lookup function exclusively
    ; since the JP/EN tables are merged.
    jal     0x800D69EC
    nop

    ; a1 contains the segment/offset word, formatted as
    ; ssoooooo
    ; where "s" is the segment and "o" is the offset.
    ; Vanilla crashes on a failed text lookup already,
    ; so we don't need to worry about bad values.
    srl     t0, a1, 0x18
    andi    t0, t0, 0x08
    sltiu   t6, t0, 0x0001

    or      $ra, $zero, t4
    jr      $ra
    nop
