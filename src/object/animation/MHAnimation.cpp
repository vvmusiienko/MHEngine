/*
 * MHAnimation.cpp
 *
 *  Created on: 25.02.2011
 *      Author: Администратор
 */

#include "MHAnimation.h"
#include "../../Game.h"

MHAnimation::MHAnimation() {
	onBeginAction = NULL;
	onEndAction = NULL;
	onBeginActionScript = "";
	onEndActionScript = "";

	owner = NULL;
	finished = false;
	started = false;
	paused = false;
	needFinish = false;
	secondsBeforeStart = 0.0f;
}


bool MHAnimation::isFinished() {
	return finished;
}


void MHAnimation::finish() {
	needFinish = true;
}


void MHAnimation::setOwner(GameObject* owner) {
	this->owner = owner;
}


void MHAnimation::setOnBeginAction(Action* a) {
	onBeginAction = a;
}


void MHAnimation::setOnEndAction(Action* a) {
	onEndAction = a;
}


void MHAnimation::setOnBeginAction(const string& script) {
	onBeginActionScript = script;
}


void MHAnimation::setOnEndAction(const string& script) {
	onEndActionScript = script;
}


void MHAnimation::startAfter(float seconds) {
	secondsBeforeStart = seconds;
}


void MHAnimation::onStart() {

}


void MHAnimation::update(float dt) {
	if (finished && needFinish)
		return;
	if (needFinish) {
		if (onEndAction)
			onEndAction->execute(owner);
		if (onEndActionScript != "")
			Game::instance->luaVM->doString(onEndActionScript);
		finished = true;
		return;
	}

	if (secondsBeforeStart > 0.0f) {
		secondsBeforeStart -= (1.0f / Game::instance->osAdapter->getFixedTimeStep()) * dt;
		paused = true;
		return;
	} else {
		paused = false;
	}

	if (!started) {
		if (onBeginAction)
			onBeginAction->execute(owner);
		if (onBeginActionScript != "")
			Game::instance->luaVM->doString(onBeginActionScript);
		onStart();
		started = true;
	}
}



MHAnimation::~MHAnimation() {
	// TODO Auto-generated destructor stub
}
