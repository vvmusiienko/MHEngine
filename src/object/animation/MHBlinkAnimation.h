/*
 * MHAlphaAnimation.h
 *
 *  Created on: 25.02.2011
 *      Author: Администратор
 */

#ifndef MHBLINKANIMATION_H_
#define MHBLINKANIMATION_H_

#include "MHAnimation.h"

class MHBlinkAnimation: public MHAnimation {

	float duration;
    float blinkSpeed;
    MGColor oldColor;
    MGColor blinkColor;
    
    bool needBlink;
    float afterBlinkTime;
    
    
public:
	AnimationType getAnimationType();
	MHBlinkAnimation(MGColor blinkColor, float blinkSpeed, float duration);
	void update(float dt);
	void onStart();

	virtual ~MHBlinkAnimation();
};

#endif /* MHALPHAANIMATION_H_ */
