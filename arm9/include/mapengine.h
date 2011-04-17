/*
 * Background.h
 *
 *  Created on: 13 avr. 2009
 *      Author: Jean-Louis
 */

#ifndef BACKGROUND_H_
#define BACKGROUND_H_

extern "C" {
	#include <nds.h>
	#include <stdio.h>
	#include <malloc.h>

	#include <nds/arm9/image.h>
	#include "wall_textureA_pcx.h"
	#include "wall_textureB_pcx.h"
	#include "wall_textureC_pcx.h"
	#include "wall_textureD_pcx.h"
	#include "wall_textureE_pcx.h"
	#include "wall_textureF_pcx.h"
	#include "wall_textureG_pcx.h"
	#include "wall_textureH_pcx.h"

	#include "ground.h"
	#include "leftmenu.h"

}
#include "constants.h" // Include all constants

#define GET_TILE(i,j)			(tileMap[(i) + 4*OBJ_MAP_WIDTH * (j)])
#define PIX2F(x)		((x) * 409.6)

class MapEngine {
private:

	int keys;

	int bgId;
	u16* bgTileMap;
	u16* bgLeftHalf;
	u16* bgRightHalf;

	int	texture[8];

	unsigned short tileMap[OBJ_MAP_WIDTH * OBJ_MAP_HEIGHT * 16];
	s32 scroll_x, scroll_y;
	void fastmoveX();
	void fastmoveY();

public:
	u32 mapInX;
	u32 mapInY;
	u8 currentTexture;
	void setMapTile(unsigned short tile, u32 x, u32 y);
	void setScroll(s32 x, s32 y);
	s32 getScrollX();
	s32 getScrollY();
	int offset_x, offset_y;
	bool movingHorizontal, movingVertical;
	MapEngine(int bg, int mapBase, int tileBase);
	virtual ~MapEngine();
	void move(s32 x, s32 y);
	void drawCube(s32 x, s32 y);
	int loadGLTextures();
	int init3DSettings();
	void newLevel();

};

#endif /* BACKGROUND_H_ */
