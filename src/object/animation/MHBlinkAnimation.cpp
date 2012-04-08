/*
 * MHAlphaAnimation.cpp
 *
 *  Created on: 25.02.2011
 *      Author: Администратор
 */

#include "MHBlinkAnimation.h"

MHBlinkAnimation::MHBlinkAnimation(MGColor blinkColor, float blinkSpeed, float duration) {
	this->blinkColor = blinkColor;
    this->blinkSpeed = blinkSpeed;
    this->duration = duration;

    needBlink = true;
    afterBlinkTime = 0.0f;
}


AnimationType MHBlinkAnimation::getAnimationType() {
	return BlinkAT;
}


void MHBlinkAnimation::onStart() {
	MHAnimation::onStart();
    
    oldColor = owner->color;
}


void MHBlinkAnimation::update(float dt) {
	MHAnimation::update(dt);
	if (paused || isFinished()) return;
    
    float ddt = (1.0f / Game::instance->osAdapter->getFixedTimeStep()) * dt;
    afterBlinkTime = afterBlinkTime + ddt;
    duration = duration - ddt;
    
    if (afterBlinkTime >= blinkSpeed) {
        afterBlinkTime = 0.0f;
        if (needBlink) {
            owner->color = blinkColor;
        } else {
            owner->color = oldColor;
        }
        needBlink = !needBlink;
    }
    
    if (duration <= 0.0f) {
        owner->color = oldColor;
        finish();
    }
}


MHBlinkAnimation::~MHBlinkAnimation() {
	// TODO Auto-generated destructor stub
}
