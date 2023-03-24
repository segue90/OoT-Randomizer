SHUFFLE_CHEST_GAME:
.byte 0x00
.align 4

chestgame_buy_item_hook:
    lb      t0, SHUFFLE_CHEST_GAME
    beqz    t0, @@return        ; skip if the chest game isn't randomized

    la      t1, GLOBAL_CONTEXT
    lw      t0, 0x1D44(t1)      ; load scene collectible flags (Treasure Box House)
    ori     t0, t0, 0x2         ; set flag 1 (custom TCG salesman flag)
    sw      t0, 0x1D44(t1)

@@return:
jr      ra
sw      $zero, 0x0118(s0)

