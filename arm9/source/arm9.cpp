#include "arm9.h"

int main(void) {
	int level = 1;

	Display::init();

	iprintf("\x1b[1;3HLevel: %i", level);
	iprintf("\x1b[16;2H####################");
	iprintf("\x1b[18;2H####################");
	iprintf("\x1b[20;2H~~~~~~~~~~~~~~~~~~~~");
	// Variables for stylus control
	touchPosition touch;

	int xtmp, ytmp;

	int pos3dx = 128 << 8;
	int pos3dy = 96 << 8;
	s32 stylusAngleX = 0;
	s32 stylusAngleY = 0;

	// Randomize
	srand(time(NULL));

	// Instanciate the background
	Map *map = new Map();

	Hero *hero = new Hero();

	while (1) {

		// Get the position of the stylus
		touchRead(&touch);

		xtmp = (s32) touch.px;
		ytmp = (s32) touch.py;

		// Set the speed angle
		if (xtmp != 0 || ytmp != 0) {
			stylusAngleX = div32((xtmp - 128) * 256, sqrt32((xtmp - 128)
					* (xtmp - 128) + (96 - ytmp) * (96 - ytmp)));
			stylusAngleY = -div32((96 - ytmp) * 256, sqrt32((xtmp - 128)
					* (xtmp - 128) + (96 - ytmp) * (96 - ytmp)));
		} else {
			stylusAngleX = 0;
			stylusAngleY = 0;
		}

		pos3dx -= 2* stylusAngleX ;
		pos3dy -= 2* stylusAngleY ;

		// Set sprite rotation
		if (xtmp != 0 || ytmp != 0) {
			if((ytmp - 96 - xtmp + 128 < 0) && (ytmp - 96 + xtmp - 128> 0)) oamRotateScale(&oamMain, 0, -asinLerp(stylusAngleY*16), ZOOM_INIT, ZOOM_INIT);
			if((ytmp - 96 - xtmp + 128 > 0) && (ytmp - 96 + xtmp - 128> 0)) oamRotateScale(&oamMain, 0, -acosLerp(stylusAngleX*16), ZOOM_INIT, ZOOM_INIT);
			if((ytmp - 96 - xtmp + 128 > 0) && (ytmp - 96 + xtmp - 128< 0)) oamRotateScale(&oamMain, 0, asinLerp(stylusAngleY*16) + 16384, ZOOM_INIT, ZOOM_INIT);
			if((ytmp - 96 - xtmp + 128 < 0) && (ytmp - 96 + xtmp - 128< 0)) oamRotateScale(&oamMain, 0, acosLerp(stylusAngleX*16), ZOOM_INIT, ZOOM_INIT);

		}

		// Move Background
		map->move(2* stylusAngleX , 2* stylusAngleY );

		// Flush 3D scene to screen

		glFlush(0);

		scanKeys();
		if (map->testDoors()|| (keysDown()& KEY_A)) {
		//if (map->testDoors()) {
			iprintf("\x1b[1;10H%i", ++level);
			for (u8 i = 0; i <= 16; i++) {
				setBrightness(1, -i);
				swiWaitForVBlank();
			}
			map->newLevel();
			map->move(0, 0);
			glFlush(0);
			for (u8 i = 0; i <= 16; i++) {
				setBrightness(1, -16 + i);
				swiWaitForVBlank();
			}
		}

		hero->animate(xtmp != 0 || ytmp != 0);

		swiWaitForVBlank();
		oamUpdate(&oamMain);

	}

	return 0;
}


