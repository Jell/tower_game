/*
 * map.cpp
 *
 *  Created on: 1 déc. 2009
 *      Author: Jean-Louis
 */
#include "map.h"

Map::Map() {
	// Generate a level
	newLevel();
}

Map::~Map() {
	// TODO Auto-generated destructor stub
}

void Map::generateCave() {
	u32 i, j, inI, inJ, outI, outJ;

	// Clear Map
	for (i = 0; i < OBJ_MAP_WIDTH; i++) {
		for (j = 0; j < OBJ_MAP_HEIGHT; j++) {
			objectMap[i][j] = TILE_NULL;
		}
	}

	inI = OBJ_MAP_WIDTH/2;
	inJ = OBJ_MAP_HEIGHT/2;

	objectMap[inI][inJ] = TILE_IN;

	u32 count, randI, randJ;
	randI = inI + 1;
	randJ = inJ;
	for (count = 0; count < OBJ_MAP_WIDTH * OBJ_MAP_HEIGHT; count++) {
		if (objectMap[randI][randJ] == TILE_NULL) {
			objectMap[randI][randJ] = TILE_GROUND;
		}
		switch (rand() % 4) {
		case 0:
			randI++;
			randI -= (randI == (OBJ_MAP_WIDTH - 6));
			break;
		case 1:
			randI--;
			randI += (randI == 4);
			break;
		case 2:
			randJ++;
			randJ -= (randJ == (OBJ_MAP_HEIGHT - 6));
			break;
		case 3:
			randJ--;
			randJ += (randJ == 4);
			break;
		}
	}

	outI = randI;
	outJ = randJ;
	objectMap[outI][outJ] = TILE_OUT;

	smoothPath();
	wallsAround();

	scroll_x = 0;
	scroll_y = 0;
}

