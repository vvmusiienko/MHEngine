/*
 * MHAlphaAnimation.cpp
 *
 *  Created on: 25.02.2011
 *      Author: Администратор
 */

#include "MHAlphaAnimation.h"

MHAlphaAnimation::MHAlphaAnimation(float newAlpha, float duration) {
	alpha = newAlpha;
	this->duration = duration;

}


AnimationType MHAlphaAnimation::getAnimationType() {
	return AlphaAT;
}


void MHAlphaAnimation::onStart() {
	MHAnimation::onStart();
	oldAlpha = owner->getAlpha();
	ddt = (1.0f / Game::instance->osAdapter->getFixedTimeStep()) / duration;

	if (alpha < oldAlpha) {
		// decreasing
		ddt = ddt * (oldAlpha - alpha);
	} else {
		// increasing
		ddt = ddt * (alpha - oldAlpha);
	}
}


void MHAlphaAnimation::update(float dt) {
	MHAnimation::update(dt);
	if (paused) return;

	if (alpha < oldAlpha) {
		// decreasing
		owner->setAlpha(owner->getAlpha() - ddt * dt );
		if (owner->getAlpha() < alpha) {
			owner->setAlpha(alpha);
			finish();
		}
	} else {
		// increasing
		owner->setAlpha(owner->getAlpha() + ddt * dt );
		if (owner->getAlpha() > alpha) {
			owner->setAlpha(alpha);
			finish();
		}
	}
}


MHAlphaAnimation::~MHAlphaAnimation() {
	// TODO Auto-generated destructor stub
}
