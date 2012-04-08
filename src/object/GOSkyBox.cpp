/*
 * GOSkyBox.cpp
 *
 *  Created on: 18.03.2010
 *      Author: Администратор
 */

#include "GOSkyBox.h"
#include "../Camera.h"

GOSkyBox::GOSkyBox(string texture) : GameObject() {
	type = GO_SKY_BOX;
	needSaveToMap = false;
	p = new GOPlate();
	p->setTexture(texture);
	p->setSize(CVector(width, height));
}

void GOSkyBox::draw() {
	p->setPosition(getPosition());
	p->setOrientation(getOrientation());
	p->draw();
}


void GOSkyBox::update(float dt) {
	setPosition(CVector(game->camera->position.x, game->camera->position.y, game->camera->position.z -5000));
}


void GOSkyBox::save(FILE* f) {

}


GOSkyBox::~GOSkyBox() {
	Game::instance->skyBox = NULL;
	// TODO Auto-generated destructor stub
}
