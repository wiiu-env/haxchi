.PHONY := all code550.bin

ifeq ($(OS),Windows_NT)
	ZIP = cscript.exe //Nologo zip/zip.vbs
else
	ZIP = zip
endif

HAXCHI_S = haxchi.s
ROP_S = haxchi_rop.s
COREINIT_S = coreinit.s
ROP_BIN = haxchi_rop.bin

RELEASE  = ../haxchi_installer
TARGET_PATH  = $(RELEASE)/data
BUILD  = build
DEFINES  = defines

all: setup animalcrossing brainage dkjclimber guardiansigns kirby kirbymassattack mariokartds masterofdisguise newsmb_eur partnersintime \
	pokemonranger sfcommand sm64ds yoshids zeldaph zeldast \
	animalcrossing.zip brainage.zip dkjclimber.zip guardiansigns.zip kirby.zip kirbymassattack.zip mariokartds.zip masterofdisguise.zip \
	newsmb_eur.zip partnersintime.zip pokemonranger.zip sfcommand.zip sm64ds.zip yoshids.zip zeldaph.zip zeldast.zip

animalcrossing: setup_animalcrossing animalcrossing.nds

brainage: setup_brainage brainage.nds

dkjclimber: setup_dkjclimber dkjclimber.nds

guardiansigns: setup_guardiansigns guardiansigns.nds

kirby: setup_kirby kirby.nds

kirbymassattack: setup_kirbymassattack kirbymassattack.nds

mariokartds: setup_mariokartds mariokartds.nds

masterofdisguise: setup_masterofdisguise masterofdisguise.nds

newsmb_eur: setup_newsmb_eur newsmb_eur.nds

partnersintime: setup_partnersintime partnersintime.nds

pokemonranger: setup_pokemonranger pokemonranger.nds

sfcommand: setup_sfcommand sfcommand.nds

sm64ds: setup_sm64ds sm64ds.nds

yoshids: setup_yoshids yoshids.nds

zeldaph: setup_zeldaph zeldaph.nds

zeldast: setup_zeldast zeldast.nds

setup:	
	mkdir -p $(TARGET_PATH)
	mkdir -p $(BUILD)

setup_animalcrossing:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/animalcrossing_defs.s defines.s

setup_brainage:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/brainage_defs.s defines.s

setup_dkjclimber:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/dkjclimber_defs.s defines.s

setup_guardiansigns:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/guardiansigns_defs.s defines.s

setup_kirby:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/kirby_defs.s defines.s

setup_kirbymassattack:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/kirbymassattack_defs.s defines.s

setup_mariokartds:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/mariokartds_defs.s defines.s

setup_masterofdisguise:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/masterofdisguise_defs.s defines.s

setup_newsmb_eur:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/newsmb_eur_defs.s defines.s

setup_pokemonranger:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/pokemonranger_defs.s defines.s

setup_partnersintime:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/partnersintime_defs.s defines.s

setup_sfcommand:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/sfcommand_defs.s defines.s

setup_sm64ds:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/sm64ds_defs.s defines.s

setup_yoshids:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/yoshids_defs.s defines.s

setup_zeldaph:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/zeldaph_defs.s defines.s

setup_zeldast:
	@rm -f defines.s $(ROP_BIN) haxchi_rop_hook.bin
	@cp -f $(DEFINES)/zeldast_defs.s defines.s

animalcrossing.nds:	
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/animalcrossing.nds

brainage.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/brainage.nds
	@cp $(BUILD)/brainage.nds $(BUILD)/yoshitouchandgo.nds

dkjclimber.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/dkjclimber.nds

guardiansigns.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/guardiansigns.nds

kirby.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/kirby.nds

kirbymassattack.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/kirbymassattack.nds

mariokartds.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/mariokartds.nds
	@cp $(BUILD)/mariokartds.nds $(BUILD)/newsmb.nds

masterofdisguise.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/masterofdisguise.nds

newsmb_eur.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/newsmb_eur.nds

pokemonranger.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/pokemonranger.nds

partnersintime.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/partnersintime.nds

sfcommand.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/sfcommand.nds

sm64ds.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/sm64ds.nds
	@cp $(BUILD)/sm64ds.nds $(BUILD)/kirbycanvascurse.nds

yoshids.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/yoshids.nds
	@cp $(BUILD)/yoshids.nds $(BUILD)/wwtouched.nds
	@cp $(BUILD)/yoshids.nds $(BUILD)/bigbrainacademy.nds

zeldaph.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/zeldaph.nds

zeldast.nds:
	@armips $(ROP_S)
	@armips $(HAXCHI_S)
	@mv rom.nds $(BUILD)/zeldast.nds
	@cp $(BUILD)/zeldast.nds $(BUILD)/explorersofsky.nds
	@cp $(BUILD)/zeldast.nds $(BUILD)/shadowsofalmia.nds

animalcrossing.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/animalcrossing.zip $(BUILD)/animalcrossing.nds

brainage.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/brainage.zip $(BUILD)/brainage.nds
	$(ZIP) -JXjq9 $(TARGET_PATH)/yoshitouchandgo.zip $(BUILD)/yoshitouchandgo.nds

dkjclimber.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/dkjclimber.zip $(BUILD)/dkjclimber.nds

guardiansigns.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/guardiansigns.zip $(BUILD)/guardiansigns.nds

kirby.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/kirby.zip $(BUILD)/kirby.nds

kirbymassattack.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/kirbymassattack.zip $(BUILD)/kirbymassattack.nds

mariokartds.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/mariokartds.zip $(BUILD)/mariokartds.nds
	$(ZIP) -JXjq9 $(TARGET_PATH)/newsmb.zip $(BUILD)/newsmb.nds

masterofdisguise.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/masterofdisguise.zip $(BUILD)/masterofdisguise.nds

newsmb_eur.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/newsmb_eur.zip $(BUILD)/newsmb_eur.nds

pokemonranger.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/pokemonranger.zip $(BUILD)/pokemonranger.nds

partnersintime.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/partnersintime.zip $(BUILD)/partnersintime.nds

sfcommand.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/sfcommand.zip $(BUILD)/sfcommand.nds

sm64ds.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/sm64ds.zip $(BUILD)/sm64ds.nds
	$(ZIP) -JXjq9 $(TARGET_PATH)/kirbycanvascurse.zip $(BUILD)/kirbycanvascurse.nds

yoshids.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/yoshids.zip $(BUILD)/yoshids.nds
	$(ZIP) -JXjq9 $(TARGET_PATH)/wwtouched.zip $(BUILD)/wwtouched.nds
	$(ZIP) -JXjq9 $(TARGET_PATH)/bigbrainacademy.zip $(BUILD)/bigbrainacademy.nds

zeldaph.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/zeldaph.zip $(BUILD)/zeldaph.nds

zeldast.zip:
	$(ZIP) -JXjq9 $(TARGET_PATH)/zeldast.zip $(BUILD)/zeldast.nds
	$(ZIP) -JXjq9 $(TARGET_PATH)/explorersofsky.zip $(BUILD)/explorersofsky.nds
	$(ZIP) -JXjq9 $(TARGET_PATH)/shadowsofalmia.zip $(BUILD)/shadowsofalmia.nds

clean:
	@rm -fr *.nds $(TARGET_PATH)/*.zip  $(DATA) $(BUILD) defines.s
	@echo "all cleaned up !"
