.PHONY := all code550.bin

ifeq ($(Windows_NT), 1)
	ZIP = zip/zip.exe
else
	ZIP = zip
endif

all: setup brainage kirby mariokartds sfcommand sm64ds yoshids zeldaph brainage.zip kirby.zip mariokartds.zip \
	sfcommand.zip sm64ds.zip yoshids.zip yoshitouchandgo.zip zeldaph.zip

brainage: setup_brainage brainage.nds

kirby: setup_kirby kirby.nds

mariokartds: setup_mariokartds mariokartds.nds

sfcommand: setup_sfcommand sfcommand.nds

sm64ds: setup_sm64ds sm64ds.nds

yoshids: setup_yoshids yoshids.nds

zeldaph: setup_zeldaph zeldaph.nds

setup:
	@cd option_select && make && cd ..
	@cd hbl_loader && make && cd ..
	@cd cfw_booter && make && cd ..

setup_brainage:
	@rm -f defines.s haxchi_rop.bin haxchi_rop_hook.bin
	@cp -f brainage_defs.s defines.s

setup_kirby:
	@rm -f defines.s haxchi_rop.bin haxchi_rop_hook.bin
	@cp -f kirby_defs.s defines.s

setup_mariokartds:
	@rm -f defines.s haxchi_rop.bin haxchi_rop_hook.bin
	@cp -f mariokartds_defs.s defines.s

setup_sfcommand:
	@rm -f defines.s haxchi_rop.bin haxchi_rop_hook.bin
	@cp -f sfcommand_defs.s defines.s

setup_sm64ds:
	@rm -f defines.s haxchi_rop.bin haxchi_rop_hook.bin
	@cp -f sm64ds_defs.s defines.s

setup_yoshids:
	@rm -f defines.s haxchi_rop.bin haxchi_rop_hook.bin
	@cp -f yoshids_defs.s defines.s

setup_zeldaph:
	@rm -f defines.s haxchi_rop.bin haxchi_rop_hook.bin
	@cp -f zeldaph_defs.s defines.s

brainage.nds:
	@armips haxchi_rop.s
	@armips haxchi.s
	@mv rom.nds brainage.nds
	@cp brainage.nds yoshitouchandgo.nds

kirby.nds:
	@armips haxchi_rop.s
	@armips haxchi.s
	@mv rom.nds kirby.nds

mariokartds.nds:
	@armips haxchi_rop.s
	@armips haxchi.s
	@mv rom.nds mariokartds.nds
	@cp mariokartds.nds newsmb.nds

sfcommand.nds:
	@armips haxchi_rop.s
	@armips haxchi.s
	@mv rom.nds sfcommand.nds

sm64ds.nds:
	@armips haxchi_rop.s
	@armips haxchi.s
	@mv rom.nds sm64ds.nds

yoshids.nds:
	@armips haxchi_rop.s
	@armips haxchi.s
	@mv rom.nds yoshids.nds
	@cp yoshids.nds wwtouched.nds

zeldaph.nds:
	@armips haxchi_rop.s
	@armips haxchi.s
	@mv rom.nds zeldaph.nds

brainage.zip:
	$(ZIP) -JXjq9 brainage.zip brainage.nds
	$(ZIP) -JXjq9 yoshitouchandgo.zip yoshitouchandgo.nds

kirby.zip:
	$(ZIP) -JXjq9 kirby.zip kirby.nds

mariokartds.zip:
	$(ZIP) -JXjq9 mariokartds.zip mariokartds.nds
	$(ZIP) -JXjq9 newsmb.zip newsmb.nds

sfcommand.zip:
	$(ZIP) -JXjq9 sfcommand.zip sfcommand.nds

sm64ds.zip:
	$(ZIP) -JXjq9 sm64ds.zip sm64ds.nds

yoshids.zip:
	$(ZIP) -JXjq9 yoshids.zip yoshids.nds
	$(ZIP) -JXjq9 wwtouched.zip wwtouched.nds

zeldaph.zip:
	$(ZIP) -JXjq9 zeldaph.zip zeldaph.nds

clean:
	@rm -f *.bin defines.s brainage.nds brainage.zip kirby.nds kirby.zip mariokartds.nds mariokartds.zip newsmb.nds newsmb.zip \
		sfcommand.nds sfcommand.zip sm64ds.nds sm64ds.zip wwtouched.nds wwtouched.zip yoshids.nds yoshids.zip yoshitouchandgo.nds yoshitouchandgo.zip \
		zeldaph.nds zeldaph.zip
	@cd option_select && make clean && cd ..
	@cd hbl_loader && make clean && cd ..
	@cd cfw_booter && make clean && cd ..
	@echo "all cleaned up !"
