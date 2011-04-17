/*
 * map.h
 *
 *  Created on: 1 déc. 2009
 *      Author: Jean-Louis
 */

#ifndef MAP_H_
#define MAP_H_

extern "C" {
	#include <nds.h>
	#include <stdio.h>
	#include "constants.h"
}
#include "mapengine.h"
#include "display.h"

#define IS_OBJ_NULL(i,j)		(objectMap[(i)][(j)] == TILE_NULL)
#define IS_OBJ_GRND(i,j)		(objectMap[(i)][(j)] == TILE_GROUND)
#define IS_OBJ_NOT_NULL(i,j)	(objectMap[(i)][(j)] != TILE_NULL)
#define IS_OBJ_WALL(i,j)		(objectMap[(i)][(j)] == TILE_WALL)

class Map {
private:
	void drawBorders();
	void smoothBorders();
	void smoothPath();
	void wallsAround();
	void generateCave();
	void generateDungeon();
	void generateOutside();
public:
	Map();
	virtual ~Map();
	s32 scroll_x, scroll_y;
	u8 objectMap[OBJ_MAP_WIDTH][OBJ_MAP_HEIGHT];
	u8 testDoors();
	u8 testWalls();
	void move(s32 x, s32 y);
	void renderMap();
	void newLevel();

};

#endif /* MAP_H_ */
