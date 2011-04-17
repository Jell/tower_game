#ifndef HERO_H_
#define HERO_H_

extern "C" {
	#include <nds.h>
	#include <nds.h>
	#include "heroSprite.h"
	#include "constants.h" // Include all constants
}
class Hero{
	private:
		s16 anim_increment;
		u8* offset;
		u8 spriteTimer;
		u16* sprite_gfx_mem;

	public:
		s16 anim_frame;
		Hero();
		virtual ~Hero();
		void animate(u8 animation);

};
#endif /* BACKGROUND_H_ */
