#include "mapengine.h"
#include "display.h"

#define GET_TILE(i,j)			(tileMap[(i) + 4*OBJ_MAP_WIDTH * (j)])

MapEngine::MapEngine(int bg, int mapBase, int tileBase) {
	currentTexture = 0;
	// Load the 3D tectures
	loadGLTextures();

	// Init the background
	bgId = bgInit(bg, BgType_Text8bpp, BgSize_T_512x256, mapBase, tileBase);

	// copy the Tiles and Palette
	dmaCopy(groundTiles, bgGetGfxPtr(bgId), groundTilesLen);
	dmaCopy(groundPal, BG_PALETTE, groundPalLen);

	// Set up the display and buffer screens
	bgTileMap = (u16*) bgGetMapPtr(bgId);
	bgLeftHalf = bgTileMap;
	bgRightHalf = bgTileMap + 32 * 32;
}

MapEngine::~MapEngine() {
	// TODO Auto-generated destructor stub
}

void MapEngine::move(s32 x, s32 y) {


	// Scroll the background to the right position
	if (movingHorizontal) {
		u16* bgTemp = ((offset_x & 63) >= BG_WIDTH) ? bgRightHalf : bgLeftHalf;

		for (
				int iy = (scroll_y >> 8) / 8 - 1;
				iy < (scroll_y >> 8) / 8 + SCR_HEIGHT + 1;
				iy++
			)
		{
			bgTemp[(offset_x & (BG_WIDTH - 1)) + (iy & (BG_HEIGHT - 1)) * 32]
					= tileMap[offset_x + iy * MAP_WIDTH];
		}
	}
	if (movingVertical) {
		for (
				int ix = (scroll_x >> 8) / 8 - 1;
				ix < (scroll_x >> 8) / 8 + SCR_WIDTH + 1;
				ix++
			)
		{
			u16* bgTemp = ((ix & 63) >= BG_WIDTH) ? bgRightHalf : bgLeftHalf;

			bgTemp[(ix & (BG_WIDTH - 1)) + (offset_y & (BG_HEIGHT - 1)) * 32]
					= tileMap[ix + offset_y * MAP_WIDTH];
		}
	}

	bgSetScroll(bgId, (scroll_x >> 8), (scroll_y >> 8));

	// Find the object map index of the upper right corner
	int map_index_x = div32(scroll_x >> 8, 32);
	int map_index_y = div32(scroll_y >> 8, 32);

	// Find the offsets of this object on the screen
	int jitter_x = (scroll_x >> 8) & 31;
	int jitter_y = (scroll_y >> 8) & 31;

	// Test all the objects on screen and set them to the right position
	for (int ix = 0; ix <= 8; ix++) {
		for (int iy = 0; iy <= 6; iy++) {
			if (GET_TILE(4 * (map_index_x + ix),4*(map_index_y + iy)) == groundMap[4]) {
				drawCube(32 * ix - jitter_x, 32 * (iy + 1) - jitter_y);
			}
		}
	}
}

void MapEngine::fastmoveX() {

	offset_x = (scroll_x >> 8) / 8 + SCR_WIDTH;
	scroll_x += 256;

	if ((scroll_x >> 8) >= ((MAP_WIDTH - SCR_WIDTH) * TILE_WIDTH))
		scroll_x = ((MAP_WIDTH - SCR_WIDTH) * TILE_WIDTH - 1) << 8;
	else
		movingHorizontal = true;

	if (movingHorizontal) {
		u16* bgTemp = ((offset_x & 63) >= BG_WIDTH) ? bgRightHalf : bgLeftHalf;

		for (int iy = (scroll_y >> 8) / 8 - 1; iy < (scroll_y >> 8) / 8
				+ SCR_HEIGHT + 1; iy++) {
			bgTemp[(offset_x & (BG_WIDTH - 1)) + (iy & (BG_HEIGHT - 1)) * 32]
					= tileMap[offset_x + iy * MAP_WIDTH];
		}
	}

	bgSetScroll(bgId, (scroll_x >> 8), (scroll_y >> 8));
	bgUpdate();
}

