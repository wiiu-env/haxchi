# Haxchi

This is the continuation of Haxchi from FIX94 (initial PoC by smea).  

# Usage
Put a payload `code550.bin` in the root of this project.  
This payload should be statically linked to 0x18000000, and is called inside a thread.  
Make sure to exit this thread via `OSExitThread(0);`, afterwards the rop switches automatically to the Mii Maker.  

An example payload which perform the kernel exploit can be found [here](https://github.com/wiiu-env/haxchi_payload).

# Notes

Currently this ONLY executes a given `code550.bin`, nothing usable for the end user. Only one game, no CFW, no coldboothax, nothing.

## Dependencies

armnips and zip

## credit

smea, plutoo, yellows8, naehrwert, derrek, FIX94, dimok and orboditilt.
