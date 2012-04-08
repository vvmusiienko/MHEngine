/*
 * Tests.cpp
 *
 *  Created on: 03.06.2010
 *      Author: Администратор
 */

#include "Game.h"
#include "Image.h"
#include "Log.h"

#define 	IMG_NAME		"./textures/background_4.tga"
#define 	IMG_NAME_PNG	"./textures/background_4.png"

void testImageLoadingSpeed(Game* game) {



	game->startProfilerZone();
	for (int i = 0; i < 100; i ++) {
		new Image(IMG_NAME);
	}

	Log::info("ImageTGA 100 images loaded from %i ms", game->endProfilerZone());

	for (int i = 0; i < 100; i ++) {
		new Image(IMG_NAME_PNG);
	}

	Log::info("PNG100 images loaded from %i ms", game->endProfilerZone());


}