void MapEngine::fastmoveY() {

	offset_y = (scroll_y >> 8) / 8 + SCR_HEIGHT;
	scroll_y += 256;

	if ((scroll_y >> 8) >= (MAP_HEIGHT - SCR_HEIGHT) * TILE_WIDTH)
		scroll_y = ((MAP_HEIGHT - SCR_HEIGHT) * TILE_WIDTH - 1) << 8;
	else
		movingVertical = true;

	if (movingVertical) {
		for (int ix = (scroll_x >> 8) / 8 - 1; ix < (scroll_x >> 8) / 8
				+ SCR_WIDTH + 1; ix++) {
			u16* bgTemp = ((ix & 63) >= BG_WIDTH) ? bgRightHalf : bgLeftHalf;

			bgTemp[(ix & (BG_WIDTH - 1)) + (offset_y & (BG_HEIGHT - 1)) * 32]
					= tileMap[ix + offset_y * MAP_WIDTH];
		}
	}

	bgSetScroll(bgId, (scroll_x >> 8), (scroll_y >> 8));
	bgUpdate();
}



void MapEngine::setMapTile(unsigned short tile, u32 x, u32 y) {
	u16 *ptBg = bgGetGfxPtr(Display::bg2Sub);
	u16 mask;
	u16 pos = (1 + 142 + x + 8*256 + (256*y))/2;
	if((x%2) == 1){
		mask = 0x00FF;
	}else{
		mask = 0xFF00;
	}

	switch(tile){
	case TILE_WALL:
		ptBg[pos] &= ~mask;
		ptBg[pos] |= (mask & 0xF7F7);
		break;
	case TILE_IN:
		mapInX = x * 4 * 8 - CENTER_X32;
		mapInY = y * 4 * 8 - CENTER_Y32;
		ptBg[pos] &= ~mask;
		ptBg[pos] |= (mask & 0xF8F8);
		break;
	case TILE_OUT:
		ptBg[pos] &= ~mask;
		ptBg[pos] |= (mask & 0xF9F9);
		break;
	default:
		ptBg[pos] &= ~mask;
		ptBg[pos] |= (mask & 0x0B0B);
		break;
	}

	for (u32 j = 0; j < 4; j++) {
		for (u32 i = 0; i < 4; i++)
			tileMap[4 * x + i + 4 * OBJ_MAP_WIDTH * (4* y + j)] =
				groundMap[i
					+ ((tile & 7) << 2) + ((j + ((tile >> 3) << 3)) << 5) +
					currentTexture * 128];
	}
}



