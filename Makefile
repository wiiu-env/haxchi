.PHONY := all code550.bin

ifeq ($(Windows_NT), 1)
	ZIP = zip/zip.exe
else
	ZIP = zip
endif

HAXCHI_S = haxchi.s
ROP_S = haxchi_rop.s
ROP_BIN = haxchi_rop.bin
all: clean brainage rom.zip 

brainage: setup_brainage brainage.nds

setup_brainage: 
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f brainage_defs.s defines.s

brainage.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds brainage.nds

rom.zip:
	$(ZIP) -JXjq9 rom.zip brainage.nds

clean:
	@rm -f *.zip *.nds defines.s
	@echo "all cleaned up !"
