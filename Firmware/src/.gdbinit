file ../build/wookiectrl.elf
target remote localhost:3333
break main
#monitor program ../build/wookiectrl.elf
monitor verify
monitor reset halt
