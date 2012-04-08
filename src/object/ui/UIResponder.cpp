/*
 * UIResponder.cpp
 *
 *  Created on: 08.06.2010
 *      Author: Администратор
 */

#include "UIResponder.h"
#include "../../Game.h"

UIResponder::UIResponder() {
	responsible = true;
	isDownByCursor = -1;
	isSomeCursorOver = false;
	for (int i = 0; i < 10; i ++)
		cursorsOver[i] = false;
	Game::instance->responders.push_back(this);
}


bool UIResponder::getResponsible() {
	return responsible;
}


void UIResponder::setResponsible(bool v) {
	responsible = v;
}


bool UIResponder::hitTest(CursorState c) {
	return false;
}


void UIResponder::cursorDown(CursorState c) {

}


void UIResponder::cursorMoved(CursorState c) {

}


void UIResponder::cursorUp(CursorState c) {

}


void UIResponder::cursorOver(CursorState c) {

}


void UIResponder::cursorLeave(CursorState c) {

}


float UIResponder::responseOrder() {
	return 0.0f;
}


UIResponder::~UIResponder() {
	// remove from list
	vector<UIResponder*>::iterator p = Game::instance->responders.begin();
	for (; p < Game::instance->responders.end(); p++) {
		if (this == *p) {
			Game::instance->responders.erase(p);
			break;
		}
	}
	// remove from event mechanism
	p = Game::instance->firstResponders.begin();
	for (; p < Game::instance->firstResponders.end(); p++) {
		if (this == *p) {
			Game::instance->firstResponders.erase(p);
			break;
		}
	}
}
