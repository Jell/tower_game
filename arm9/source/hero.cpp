#include "hero.h"

Hero::Hero() {
	anim_frame = 0;
	anim_increment = 1;
	spriteTimer = 0;

	// Allocate memory for 1 frame
	sprite_gfx_mem = oamAllocateGfx(&oamMain, SpriteSize_32x32,
			SpriteColorFormat_256Color);
	// Load palette
	dmaCopy(heroSpritePal, SPRITE_PALETTE, heroSpritePalLen);
	dmaCopy(heroSpriteTiles, sprite_gfx_mem, 32* 32 );

	oamSet(&oamMain, 0, 112, 80, 0, 0, SpriteSize_32x32, SpriteColorFormat_256Color,
			sprite_gfx_mem, 0, false, false, false, false, false);
	oamRotateScale(&oamMain, 0, 0, ZOOM_INIT, ZOOM_INIT);
}

Hero::~Hero() {
	// TODO Auto-generated destructor stub
}

// Sprite animation -------------------------------------
void Hero::animate(u8 animation){
	// Wait for the screen to refresh
	if(spriteTimer>4){
		if(animation)
			anim_frame+=anim_increment;
		else
			anim_frame = 2;

		if(anim_frame >= 4 || anim_frame <= 0) anim_increment = -anim_increment;

		offset = (u8 *)heroSpriteTiles + anim_frame*32*32;
		dmaCopy(offset, sprite_gfx_mem, 32*32);
		spriteTimer = 0;
	}else{
		spriteTimer++;
	}
}
