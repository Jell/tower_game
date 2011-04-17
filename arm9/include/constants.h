/*
 * constants.h
 *
 *  Created on: 13 avr. 2009
 *      Author: Jean-Louis
 */

#define CONSOLE_ON			TRUE

#define CONSTANTS_H_

#define OBJ_MAP_WIDTH		104
#define OBJ_MAP_HEIGHT		104

#define CENTER_X32		112
#define CENTER_Y32		80
#define CENTER_X		128
#define CENTER_Y		96

#define TILE_NULL		0
#define TILE_GROUND		2
#define TILE_WALL		1
#define TILE_IN			3
#define TILE_OUT		4
#define TILE_BORDER		1
#define TILE_OBJ		6

#define ZOOM_INIT		384


#define BIT0	1
#define BIT1	2
#define BIT2	4
#define BIT3	8
#define BIT4	16
#define BIT5	32
#define BIT6	64
#define BIT7	128


#define TILE_WIDTH		8 				//width of a tile in pixels
#define MAP_WIDTH		(4 * OBJ_MAP_WIDTH)		//width of the big map in tiles
#define MAP_HEIGHT		(4 * OBJ_MAP_HEIGHT)		//heigh of the big map in tiles
#define BG_WIDTH		(256 / 8)		//width of the hardware map in tiles
#define BG_HEIGHT		(256 / 8)		//height of the hardware map in tiles
#define SCR_WIDTH		(256 / 8)		//screen width in tiles
#define SCR_HEIGHT		(192 / 8)		//screen height in tiles
