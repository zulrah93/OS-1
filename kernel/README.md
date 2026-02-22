``
danny@unix-pc:~/os.1$ readelf -s kernel/bin/kernel

Symbol table '.symtab' contains 21 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS main.c
     2: ffffffff80000090    24 OBJECT  LOCAL  DEFAULT    1 limine_base_revision
     3: ffffffff80000020    48 OBJECT  LOCAL  DEFAULT    1 efi_memmap_request
     4: ffffffff80000060    48 OBJECT  LOCAL  DEFAULT    1 framebuffer_request
     5: ffffffff800000b0    16 OBJECT  LOCAL  DEFAULT    1 limine_requests_[...]
     6: ffffffff80000000    32 OBJECT  LOCAL  DEFAULT    1 limine_requests_[...]
     7: ffffffff80001420   193 FUNC    GLOBAL DEFAULT    2 get_cpu_information
     8: ffffffff80001260   264 FUNC    GLOBAL DEFAULT    2 put_glyph
     9: ffffffff80001170   224 FUNC    GLOBAL DEFAULT    2 fill_glyph
    10: ffffffff80004245     0 NOTYPE  GLOBAL DEFAULT    4 _binary_src_boot[...]
    11: ffffffff80001370   166 FUNC    GLOBAL DEFAULT    2 printk
    12: 0000000000040046     0 NOTYPE  GLOBAL DEFAULT  ABS _binary_src_boot[...]
    13: ffffffff8004428b     0 NOTYPE  GLOBAL DEFAULT    4 _binary_src_boot[...]
    14: ffffffff80003000     0 NOTYPE  GLOBAL DEFAULT    4 _binary_src_font[...]
    15: ffffffff80004245     0 NOTYPE  GLOBAL DEFAULT    4 _binary_src_font[...]
    16: 0000000000001245     0 NOTYPE  GLOBAL DEFAULT  ABS _binary_src_font[...]
    17: ffffffff80001100   108 FUNC    GLOBAL DEFAULT    2 draw_bitmap
    18: ffffffff80001500   690 FUNC    GLOBAL DEFAULT    2 kmain
    19: ffffffff80001000   164 FUNC    GLOBAL DEFAULT    2 integer_to_string
    20: ffffffff800010b0    80 FUNC    GLOBAL DEFAULT    2 clear_screen
``