void Map::generateDungeon() {
	// Clear Map
	for (u16 i = 0; i < OBJ_MAP_WIDTH; i++) {
		for (u16 j = 0; j < OBJ_MAP_HEIGHT; j++) {
			objectMap[i][j] = TILE_NULL;
		}
	}

	u8 *borderListI = new u8[512];
	u8 *borderListJ = new u8[512];

	u16 inI = OBJ_MAP_WIDTH/2;
	u16 inJ = OBJ_MAP_HEIGHT/2;
	u16 outI = 0;
	u16 outJ = 0;

	u16 count = 0;
	u16 randI = inI;
	u16 randJ = inJ;

	u16 off_i_left = (rand() % 3) + 2;
	u16 off_i_right = (rand() % 3) + 2;
	u16 off_j_up = (rand() % 3) + 2;
	u16 off_j_down = (rand() % 3) + 2;

	for (u16 i = randI - off_i_left; i < randI + off_i_right; i++)
	{
		for (u16 j = randJ - off_j_up; j < randJ + off_j_down; j++)
		{
			if (i == randI - off_i_left ||
				i == randI + off_i_right - 1 ||
				j == randJ - off_j_up ||
				j == randJ + off_j_down - 1)
			{
				borderListI[count] = i;
				borderListJ[count] = j;
				count++;
			}
			objectMap[i][j] = TILE_GROUND;
		}
	}

	u8 drawRoom;
	u8 corridorLength;
	u16 randCount;

	while (count < 511) {
		drawRoom = false;
		randCount = rand() % count;

		randI = borderListI[randCount];
		randJ = borderListJ[randCount];

		corridorLength = (rand() % 5) + 3;

		if ((objectMap[randI + 1][randJ] == TILE_NULL)&&
			(randI + corridorLength + 1 < OBJ_MAP_WIDTH - 6))
		{
			for (u16 i = 1; i <= corridorLength; i++)
			{
				objectMap[randI + i][randJ] = TILE_GROUND;
			}
			off_i_left = 0;
			off_i_right = (rand() % 6) + 2;
			off_j_up = (rand() % 3) + 2;
			off_j_down = (rand() % 3) + 2;

			randI = randI + corridorLength;
			drawRoom = true;

		} else if ((objectMap[randI - 1][randJ] == TILE_NULL)&&
					(randI - corridorLength - 1 > 6))
		{

			for (u16 i = 1; i <= corridorLength; i++) {
				objectMap[randI - i][randJ] = TILE_GROUND;
			}
			off_i_left = (rand() % 6) + 2;
			off_i_right = 0;
			off_j_up = (rand() % 3) + 2;
			off_j_down = (rand() % 3) + 2;

			randI = randI - corridorLength;
			drawRoom = true;

		} else if ((objectMap[randI][randJ + 1] == TILE_NULL)&&
				(randJ + corridorLength + 1 < OBJ_MAP_HEIGHT - 6))
		{

			for (u16 j = 1; j <= corridorLength; j++) {
				objectMap[randI][randJ + j] = TILE_GROUND;
			}
			off_i_left = (rand() % 3) + 2;
			off_i_right = (rand() % 3) + 2;
			off_j_up = 0;
			off_j_down = (rand() % 6) + 2;

			randJ = randJ + corridorLength;
			drawRoom = true;

		} else if ((objectMap[randI][randJ - 1] == TILE_NULL)&&(randJ - corridorLength - 1 > 6))
		{
			objectMap[randI][randJ - 1] = TILE_GROUND;

			for (u16 j = 1; j <= corridorLength; j++) {
				objectMap[randI][randJ - j] = TILE_GROUND;
			}
			off_i_left = (rand() % 3) + 2;
			off_i_right = (rand() % 3) + 2;
			off_j_up = (rand() % 6) + 2;
			off_j_down = 0;

			randJ = randJ - corridorLength;
			drawRoom = true;
		}

		if(randI - off_i_left < 6) off_i_left = randI - 6;
		if(randJ - off_j_up < 6) off_j_up = randJ - 6;
		if(randI + off_i_right > OBJ_MAP_WIDTH - 6) off_i_right = OBJ_MAP_WIDTH - 6 - randI;
		if(randJ + off_j_down > OBJ_MAP_WIDTH - 6) off_j_down = OBJ_MAP_WIDTH - 6 - randJ;

		if(drawRoom == true){
			for (u16 i = randI - off_i_left; i < randI + off_i_right; i++)
			{
				for (u16 j = randJ - off_j_up; j < randJ + off_j_down; j++)
				{
					if ((i == randI - off_i_left || i == randI + off_i_right - 1
							|| j == randJ - off_j_up || j == randJ + off_j_down - 1)
							&& count < 511)
					{
						borderListI[count] = i;
						borderListJ[count] = j;
						count++;
					}

					objectMap[i][j] = TILE_GROUND;
				}
			}

			outI = randI + (off_i_right - off_i_left)/2 ;
			outJ = randJ + (off_j_down - off_j_up)/2;
		}
	}
	/*
	 for (i = 0; i < count;i++){
	 objectMap[borderListI[i]][borderListJ[i]] = TILE_OUT;
	 }
	 */

	randI = inI;
	randJ = inJ;

	//outI = randI;
	//outJ = randJ;
	//objectMap[outI][outJ] = TILE_OUT;

	wallsAround();

	objectMap[inI][inJ] = TILE_IN;
	objectMap[outI][outJ] = TILE_OUT;

	delete [] borderListI;
	delete [] borderListJ;

	scroll_x = 0;
	scroll_y = 0;
}

void Map::smoothPath() {
	for (u32 i = 2; i < OBJ_MAP_WIDTH - 2; i++) {
		for (u32 j = 2; j < OBJ_MAP_HEIGHT - 2; j++) {
			if (IS_OBJ_GRND(i, j) &&
				(
					(
						IS_OBJ_NULL(i, j - 1) ^
							(IS_OBJ_NULL(i - 1, j - 1) && IS_OBJ_NULL(i + 1, j - 1))
					) || (
						IS_OBJ_NULL(i, j + 1) ^
							(IS_OBJ_NULL(i - 1, j + 1) && IS_OBJ_NULL(i + 1, j + 1))
					) || (
						IS_OBJ_NULL(i - 1, j) ^
							(IS_OBJ_NULL(i - 1, j - 1) && IS_OBJ_NULL(i - 1, j + 1))
					) || (
						IS_OBJ_NULL(i + 1, j) ^
							(IS_OBJ_NULL(i + 1, j - 1) && IS_OBJ_NULL(i + 1, j + 1))
					)
				))
			{
				for (u32 itmp = i - 1; itmp < i + 1; itmp++) {
					for (u32 jtmp = j - 1; jtmp < j + 1; jtmp++) {
						if (IS_OBJ_NULL(itmp, jtmp)) {
							objectMap[itmp][jtmp] = TILE_GROUND;
						}

					}
				}

			}
		}
	}
}