void MapEngine::drawCube(s32 x, s32 y) {
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef((float) (x - 128) / 10, (float) (96 - y) / 10, -12.8);

	glBegin(GL_QUADS);

	// Front
	glColor3b(0, 0, 0);

	glVertex3v16( PIX2F(33.1), PIX2F(33.1), PIX2F(16.01) ); // Top Right Of The Quad (Front)
	glVertex3v16( PIX2F(-1.1), PIX2F(33.1), PIX2F(16.01) ); // Top Left Of The Quad (Front)
	glVertex3v16( PIX2F(-1.1), PIX2F(-1.1), PIX2F(16.01) ); // Bottom Left Of The Quad (Front)
	glVertex3v16( PIX2F(33.1), PIX2F(-1.1), PIX2F(16.01) ); // Bottom Right Of The Quad (Front)


	// Other Faces with textures
	glBindTexture(GL_TEXTURE_2D, texture[currentTexture]);
	glColor3b(255, 255, 255);

	// Top
	glTexCoord2f(0.0, 1.0);
	glVertex3v16( PIX2F(33.0), PIX2F(33.0), PIX2F(-1.0) ); // Top Right Of The Quad (Top)
	glTexCoord2f(1.0, 1.0);
	glVertex3v16( PIX2F(-1.0),  PIX2F(33.0), PIX2F(-1.0) ); // Top Left Of The Quad (Top)
	glTexCoord2f(1.0, 0.0);
	glVertex3v16( PIX2F(-1.0),  PIX2F(33.0), PIX2F(16.0)); // Bottom Left Of The Quad (Top)
	glTexCoord2f(0.0, 0.0);
	glVertex3v16( PIX2F(33.0), PIX2F(33.0), PIX2F(16.0)); // Bottom Right Of The Quad (Bottom)

	// Bottom
	glTexCoord2f(1.0, 0.0);
	glVertex3v16( PIX2F(33.0), PIX2F(-1.0), PIX2F(16.0)); // Top Right Of The Quad (Bottom)
	glTexCoord2f(0.0, 0.0);
	glVertex3v16( PIX2F(-1.0),  PIX2F(-1.0), PIX2F(16.0)); // Top Left Of The Quad (Bottom)
	glTexCoord2f(0.0, 1.0);
	glVertex3v16( PIX2F(-1.0),  PIX2F(-1.0), PIX2F(-1.0) ); // Bottom Left Of The Quad (Bottom)
	glTexCoord2f(1.0, 1.0);
	glVertex3v16( PIX2F(33.0), PIX2F(-1.0), PIX2F(-1.0) ); // Bottom Right Of The Quad (Bottom)


	// Left
	glTexCoord2f(0.0, 0.0);
	glVertex3v16( PIX2F(-1.0), PIX2F(33.0), PIX2F(16.0)); // Top Right Of The Quad (Left)
	glTexCoord2f(0.0, 1.0);
	glVertex3v16( PIX2F(-1.0), PIX2F(33.0), PIX2F(-1.0) ); // Top Left Of The Quad (Left)
	glTexCoord2f(1.0, 1.0);
	glVertex3v16( PIX2F(-1.0), PIX2F(-1.0),  PIX2F(-1.0) ); // Bottom Left Of The Quad (Left)
	glTexCoord2f(1.0, 0.0);
	glVertex3v16( PIX2F(-1.0), PIX2F(-1.0),  PIX2F(16.0)); // Bottom Right Of The Quad (Left)

	// Right
	glTexCoord2f(1.0, 1.0);
	glVertex3v16( PIX2F(33.0), PIX2F(33.0), PIX2F(-1.0) ); // Top Right Of The Quad (Right)
	glTexCoord2f(1.0, 0.0);
	glVertex3v16( PIX2F(33.0), PIX2F(33.0), PIX2F(16.0)); // Top Left Of The Quad (Right)
	glTexCoord2f(0.0, 0.0);
	glVertex3v16( PIX2F(33.0), PIX2F(-1.0),  PIX2F(16.0)); // Bottom Left Of The Quad (Right)
	glTexCoord2f(0.0, 1.0);
	glVertex3v16( PIX2F(33.0), PIX2F(-1.0),  PIX2F(-1.0) ); // Bottom Right Of The Quad (Right)

	glEnd();

	glPopMatrix(1);

}

/**
 * Load Texture File
 */
