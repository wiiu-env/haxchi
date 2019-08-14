# Haxchi-Exploit
This is the continuation of Haxchi from FIX94 (initial PoC by smea).  

```
haxchi is an exploit for the Nintendo DS virtual console emulator on Wii U (hachihachi). 
it is possible due to "contenthax", a vulnerability in the wii u's title integrity design: 
only code and critical descriptors are signed, with all other contents left at the mercy of attackers.
```

# Usage
Put a payload `code550.bin` in the root of this project.  
This payload should be statically linked to 0x18000000, and is called inside a thread.  
Make sure to exit this thread via `OSExitThread(0);`, afterwards the rop switches automatically to the Mii Maker.  

An example payload which perform the kernel exploit can be found [here](https://github.com/wiiu-env/haxchi_payload).

# Notes

Currently this ONLY executes a given `code550.bin`, nothing usable for the end user. Only one game, no CFW, no coldboothax, nothing.

## Dependencies

[armips](https://github.com/Kingcom/armips/releases) and zip in your PATH variable.

## credit

smea, plutoo, yellows8, naehrwert, derrek, FIX94, dimok and orboditilt.
