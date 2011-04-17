/*
 * display.cpp
 *
 *  Created on: 2 déc. 2009
 *      Author: Jean-Louis
 */
#include "display.h"
namespace Display
{
	PrintConsole main_console;
	PrintConsole sub_console;
	MapEngine *mapEngine;
	ConsoleFont font;
	int bg3;
	int bg2Sub;
	int bg3Sub;

	void init3DSettings() {
		glInit();
		glEnable(GL_TEXTURE_2D | GL_ANTIALIAS | GL_BLEND);

		glClearColor(31, 31, 31, 0);
		glClearPolyID(63);
		glClearDepth(GL_MAX_DEPTH);

		glViewport(0, 0, 255, 191);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glFrustum(-6.4, 6.4, -4.8, 4.8, 6.4, 25);

		glMaterialf(GL_AMBIENT, RGB15(8, 8, 8));
		glMaterialf(GL_DIFFUSE, RGB15(23, 23, 23));
		glMaterialf(GL_SPECULAR, BIT(15) | RGB15(8, 8, 8));
		glMaterialf(GL_EMISSION, RGB15(5, 5, 5));
		glMaterialShinyness();

		glLight(0, RGB15(31, 31, 31), floattov10(4.0), 0, floattov10(-0.8));
		glLight(1, RGB15(31, 31, 31), floattov10(-4.0), 0, floattov10(-0.8));
		glLight(2, RGB15(31, 31, 31), 0, floattov10(-4.0), floattov10(-0.8));
		glLight(3, RGB15(31, 31, 31), 0, floattov10(4.0), floattov10(-0.8));

		glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FORMAT_LIGHT0
				| POLY_FORMAT_LIGHT1 | POLY_FORMAT_LIGHT2 | POLY_FORMAT_LIGHT3);
	}

	void initMemory() {
		// Setup video mode for Main: 3D | Text | Text | Text
		videoSetMode(MODE_5_3D);
		// Setup video mode for Sub: Text | Text | Rotation | Rotation
		videoSetModeSub(MODE_5_2D);
		//Setup Memory Banks
		vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
		vramSetBankB(VRAM_B_MAIN_SPRITE_0x06400000);
		vramSetBankC(VRAM_C_SUB_BG_0x06200000);
		vramSetBankD(VRAM_D_TEXTURE);

		// Set Main screen on bottom
		REG_POWERCNT &= ~POWER_SWAP_LCDS;
	}

	void init() {

		initMemory();

		//Init backgrounds
		// Main 3D
		init3DSettings();
		// Main Map:
		mapEngine = new MapEngine(1, 4, 1);
		// Main Console:
		consoleInit(&main_console,3,BgType_ExRotation, BgSize_ER_256x256, 31, 0, true, false);
		bg3 = main_console.bgId;
		bgSetCenter(bg3, 254, 0);
		bgSetRotate(bg3, -8192);

		// Sub Console:
		consoleInit(&sub_console,3,BgType_ExRotation, BgSize_ER_256x256, 31, 1, false, false);
		bg3Sub = sub_console.bgId;
		bgSetCenter(bg3Sub, 254, 0);
		bgSetRotate(bg3Sub, -8192);

		// Sub Image:
		bg2Sub = bgInitSub(2, BgType_Bmp8, BgSize_B8_256x256, 4,0);
		dmaCopy(leftmenuBitmap, bgGetGfxPtr(bg2Sub), 256*192);
		dmaCopy(leftmenuPal, BG_PALETTE_SUB, leftmenuPalLen);

		// Init Sprites
		oamInit(&oamMain, SpriteMapping_1D_128, false);

		// Set the priorities
		bgSetPriority(0,1);
		bgSetPriority(1,2);
		bgSetPriority(2,3);
		bgSetPriority(3,0);
		bgSetPriority(bg3Sub, 2);
		bgSetPriority(bg2Sub, 3);

		//Init font
		font.gfx = (u16*)fontTiles;
		font.pal = (u16*)fontPal;
		font.numChars = 95;
		font.numColors =  fontPalLen / 2;
		font.bpp = 8;
		font.asciiOffset = 32;
		font.convertSingleColor = false;
		consoleSetFont(&main_console, &font);
		consoleSetFont(&sub_console, &font);

		consoleSelect(&main_console);
		iprintf("\x1b[2J");
		iprintf("Arkham Tower v0.01");
		consoleSelect(&sub_console);
		iprintf("\x1b[2J");
	}

	MapEngine *getMapEngine(){
		return mapEngine;
	}
}
