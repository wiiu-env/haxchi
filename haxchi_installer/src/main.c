/*
 * Copyright (C) 2016-2017 FIX94
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iosuhax.h>
#include <coreinit/mcp.h>
#include <vpad/input.h>

#include <whb/proc.h>
#include <whb/log.h>
#include <whb/log_console.h>


#include <coreinit/foreground.h>
#include <coreinit/systeminfo.h>
#include <proc_ui/procui.h>
#include <sysapp/launch.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include "main.h"
#include "gameList.h"
#include "memory.h"


static const char *sdCardVolPath = "/vol/storage_sdcard_new";
#ifdef CB
static const char *systemXmlPath = "/vol/system/config/system.xml";
static const char *systemXmlPath2 = "/vol/system/config/system_new.xml";
static const char *syshaxXmlPath = "/vol/system/config/syshax.xml";
#endif
//just to be able to call async
void someFunc(void *arg)
{
	(void)arg;
}

extern void OSForceFullRelaunch(void);
extern 	unsigned long long(*_SYSGetSystemApplicationTitleId)(int sysApp);


void println_noflip(int line, const char *msg)
{
	WHBLogPrintf(msg);
	WHBLogConsoleDraw();
}

void println(int line, const char *msg)
{
	WHBLogPrintf(msg);
	WHBLogConsoleDraw();
}

typedef struct _parsedList_t {
	uint32_t tid;
	char name[64];
	char path[64];
	uint8_t *romPtr;
	uint32_t romSize;
} parsedList_t;

int fsa_read(int fsa_fd, int fd, void *buf, int len)
{
	int done = 0;
	uint8_t *buf_uint8_t = (uint8_t*)buf;
	while(done < len)
	{
		size_t read_size = len - done;
		int result = IOSUHAX_FSA_ReadFile(fsa_fd, buf_uint8_t + done, 0x01, read_size, fd, 0);
		if(result < 0)
			return result;
		else
			done += result;
	}
	return done;
}

int fsa_write(int fsa_fd, int fd, void *buf, int len)
{
	int done = 0;
	uint8_t *buf_uint8_t = (uint8_t*)buf;
	while(done < len)
	{
		size_t write_size = len - done;
		int result = IOSUHAX_FSA_WriteFile(fsa_fd, buf_uint8_t + done, 0x01, write_size, fd, 0);
		if(result < 0)
			return result;
		else
			done += result;
	}
	return done;
}

int availSort(const void *c1, const void *c2)
{
	return strcmp(((parsedList_t*)c1)->name,((parsedList_t*)c2)->name);
}

void printhdr_noflip()
{
#ifdef CB
	println_noflip(0,"CBHC v1.6 by FIX94");
#else
	println_noflip(0,"Haxchi v2.5u2 by FIX94");
#endif
	println_noflip(1,"Credits to smea, plutoo, yellows8, naehrwert, derrek and dimok");
}

static uint32_t
procSaveCallback(void *context) {
    OSSavesDone_ReadyToRelease();
    return 0;
}

int cleanupAndExitToHBL(){
	memoryRelease();
	WHBLogConsoleFree();
	WHBProcShutdown();
	return EXIT_SUCCESS;
}

int main(void)
{	

	OSEnableHomeButtonMenu(FALSE);
	 WHBProcInit();
	WHBLogConsoleInit();

	VPADInit();
	memoryInitialize();

	int mcp_handle = MCP_Open();
	int count = MCP_TitleCount(mcp_handle);
	int listSize = count*sizeof(MCPTitleListType);
	MCPTitleListType *tList = (MCPTitleListType*)memalign(32, listSize); //cant be in MEMBucket
	memset(tList, 0, listSize);
	uint32_t recievedCount = count;
	MCP_TitleList(mcp_handle, &recievedCount, tList, listSize);
	MCP_Close(mcp_handle);

	int gAvailCnt = 0;
	parsedList_t *gAvail = (parsedList_t*)MEMBucket_alloc(recievedCount*sizeof(parsedList_t), 4);
	memset(gAvail, 0, recievedCount*sizeof(parsedList_t));

	int i, j;
	for(i = 0; i < recievedCount; i++)
	{
		MCPTitleListType cListElm = tList[i];
		if(memcmp(cListElm.indexedDevice,"mlc",4) != 0 || ((cListElm.titleId & 0xFFFFFFFF00000000L) != 0x0005000000000000L))
			continue;
		for(j = 0; j < GameListSize; j++)
		{
			if((cListElm.titleId & 0x00000000FFFFFFFFL) == GameList[j].tid)
			{
				gAvail[gAvailCnt].tid = GameList[j].tid;
				memcpy(gAvail[gAvailCnt].name, GameList[j].name, 64);
				memcpy(gAvail[gAvailCnt].path, cListElm.path, 64);
				gAvail[gAvailCnt].romPtr = GameList[j].romPtr;
				gAvail[gAvailCnt].romSize = *(GameList[j].romSizePtr);
				gAvailCnt++;
				break;
			}
		}
	}

	int vpadError = -1;
	VPADStatus vpad;

	if(gAvailCnt == 0)
	{
		printhdr_noflip();
		println_noflip(2,"No games found on NAND! Make sure that you have your DS VC");
		println_noflip(3,"game installed on NAND and have all of your USB Devices");
		println_noflip(4,"disconnected while installing Haxchi as it can lead to issues.");
		println_noflip(5,"Press any button to return to Homebrew Launcher.");

		while(1)
		{
			usleep(25000);
			VPADRead(0, &vpad, 1, &vpadError);
			if(vpadError != 0)
				continue;
			if(vpad.trigger | vpad.hold)
				break;
		}
		return cleanupAndExitToHBL();
	}

	qsort(gAvail,gAvailCnt,sizeof(parsedList_t),availSort);

	uint32_t redraw = 1;
	int32_t PosX = 0;
	int32_t ScrollX = 0;

	int32_t ListMax = gAvailCnt;
	if( ListMax > 13 )
		ListMax = 13;

	uint32_t UpHeld = 0, triggerHeld = 0;
	while(1)
	{
		usleep(25000);
		VPADRead(0, &vpad, 1, &vpadError);
		if(vpadError != 0)
			continue;

		if((vpad.trigger | vpad.hold) & VPAD_BUTTON_HOME)
		{
			
			return cleanupAndExitToHBL();
		}
		if( vpad.hold & VPAD_BUTTON_DOWN)
		{
			if(triggerHeld == 0 || triggerHeld > 10)
			{
				if( PosX + 1 >= ListMax )
				{
					if( PosX + 1 + ScrollX < gAvailCnt)
						ScrollX++;
					else {
						PosX	= 0;
						ScrollX = 0;
					}
				} else {
					PosX++;
				}
				redraw = 1;
			}
			triggerHeld++;
		}
		else
			triggerHeld = 0;

		if( vpad.hold & VPAD_BUTTON_UP )
		{
			if(UpHeld == 0 || UpHeld > 10)
			{
				if( PosX <= 0 )
				{
					if( ScrollX > 0 )
						ScrollX--;
					else {
						PosX	= ListMax - 1;
						ScrollX = gAvailCnt - ListMax;
					}
				} else {
					PosX--;
				}
				redraw = 1;
			}
			UpHeld++;
		}
		else
			UpHeld = 0;

		if( vpad.trigger & VPAD_BUTTON_A )
			break;

		if( redraw )
		{
			
			printhdr_noflip();
			println_noflip(2,"Please select the game for the Installation from the list below.");
			// Starting position.
			int gamelist_y = 4;
			for (i = 0; i < ListMax; ++i, gamelist_y++)
			{
				const parsedList_t *cur_gi = &gAvail[i+ScrollX];
				char printStr[64];
				sprintf(printStr,"%c %s", i == PosX ? '>' : ' ', cur_gi->name);
				println_noflip(gamelist_y, printStr);
			}			
			redraw = 0;
		}
	}
#ifdef CB
	int action = 0;
#endif
	const parsedList_t *SelectedGame = &gAvail[PosX + ScrollX];
		println_noflip(2,"You have selected the following game:");
		println_noflip(3,SelectedGame->name);
#ifdef CB
		println_noflip(4,"Press A to install CBHC, B to remove coldboothax, HOME to Exit.");
		println_noflip(5,"WARNING, INSTALLING CBHC CAN POTENTIALLY BRICK YOUR SYSTEM!");
		println_noflip(6,"NEVER UNINSTALL OR MOVE THE SELECTED GAME OR YOUR WIIU IS DEAD!");
#else
		println_noflip(4,"This will install Haxchi. To remove it you have to delete and");
		println_noflip(5,"re-install the game. If you are sure press A, else press HOME.");
#endif
		
	while(1)
	{
		usleep(25000);
		VPADRead(0, &vpad, 1, &vpadError);
		if(vpadError != 0)
			continue;
		//user aborted
		if((vpad.trigger | vpad.hold) & VPAD_BUTTON_HOME)
		{			
			return cleanupAndExitToHBL();
		}
		//lets go!
		if(vpad.trigger & VPAD_BUTTON_A)
			break;
#ifdef CB
		if(vpad.trigger & VPAD_BUTTON_B)
		{
			action = 1;
			break;
		}
#endif
	}

#ifdef CB	
	unsigned long long sysmenuIdUll = _SYSGetSystemApplicationTitleId(0);
	char sysmenuId[20];
	memset(sysmenuId, 0, 20);
	sprintf(sysmenuId, "%08x%08x", (uint32_t)((sysmenuIdUll>>32)&0xFFFFFFFF),(uint32_t)(sysmenuIdUll&0xFFFFFFFF));
	char new_title_id[20];
	memset(new_title_id, 0, 20);
	sprintf(new_title_id, "00050000%08x", SelectedGame->tid);
	int line = 7;
#else
	int line = 6;
#endif

	int fsaFd = -1;
	int sdMounted = 0;
	int sdFd = -1, mlcFd = -1, slcFd = -1;

	//open up iosuhax
	int res = IOSUHAX_Open(NULL);	
	if(res < 0)
	{
		println(line++,"IOSUHAX_Open failed");
		goto prgEnd;		
	}

	//mount with full permissions
	fsaFd = IOSUHAX_FSA_Open();
	if(fsaFd < 0)
	{
		println(line++,"FSA could not be opened!");
		goto prgEnd;
	}
#ifdef CB
	if(action == 1)
	{
		if(IOSUHAX_FSA_OpenFile(fsaFd, systemXmlPath, "rb", &slcFd) >= 0)
		{
			//read in system xml file
			fileStat_s stats;
			IOSUHAX_FSA_StatFile(fsaFd, slcFd, &stats);
			size_t sysXmlSize = stats.size;
			char *sysXmlBuf = MEMBucket_alloc(sysXmlSize+1,4);
			memset(sysXmlBuf, 0, sysXmlSize+1);
			fsa_read(fsaFd, slcFd, sysXmlBuf, sysXmlSize);
			IOSUHAX_FSA_CloseFile(fsaFd, slcFd);
			slcFd = -1;
			xmlDocPtr doc = xmlReadMemory(sysXmlBuf, sysXmlSize, "system.xml", "utf-8", 0);
			//verify title id
			int idFound = 0, idCorrect = 0;
			xmlNode *root_element = xmlDocGetRootElement(doc);
			xmlNode *cur_node = NULL;
			for (cur_node = root_element->children; cur_node; cur_node = cur_node->next) {
				if (cur_node->type == XML_ELEMENT_NODE) {
					if(memcmp(cur_node->name, "default_title_id", 17) == 0)
					{
						if(xmlNodeGetContent(cur_node) == NULL || !strlen((char*)xmlNodeGetContent(cur_node))) continue;
						if(memcmp(new_title_id, (char*)xmlNodeGetContent(cur_node), 17) == 0) idCorrect++;
						idFound++;
					}
				}
			}
			xmlFreeDoc(doc);
			MEMBucket_free(sysXmlBuf);
			if(idFound != 1)
				println(line++,"default_title_id missing!");
			else if(idCorrect != 1)
				println(line++,"default_title_id not set to selected DS VC!");
			else
			{
				if(IOSUHAX_FSA_OpenFile(fsaFd, syshaxXmlPath, "rb", &slcFd) >= 0)
				{
					//read in system xml file
					fileStat_s stats;
					IOSUHAX_FSA_StatFile(fsaFd, slcFd, &stats);
					size_t sysXmlSize = stats.size;
					sysXmlBuf = MEMBucket_alloc(sysXmlSize+1,4);
					memset(sysXmlBuf, 0, sysXmlSize+1);
					fsa_read(fsaFd, slcFd, sysXmlBuf, sysXmlSize);
					IOSUHAX_FSA_CloseFile(fsaFd, slcFd);
					slcFd = -1;
					xmlDocPtr doc = xmlReadMemory(sysXmlBuf, sysXmlSize, "syshax.xml", "utf-8", 0);
					//verify title id
					int idFound = 0, idCorrect = 0;
					xmlNode *root_element = xmlDocGetRootElement(doc);
					xmlNode *cur_node = NULL;
					for (cur_node = root_element->children; cur_node; cur_node = cur_node->next) {
						if (cur_node->type == XML_ELEMENT_NODE) {
							if(memcmp(cur_node->name, "default_title_id", 17) == 0)
							{
								if(xmlNodeGetContent(cur_node) == NULL || !strlen((char*)xmlNodeGetContent(cur_node))) continue;
								if(memcmp(sysmenuId, (char*)xmlNodeGetContent(cur_node), 17) == 0) idCorrect++;
								idFound++;
							}
						}
					}
					xmlFreeDoc(doc);
					if(idFound != 1)
						println(line++,"default_title_id missing!");
					else if(idCorrect != 1)
						println(line++,"default_title_id not set to System Menu!");
					else
					{
						if(IOSUHAX_FSA_OpenFile(fsaFd, systemXmlPath, "wb", &slcFd) >= 0)
						{
							println(line++,"Restoring system.xml...");
							fsa_write(fsaFd, slcFd, sysXmlBuf, sysXmlSize);
							IOSUHAX_FSA_CloseFile(fsaFd, slcFd);
							slcFd = -1;
							println(line++,"Removed coldboothax!");
						}
					}
					MEMBucket_free(sysXmlBuf);
				}
				else
					println(line++,"syshax.xml backup not found, aborting!");
			}
		}
		goto prgEnd;
	}
#endif
	int ret = IOSUHAX_FSA_Mount(fsaFd, "/dev/sdcard01", sdCardVolPath, 2, (void*)0, 0);
	if(ret < 0)
	{
		println(line++,"Failed to mount SD!");
		goto prgEnd;
	}
	else
		sdMounted = 1;
	char path[256];
	sprintf(path,"%s/content/0010/rom.zip",SelectedGame->path);
	if(IOSUHAX_FSA_OpenFile(fsaFd, path, "rb", &mlcFd) < 0)
	{
		println(line++,"No already existing rom.zip found in the game!");
		println(line++,"Make sure to re-install your DS title and try again.");
		goto prgEnd;
	}
	else
		IOSUHAX_FSA_CloseFile(fsaFd, mlcFd);
	if(IOSUHAX_FSA_OpenFile(fsaFd, path, "wb", &mlcFd) >= 0)
	{
		println(line++,"Writing rom.zip...");
		fsa_write(fsaFd, mlcFd, SelectedGame->romPtr, SelectedGame->romSize);
		IOSUHAX_FSA_CloseFile(fsaFd, mlcFd);
		mlcFd = -1;
	}

	char sdHaxchiPath[256];
#ifdef CB
	sprintf(sdHaxchiPath,"%s/cbhc",sdCardVolPath);
#else
	sprintf(sdHaxchiPath,"%s/haxchi",sdCardVolPath);
#endif
	char sdPath[256];

#ifndef CB
	sprintf(sdPath,"%s/config.txt",sdHaxchiPath);
	if(IOSUHAX_FSA_OpenFile(fsaFd, sdPath, "rb", &sdFd) >= 0)
	{
		//read in sd file
		fileStat_s stats;
		IOSUHAX_FSA_StatFile(fsaFd, sdFd, &stats);
		size_t cfgSize = stats.size;
		uint8_t *cfgBuf = MEMBucket_alloc(cfgSize,4);
		fsa_read(fsaFd, sdFd, cfgBuf, cfgSize);
		IOSUHAX_FSA_CloseFile(fsaFd, sdFd);
		sdFd = -1;
		//write to nand
		sprintf(path,"%s/content/config.txt",SelectedGame->path);
		if(IOSUHAX_FSA_OpenFile(fsaFd, path, "wb", &mlcFd) >= 0)
		{
			println(line++,"Writing config.txt...");
			fsa_write(fsaFd, mlcFd, cfgBuf, cfgSize);
			IOSUHAX_FSA_CloseFile(fsaFd, mlcFd);
			mlcFd = -1;
			//make it readable by game
			IOSUHAX_FSA_ChangeMode(fsaFd, path, 0x644);
		}
		MEMBucket_free(cfgBuf);
	}
#endif

	sprintf(sdPath,"%s/title.txt",sdHaxchiPath);
	if(IOSUHAX_FSA_OpenFile(fsaFd, sdPath, "rb", &sdFd) >= 0)
	{
		//read in sd file
		fileStat_s stats;
		IOSUHAX_FSA_StatFile(fsaFd, sdFd, &stats);
		size_t titleSize = stats.size;
		xmlChar *titleBuf = MEMBucket_alloc(titleSize+1,4);
		memset(titleBuf, 0, titleSize+1);
		fsa_read(fsaFd, sdFd, titleBuf, titleSize);
		IOSUHAX_FSA_CloseFile(fsaFd, sdFd);
		sdFd = -1;
		sprintf(path,"%s/meta/meta.xml",SelectedGame->path);
		if(IOSUHAX_FSA_OpenFile(fsaFd, path, "rb", &mlcFd) >= 0)
		{
			IOSUHAX_FSA_StatFile(fsaFd, mlcFd, &stats);
			size_t metaSize = stats.size;
			char *metaBuf = MEMBucket_alloc(metaSize,4);
			fsa_read(fsaFd, mlcFd, metaBuf, metaSize);
			IOSUHAX_FSA_CloseFile(fsaFd, mlcFd);
			mlcFd = -1;
			//parse doc
			xmlDocPtr doc = xmlReadMemory(metaBuf, metaSize, "meta.xml", "utf-8", 0);
			//change title
			xmlNode *root_element = xmlDocGetRootElement(doc);
			xmlNode *cur_node = NULL;
			for (cur_node = root_element->children; cur_node; cur_node = cur_node->next) {
				if (cur_node->type == XML_ELEMENT_NODE) {
					if(memcmp(cur_node->name, "longname_", 9) == 0 || memcmp(cur_node->name, "shortname_", 10) == 0)
					{
						if(xmlNodeGetContent(cur_node) == NULL || !strlen((char*)xmlNodeGetContent(cur_node))) continue;
						xmlNodeSetContent(cur_node, titleBuf);
					}
				}
			}
			//back to xml
			xmlChar *newXml = NULL;
			int newSize = 0;
			xmlSaveNoEmptyTags = 1; //keeps original style
			xmlDocDumpFormatMemoryEnc(doc, &newXml, &newSize, "utf-8", 0);
			xmlFreeDoc(doc);
			if(newXml != NULL && newSize > 0)
			{
				//libxml2 adds in extra \n at the end
				if(newXml[newSize-1] == '\n' && metaBuf[metaSize-1] != '\n')
				{
					newXml[newSize-1] = '\0';
					newSize--;
				}
				//write back to nand
				if(IOSUHAX_FSA_OpenFile(fsaFd, path, "wb", &mlcFd) >= 0)
				{
					println(line++,"Changing game title...");
					//UTF-8 BOM
					char bom[3] = { 0xEF, 0xBB, 0xBF };
					if(memcmp(newXml, bom, 3) != 0 && memcmp(metaBuf, bom, 3) == 0)
						fsa_write(fsaFd, mlcFd, bom, 0x03);
					fsa_write(fsaFd, mlcFd, newXml, newSize);
					IOSUHAX_FSA_CloseFile(fsaFd, mlcFd);
					mlcFd = -1;
				}
				free(newXml);
			}
			MEMBucket_free(metaBuf);
		}
		MEMBucket_free(titleBuf);
	}

	sprintf(sdPath,"%s/bootDrcTex.tga",sdHaxchiPath);
	if(IOSUHAX_FSA_OpenFile(fsaFd, sdPath, "rb", &sdFd) >= 0)
	{
		//read in sd file
		fileStat_s stats;
		IOSUHAX_FSA_StatFile(fsaFd, sdFd, &stats);
		size_t bootDrcTexSize = stats.size;
		uint8_t *bootDrcTex = MEMBucket_alloc(bootDrcTexSize,4);
		fsa_read(fsaFd, sdFd, bootDrcTex, bootDrcTexSize);
		IOSUHAX_FSA_CloseFile(fsaFd, sdFd);
		sdFd = -1;
		//write to nand
		sprintf(path,"%s/meta/bootDrcTex.tga",SelectedGame->path);
		if(IOSUHAX_FSA_OpenFile(fsaFd, path, "wb", &mlcFd) >= 0)
		{
			println(line++,"Writing bootDrcTex.tga...");
			fsa_write(fsaFd, mlcFd, bootDrcTex, bootDrcTexSize);
			IOSUHAX_FSA_CloseFile(fsaFd, mlcFd);
			mlcFd = -1;
		}
		MEMBucket_free(bootDrcTex);
	}

	sprintf(sdPath,"%s/bootTvTex.tga",sdHaxchiPath);
	if(IOSUHAX_FSA_OpenFile(fsaFd, sdPath, "rb", &sdFd) >= 0)
	{
		//read in sd file
		fileStat_s stats;
		IOSUHAX_FSA_StatFile(fsaFd, sdFd, &stats);
		size_t bootTvTexSize = stats.size;
		uint8_t *bootTvTex = MEMBucket_alloc(bootTvTexSize,4);
		fsa_read(fsaFd, sdFd, bootTvTex, bootTvTexSize);
		IOSUHAX_FSA_CloseFile(fsaFd, sdFd);
		sdFd = -1;
		//write to nand
		sprintf(path,"%s/meta/bootTvTex.tga",SelectedGame->path);
		if(IOSUHAX_FSA_OpenFile(fsaFd, path, "wb", &mlcFd) >= 0)
		{
			println(line++,"Writing bootTvTex.tga...");
			fsa_write(fsaFd, mlcFd, bootTvTex, bootTvTexSize);
			IOSUHAX_FSA_CloseFile(fsaFd, mlcFd);
			mlcFd = -1;
		}
		MEMBucket_free(bootTvTex);
	}

	sprintf(sdPath,"%s/iconTex.tga",sdHaxchiPath);
	if(IOSUHAX_FSA_OpenFile(fsaFd, sdPath, "rb", &sdFd) >= 0)
	{
		//read in sd file
		fileStat_s stats;
		IOSUHAX_FSA_StatFile(fsaFd, sdFd, &stats);
		size_t iconTexSize = stats.size;
		uint8_t *iconTex = MEMBucket_alloc(iconTexSize,4);
		fsa_read(fsaFd, sdFd, iconTex, iconTexSize);
		IOSUHAX_FSA_CloseFile(fsaFd, sdFd);
		sdFd = -1;
		//write to nand
		sprintf(path,"%s/meta/iconTex.tga",SelectedGame->path);
		if(IOSUHAX_FSA_OpenFile(fsaFd, path, "wb", &mlcFd) >= 0)
		{
			println(line++,"Writing iconTex.tga...");
			fsa_write(fsaFd, mlcFd, iconTex, iconTexSize);
			IOSUHAX_FSA_CloseFile(fsaFd, mlcFd);
			mlcFd = -1;
		}
		MEMBucket_free(iconTex);
	}

	sprintf(sdPath,"%s/bootSound.btsnd",sdHaxchiPath);
	if(IOSUHAX_FSA_OpenFile(fsaFd, sdPath, "rb", &sdFd) >= 0)
	{
		//read in sd file
		fileStat_s stats;
		IOSUHAX_FSA_StatFile(fsaFd, sdFd, &stats);
		size_t bootSoundSize = stats.size;
		uint8_t *bootSound = MEMBucket_alloc(bootSoundSize,4);
		fsa_read(fsaFd, sdFd, bootSound, bootSoundSize);
		IOSUHAX_FSA_CloseFile(fsaFd, sdFd);
		sdFd = -1;
		//write to nand
		sprintf(path,"%s/meta/bootSound.btsnd",SelectedGame->path);
		if(IOSUHAX_FSA_OpenFile(fsaFd, path, "wb", &mlcFd) >= 0)
		{
			println(line++,"Writing bootSound.btsnd...");
			fsa_write(fsaFd, mlcFd, bootSound, bootSoundSize);
			IOSUHAX_FSA_CloseFile(fsaFd, mlcFd);
			mlcFd = -1;
		}
		MEMBucket_free(bootSound);
	}

#ifdef CB
	if(IOSUHAX_FSA_OpenFile(fsaFd, systemXmlPath, "rb", &slcFd) >= 0)
	{
		//read in system xml file
		fileStat_s stats;
		IOSUHAX_FSA_StatFile(fsaFd, slcFd, &stats);
		size_t sysXmlSize = stats.size;
		char *sysXmlBuf = MEMBucket_alloc(sysXmlSize+1,4);
		memset(sysXmlBuf, 0, sysXmlSize+1);
		fsa_read(fsaFd, slcFd, sysXmlBuf, sysXmlSize);
		IOSUHAX_FSA_CloseFile(fsaFd, slcFd);
		slcFd = -1;
		xmlDocPtr doc = xmlReadMemory(sysXmlBuf, sysXmlSize, "system.xml", "utf-8", 0);
		//change default title id
		int idFound = 0, idCorrect = 0;
		xmlNode *root_element = xmlDocGetRootElement(doc);
		xmlNode *cur_node = NULL;
		for (cur_node = root_element->children; cur_node; cur_node = cur_node->next) {
			if (cur_node->type == XML_ELEMENT_NODE) {
				if(memcmp(cur_node->name, "default_title_id", 17) == 0)
				{
					if(xmlNodeGetContent(cur_node) == NULL || !strlen((char*)xmlNodeGetContent(cur_node))) continue;
					if(memcmp(sysmenuId, (char*)xmlNodeGetContent(cur_node), 17) == 0) idCorrect++;
					idFound++;
				}
			}
		}
		if(idFound != 1)
			println(line++,"default_title_id missing!");
		else if(idCorrect != 1)
			println(line++,"default_title_id not set to System Menu!");
		else
		{
			int xmlBackedUp = 0;
			if(IOSUHAX_FSA_OpenFile(fsaFd, syshaxXmlPath, "rb", &slcFd) < 0)
			{
				//write syshax.xml
				if(IOSUHAX_FSA_OpenFile(fsaFd, syshaxXmlPath, "wb", &slcFd) >= 0)
				{
					println(line++,"Writing syshax.xml...");
					fsa_write(fsaFd, slcFd, sysXmlBuf, sysXmlSize);
					xmlBackedUp = 1;
					IOSUHAX_FSA_CloseFile(fsaFd, slcFd);
					slcFd = -1;
				}
			}
			else
			{
				println(line++,"syshax.xml already found, skipping...");
				xmlBackedUp = 1;
				IOSUHAX_FSA_CloseFile(fsaFd, slcFd);
				slcFd = -1;
			}
			if(xmlBackedUp == 0)
				println(line++,"Failed to back up system.xml!");
			else
			{
				idFound = 0, idCorrect = 0;
				root_element = xmlDocGetRootElement(doc);
				cur_node = NULL;
				for (cur_node = root_element->children; cur_node; cur_node = cur_node->next) {
					if (cur_node->type == XML_ELEMENT_NODE) {
						if(memcmp(cur_node->name, "default_title_id", 17) == 0)
						{
							if(xmlNodeGetContent(cur_node) == NULL || !strlen((char*)xmlNodeGetContent(cur_node))) continue;
							if(memcmp(sysmenuId, (char*)xmlNodeGetContent(cur_node), 17) == 0)
							{
								xmlNodeSetContent(cur_node, (xmlChar*)new_title_id);
								idCorrect++;
							}
							idFound++;
						}
					}
				}
				if(idFound != 1)
					println(line++,"default_title_id missing!");
				else if(idCorrect != 1)
					println(line++,"default_title_id not set to System Menu!");
				else
				{
					//back to xml
					xmlChar *newXml = NULL;
					int newSize = 0;
					xmlSaveNoEmptyTags = 0; //yep, different from meta.xml style
					xmlDocDumpFormatMemoryEnc(doc, &newXml, &newSize, "utf-8", 0);
					xmlFreeDoc(doc);
					if(newXml != NULL && newSize > 0)
					{
						//libxml2 adds in extra \n at the end
						if(newXml[newSize-1] == '\n' && sysXmlBuf[sysXmlSize-1] != '\n')
						{
							newXml[newSize-1] = '\0';
							newSize--;
						}
						//write back to nand
						if(IOSUHAX_FSA_OpenFile(fsaFd, systemXmlPath2, "wb", &slcFd) >= 0)
						{
							println(line++,"Writing system.xml...");
							//UTF-8 BOM
							char bom[3] = { 0xEF, 0xBB, 0xBF };
							if(memcmp(newXml, bom, 3) != 0 && memcmp(sysXmlBuf, bom, 3) == 0)
								fsa_write(fsaFd, slcFd, bom, 0x03);
							fsa_write(fsaFd, slcFd, newXml, newSize);
							IOSUHAX_FSA_CloseFile(fsaFd, slcFd);
							slcFd = -1;
						}
						free(newXml);
					}
				}
			}
		}
		MEMBucket_free(sysXmlBuf);
	}
	println(line++,"Done installing CBHC!");
#else
	println(line++,"Done installing Haxchi!");
#endif

WHBLogPrintf("Success");	
WHBLogConsoleDraw();

prgEnd:
	if(tList) //cant be in MEMBucket
		free(tList);	
	//close trigger everything fsa related
	if(fsaFd >= 0)
	{
		if(slcFd >= 0)
			IOSUHAX_FSA_CloseFile(fsaFd, slcFd);
		if(mlcFd >= 0)
			IOSUHAX_FSA_CloseFile(fsaFd, mlcFd);
		if(sdFd >= 0)
			IOSUHAX_FSA_CloseFile(fsaFd, sdFd);
		if(sdMounted)
			IOSUHAX_FSA_Unmount(fsaFd, sdCardVolPath, 2);
		
		if(IOSUHAX_FSA_FlushVolume(fsaFd, "/vol/storage_mlc01") == 0)
			println(line++, "Flushed NAND Cache!");
		
		IOSUHAX_FSA_Close(fsaFd);
	}

	IOSUHAX_Close();
	
	sleep(5);
	
	OSForceFullRelaunch();
	ProcUIShutdown();
	
	memoryRelease();
	WHBLogConsoleFree();	
	SYSRelaunchTitle(0, NULL);
	WHBProcShutdown();
	
	return 0;
}
