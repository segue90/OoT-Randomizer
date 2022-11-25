player_save_mask:
    lbu     t0, 0x3B(s2) ;saveContext->savedMask (custom field)
    sb      t0, 0x014F(a1) ;player->currentMask
    la      t0, 0x800FE49C ;Links entry in `gKaleidoMgrOverlayTable`
    lw      t1, 0x0(t0) ;ovlTable->loadedRamAddr
    la      t2, 0x80834000 ;vram of function to jump to in Links overlay
    lw      t3, 0xC(t0) ;ovlTable->vramStart
    subu    t4, t2, t3 ; 0x80834000 - vramStart (offset into overlay)
    addu    t5, t4, t1 ; offset + loadedRamAddr (relocated address)
    jr      t5
    nop
