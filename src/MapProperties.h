/*
 * MapProperties.h
 *
 *  Created on: 10.03.2010
 *      Author: Администратор
 */

#ifndef MAPPROPERTIES_H_
#define MAPPROPERTIES_H_

#include "Common.h"

class Game;

class MapProperties {
	Game* game;
	string id;
	int type;

	void apply();
public:
	MGColor ambientLight;
	MGColor clearColor;
	string skyBox;
	bool enabledLighting;
	void load(FILE* f);
	void save(FILE* f);

	MapProperties(Game* game);
	virtual ~MapProperties();
};

#endif /* MAPPROPERTIES_H_ */