int MapEngine::loadGLTextures() {
	sImage pcx;

	glGenTextures(8, &texture[0]);
	//load our texture

	loadPCX((u8*) wall_textureA_pcx, &pcx);
	image8to16trans(&pcx, 255);
	glBindTexture(0, texture[0]);
	glTexImage2D(0, 0, GL_RGBA, TEXTURE_SIZE_32, TEXTURE_SIZE_32, 0,
				TEXGEN_TEXCOORD, pcx.image.data8);

	loadPCX((u8*) wall_textureB_pcx, &pcx);
	image8to16trans(&pcx, 255);
	glBindTexture(1, texture[1]);
	glTexImage2D(1, 0, GL_RGBA, TEXTURE_SIZE_32, TEXTURE_SIZE_32, 0,
					TEXGEN_TEXCOORD, pcx.image.data8);

	loadPCX((u8*) wall_textureC_pcx, &pcx);
	image8to16trans(&pcx, 255);
	glBindTexture(2, texture[2]);
	glTexImage2D(2, 0, GL_RGBA, TEXTURE_SIZE_32, TEXTURE_SIZE_32, 0,
					TEXGEN_TEXCOORD, pcx.image.data8);

	loadPCX((u8*) wall_textureD_pcx, &pcx);
	image8to16trans(&pcx, 255);
	glBindTexture(3, texture[3]);
	glTexImage2D(3, 0, GL_RGBA, TEXTURE_SIZE_32, TEXTURE_SIZE_32, 0,
					TEXGEN_TEXCOORD, pcx.image.data8);

	loadPCX((u8*) wall_textureE_pcx, &pcx);
	image8to16trans(&pcx, 255);
	glBindTexture(4, texture[4]);
	glTexImage2D(4, 0, GL_RGBA, TEXTURE_SIZE_32, TEXTURE_SIZE_32, 0,
					TEXGEN_TEXCOORD, pcx.image.data8);

	loadPCX((u8*) wall_textureF_pcx, &pcx);
	image8to16trans(&pcx, 255);
	glBindTexture(5, texture[5]);
	glTexImage2D(5, 0, GL_RGBA, TEXTURE_SIZE_32, TEXTURE_SIZE_32, 0,
					TEXGEN_TEXCOORD, pcx.image.data8);

	loadPCX((u8*) wall_textureG_pcx, &pcx);
	image8to16trans(&pcx, 255);
	glBindTexture(6, texture[6]);
	glTexImage2D(6, 0, GL_RGBA, TEXTURE_SIZE_32, TEXTURE_SIZE_32, 0,
					TEXGEN_TEXCOORD, pcx.image.data8);

	loadPCX((u8*) wall_textureH_pcx, &pcx);
	image8to16trans(&pcx, 255);
	glBindTexture(7, texture[7]);
	glTexImage2D(7, 0, GL_RGBA, TEXTURE_SIZE_32, TEXTURE_SIZE_32, 0,
					TEXGEN_TEXCOORD, pcx.image.data8);

	imageDestroy(&pcx);

	return TRUE;
}

/**
 * Creates a new game level
 */
void MapEngine::newLevel() {

	scroll_x = 0;
	scroll_y = 0;

	keys = 0;
	offset_x = 0;
	offset_y = 0;
	movingHorizontal = false;
	movingVertical = false;

	for (int iy = 0; iy < SCR_HEIGHT; iy++)
		dmaCopy(&tileMap[iy * MAP_WIDTH], &bgTileMap[iy * BG_WIDTH], SCR_WIDTH * 2);

	for (u32 i = 0; i < mapInX; i++)
		fastmoveX();
	for (u32 i = 0; i < mapInY; i++)
		fastmoveY();

	bgUpdate();
}


void MapEngine::setScroll(s32 x, s32 y){
	u16 *ptBg = bgGetGfxPtr(Display::bg2Sub);
	u16 mask;
	u16 ix = (((scroll_x>>8) + CENTER_X)>>5);
	u16 iy = (((scroll_y>>8) + CENTER_Y)>>5);
	u16 pos = (1 + 142 + ix + 8*256 + (256*iy))/2;
	if((ix&1) == 1){
		mask = 0x00FF;
	}else{
		mask = 0xFF00;
	}
	ptBg[pos] &= ~mask;
	ptBg[pos] |= (mask & 0x0B0B);

	scroll_x = x;
	scroll_y = y;

	ix = (((scroll_x>>8) + CENTER_X)>>5);
	iy = (((scroll_y>>8) + CENTER_Y)>>5);
	pos = (1 + 142 + ix + 8*256 + (256*iy))/2;
	if((ix&1) == 1){
		mask = 0x00FF;
	}else{
		mask = 0xFF00;
	}
	ptBg[pos] &= ~mask;
	ptBg[pos] |= (mask & 0xFFFF);
}

s32 MapEngine::getScrollX(){
	return scroll_x;
}
s32 MapEngine::getScrollY(){
	return scroll_y;
}
