/*
 * MHAnimationQueue.cpp
 *
 *  Created on: 25.02.2011
 *      Author: Администратор
 */

#include "MHAnimationQueue.h"

MHAnimationQueue::MHAnimationQueue() {
	lastStarted = NULL;
}


void MHAnimationQueue::addAnimation(MHAnimation* a) {
	animations.push_back(a);
}

AnimationType MHAnimationQueue::getAnimationType() {
    return QueueAT;
}


void MHAnimationQueue::update(float dt) {
	MHAnimation::update(dt);

	bool haveUnprocessed = false;
	// iterate animations
	for (vector<MHAnimation*>::iterator i = animations.begin(); i != animations.end(); i++) {
		if (!(*i)->isFinished()) {
			if (lastStarted != (*i)) {
				lastStarted = (*i);
				(*i)->setOwner(owner);
			}
			(*i)->update(dt);
			haveUnprocessed = true;
			break;
		}

	}

	if (!haveUnprocessed)
		finish();
}


MHAnimationQueue::~MHAnimationQueue() {
}