void Map::wallsAround() {
	for (u32 i = 0; i < OBJ_MAP_WIDTH - 1; i++) {
		for (u32 j = 1; j < OBJ_MAP_HEIGHT - 1; j++) {
			if (
					IS_OBJ_NULL(i, j) &&
					(
						IS_OBJ_GRND(i, j + 1) ||
						IS_OBJ_GRND(i, j - 1) ||
						IS_OBJ_GRND(i - 1, j) ||
						IS_OBJ_GRND(i + 1, j) ||
						IS_OBJ_GRND(i - 1, j + 1) ||
						IS_OBJ_GRND(i - 1, j - 1) ||
						IS_OBJ_GRND(i + 1, j + 1) ||
						IS_OBJ_GRND(i + 1, j - 1)
					)
				)
			{
				objectMap[i][j] = TILE_WALL;
			}
		}
	}
}

void Map::generateOutside() {
	u32 i, j;
	// Clear Map
	for (i = 0; i < OBJ_MAP_WIDTH; i++) {
		for (j = 0; j < OBJ_MAP_HEIGHT; j++) {
			objectMap[i][j] = TILE_NULL;
		}
	}

	for (i = 5; i < OBJ_MAP_WIDTH - 5; i++) {
		for (j = 6; j < OBJ_MAP_HEIGHT - 5; j++) {
			objectMap[i][j] = TILE_GROUND;
		}
	}

	drawBorders();
	smoothBorders();

}

void Map::drawBorders() {

	u32 i, j;
	u32 borderWidth = (rand() % 8) + 3;//PA_RandMinMax(3, 10);
	u32 borderHeight = (rand() % 4) + 1; //PA_RandMinMax(1, 4);
	i = 5;
	u32 itmp, jtmp;
	while (i < OBJ_MAP_WIDTH - 5) {
		for (itmp = i; (itmp < i + borderWidth) && (itmp < OBJ_MAP_WIDTH - 5); itmp++) {
			for (jtmp = 5; jtmp < 5 + borderHeight; jtmp++) {
				objectMap[itmp][jtmp] = TILE_BORDER;
			}
		}
		i += borderWidth;
		borderWidth = (rand() % 8) + 3;
		borderHeight = (rand() % 4) + 1;
	}

	i = 5;
	while (i < OBJ_MAP_WIDTH - 5) {
		for (itmp = i; (itmp < i + borderWidth) && (itmp < OBJ_MAP_WIDTH - 5); itmp++) {
			for (jtmp = OBJ_MAP_HEIGHT - 5 - borderHeight; jtmp
					< OBJ_MAP_HEIGHT - 5; jtmp++) {
				objectMap[itmp][jtmp] = TILE_BORDER;
			}
		}
		i += borderWidth;
		borderWidth = (rand() % 8) + 3;
		borderHeight = (rand() % 4) + 1;
	}

	j = 5;
	while (j < OBJ_MAP_HEIGHT - 5) {
		for (itmp = 5; itmp < 5 + borderHeight; itmp++) {
			for (jtmp = j; (jtmp < j + borderWidth) && (jtmp < OBJ_MAP_HEIGHT
					- 5); jtmp++) {
				objectMap[itmp][jtmp] = TILE_BORDER;
			}
		}
		j += borderWidth;
		borderWidth = (rand() % 8) + 3;
		borderHeight = (rand() % 4) + 1;
	}

	j = 5;
	while (j < OBJ_MAP_HEIGHT - 5) {
		for (itmp = OBJ_MAP_WIDTH - 5 - borderHeight; itmp < OBJ_MAP_WIDTH - 5; itmp++) {
			for (jtmp = j; (jtmp < j + borderWidth) && (jtmp < OBJ_MAP_HEIGHT
					- 5); jtmp++) {
				objectMap[itmp][jtmp] = TILE_BORDER;
			}
		}
		j += borderWidth;
		borderWidth = (rand() % 8) + 3; //PA_RandMinMax(3, 10);
		borderHeight = (rand() % 4) + 1; //PA_RandMinMax(1, 4);
	}
}

