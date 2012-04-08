/*
 * GOPlayer.h
 *
 *  Created on: 13.01.2010
 *      Author: Администратор
 */

#ifndef GOPLAYER_H_
#define GOPLAYER_H_

#include "GOCreature.h"

class GOPlayer : public GOCreature {



private:
	void init();
	void bindLua();
public:
	void moveRight();
	void moveLeft();
	void stopRun();
	void jump();

	void draw();
	void update(float dt);

	GOPlayer(string id);
	GOPlayer(FILE* f);
	virtual ~GOPlayer();
};

#endif /* GOPLAYER_H_ */
