/*
 * MHAlphaAnimation.cpp
 *
 *  Created on: 25.02.2011
 *      Author: Администратор
 */

#include "MHPauseAnimation.h"

MHPauseAnimation::MHPauseAnimation(float duration) {
    this->duration = duration;
}


AnimationType MHPauseAnimation::getAnimationType() {
	return PauseAT;
}


void MHPauseAnimation::update(float dt) {
	MHAnimation::update(dt);
	if (paused || isFinished()) return;
    
    float ddt = (1.0f / Game::instance->osAdapter->getFixedTimeStep()) * dt;
    duration = duration - ddt;
    
    if (duration <= 0.0f) {
        finish();
    }
}


MHPauseAnimation::~MHPauseAnimation() {
	// TODO Auto-generated destructor stub
}