void Map::smoothBorders() {
	for (u32 i = 5; i < OBJ_MAP_WIDTH - 5; i++) {
		for (u32 j = 5; j < OBJ_MAP_WIDTH - 5; j++) {
			if (objectMap[i][j] == TILE_GROUND &&
					(
						(
							objectMap[i + 1][j] == TILE_BORDER &&
							objectMap[i - 1][j] == TILE_BORDER
						) || (
							objectMap[i][j + 1] == TILE_BORDER &&
							objectMap[i][j - 1] == TILE_BORDER
						)
					)
				)
			{
				objectMap[i][j] = TILE_BORDER;
			}
		}
	}

}
void Map::move(s32 x, s32 y) {
	bgUpdate();
	u8 test;

	// Define the scrolling parameters ----------------------------------
	if (x < 0) {
		Display::mapEngine->offset_x = (scroll_x >> 8) / 8 - 1;
		scroll_x += x;
		test = testWalls();

		if ((test & 3) != 0)
			scroll_x -= x;

		if (scroll_x < 0)
			scroll_x = 0;
		else
			Display::mapEngine->movingHorizontal = true;
	} else if (x > 0) {
		Display::mapEngine->offset_x = (scroll_x >> 8) / 8 + SCR_WIDTH;
		scroll_x += x;

		test = testWalls();
		if ((test & 3) != 0)
			scroll_x -= x;

		if ((scroll_x >> 8) >= ((MAP_WIDTH - SCR_WIDTH) * TILE_WIDTH))
			scroll_x = ((MAP_WIDTH - SCR_WIDTH) * TILE_WIDTH - 1) << 8;
		else
			Display::mapEngine->movingHorizontal = true;
	}

	if (y < 0) {
		Display::mapEngine->offset_y = (scroll_y >> 8) / 8 - 1;
		scroll_y += y;

		test = testWalls();
		if ((test & 12) != 0)
			scroll_y -= y;

		if (scroll_y < 0)
			scroll_y = 0;
		else
			Display::mapEngine->movingVertical = true;

	} else if (y > 0) {
		Display::mapEngine->offset_y = (scroll_y >> 8) / 8 + SCR_HEIGHT;
		scroll_y += y;

		test = testWalls();
		if ((test & 12) != 0)
			scroll_y -= y;

		if ((scroll_y >> 8) >= (MAP_HEIGHT - SCR_HEIGHT) * TILE_WIDTH)
			scroll_y = ((MAP_HEIGHT - SCR_HEIGHT) * TILE_WIDTH - 1) << 8;
		else
			Display::mapEngine->movingVertical = true;
	}

	/*	// Find the object map index of the upper right corner
	int map_index_x = div32(scroll_x >> 8, 32);
	int map_index_y = div32(scroll_y >> 8, 32);

	// Find the offsets of this object on the screen
	int jitter_x = (scroll_x >> 8) & 31;
	int jitter_y = (scroll_y >> 8) & 31;

	// Test all the objects on screen and set them to the right position
	for (int ix = 0; ix <= 8; ix++) {
		for (int iy = 0; iy <= 6; iy++) {
			if (IS_OBJ_WALL(map_index_x + ix, map_index_y + iy)) {
				Display::mapEngine->drawCube(32 * ix - jitter_x, 32 * (iy + 1) - jitter_y);
			}
		}
	}*/

	Display::mapEngine->setScroll(scroll_x, scroll_y);
	Display::mapEngine->move(x, y);
}

u8 Map::testDoors() {
	u32 a = (scroll_x >> 8) + CENTER_X;
	u32 b = (scroll_y >> 8) + CENTER_Y;
	return (objectMap[a/32][b/32] == TILE_OUT);
}

u8 Map::testWalls() {
	u32 a = (scroll_x >> 8) + CENTER_X;
	u32 b = (scroll_y >> 8) + CENTER_Y;

	return 1 * (objectMap[(a + 11) / 32][b / 32] == TILE_WALL) +
		   2 * (objectMap[(a - 11) / 32][b / 32] == TILE_WALL) +
		   4 * (objectMap[a / 32][(b + 11) / 32] == TILE_WALL) +
		   8 * (objectMap[a / 32][(b - 11) / 32] == TILE_WALL) +
		   5 * (
				   (objectMap[(a + 10) / 32][(b - 10) / 32] == TILE_WALL) ||
				   (objectMap[(a + 10) / 32][(b + 10) / 32] == TILE_WALL) ||
				   (objectMap[(a - 10) / 32][(b - 10) / 32] == TILE_WALL) ||
				   (objectMap[(a - 10) / 32][(b + 10) / 32] == TILE_WALL)
				);
}

void Map::renderMap() {
	for (u32 i = 0; i < OBJ_MAP_WIDTH; i++) {
		for (u32 j = 0; j < OBJ_MAP_HEIGHT; j++) {
			Display::mapEngine->setMapTile(objectMap[i][j], i, j);
		}
	}
}

void Map::newLevel() {

	if(rand()&1) generateDungeon();
	else generateCave();
	Display::mapEngine->currentTexture = rand() & 7;
	renderMap();

	Display::mapEngine->newLevel();
	scroll_x = Display::mapEngine->getScrollX();
	scroll_y = Display::mapEngine->getScrollY();
}
