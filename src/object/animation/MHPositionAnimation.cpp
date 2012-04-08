/*
 * MHPositionAnimation.cpp
 *
 *  Created on: 25.02.2011
 *      Author: Администратор
 */

#include "MHPositionAnimation.h"

MHPositionAnimation::MHPositionAnimation(CVector endPosition, float duration, bool linear) {
	isLinear = linear;
	newPos = endPosition;

	iters_count = 2;
	if (duration < 5)
		iters_count = 6;
	if (duration < 1.5)
		iters_count = 10;
	if (duration < 1.0)
		iters_count = 16;

	iterSpeed = (1.0f / Game::instance->osAdapter->getFixedTimeStep()) / duration;
}

AnimationType MHPositionAnimation::getAnimationType() {
	return PositionAT;
}


void MHPositionAnimation::onStart() {
	MHAnimation::onStart();
	init();
}


void MHPositionAnimation::init() {
	if ((newPos - owner->getPosition()).Length() == 0.0f) {
		stopImediately = true;
		return;
	}
	stopImediately = false;

	if (isLinear) {
		this->speed = (newPos - owner->getPosition()).Length() * iterSpeed * 2.0f;
	} else {
		this->speed = ((PI * (newPos - owner->getPosition()).Length()) * iterSpeed) / 2.0f ;
	}
	this->iterSpeed = iterSpeed;
	this->newPos = newPos;
	movingStage = 0.0f;
	movingDirection = (newPos - owner->getPosition()).UnitVector();
}


void MHPositionAnimation::update(float dt) {
	MHAnimation::update(dt);
	if (paused || isFinished()) return;

	if (!stopImediately) {
		for (int i = 0; i < iters_count; i++) {
			movingStage+= (iterSpeed * dt) / iters_count;
			if (movingStage < 1.0f) {
				if (isLinear) {
					velocity = movingDirection * speed * movingStage;
				} else {
					velocity = movingDirection * speed * sinf(PI * movingStage);
				}
				owner->setPosition(owner->getPosition() + velocity * (dt / iters_count));
			} else {
				owner->setPosition(newPos);
				finish();
				i = iters_count;
			}
		}
	} else {
		finish();
	}
}


MHPositionAnimation::~MHPositionAnimation() {
	// TODO Auto-generated destructor stub
}
