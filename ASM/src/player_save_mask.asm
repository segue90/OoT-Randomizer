player_save_mask:
    lbu     t0, 0x3B(s2) ;saveContext->savedMask (custom field)
    sb      t0, 0x014F(a1) ;player->currentMask

    j       0x8038C9A0 ; Pre relocated jump. Player is always in the same place so it *should* be fine.
    nop