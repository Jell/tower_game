/*
 * display.h
 *
 *  Created on: 2 déc. 2009
 *      Author: Jean-Louis
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_
extern "C" {
	#include <nds.h>
	#include <stdio.h>
	#include "font.h"
	#include "leftmenu.h"

}
#include "mapengine.h"

namespace Display
{
	extern PrintConsole main_console;
	extern PrintConsole sub_console;
	extern MapEngine *mapEngine;
	extern ConsoleFont font;
	extern int bg3;
	extern int bg2Sub;
	extern int bg3Sub;
	void initMemory();
	void init3DSettings();
	void init();
}

#endif /* DISPLAY_H_ */
