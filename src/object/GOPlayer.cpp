/*
 * GOPlayer.cpp
 *
 *  Created on: 13.01.2010
 *      Author: Администратор
 */

#include "GOPlayer.h"
#include "../MGConfig.h"
#include "GOModel.h"

GOPlayer::GOPlayer(string id) : GOCreature(id) {
	init();
	bindLua();
}


GOPlayer::GOPlayer(FILE* f) : GOCreature(f) {
	init();
	bindLua();
}


void GOPlayer::init() {
	setModel("mario2.md2");
	type = GO_PLAYER;
	jumpPower = game->config->getVector("player_jump_power");
}


void GOPlayer::bindLua() {
}


void GOPlayer::draw() {
	GOCreature::draw();
}


void GOPlayer::moveRight() {
	setState(CS_MOVE_RIGHT);
}


void GOPlayer::moveLeft() {
	setState(CS_MOVE_LEFT);
}


void GOPlayer::stopRun() {
	setState(CS_STAND);
}


void GOPlayer::jump() {
	setState(CS_JUMP);
}


void GOPlayer::update(float dt) {
	GOCreature::update(dt);
}


GOPlayer::~GOPlayer() {
	Game::instance->player = NULL;
	// TODO Auto-generated destructor stub
}
