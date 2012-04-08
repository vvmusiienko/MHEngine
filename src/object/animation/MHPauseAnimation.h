/*
 * MHAlphaAnimation.h
 *
 *  Created on: 25.02.2011
 *      Author: Администратор
 */

#ifndef MHPAUSEANIMATION_H_
#define MHPAUSEANIMATION_H_

#include "MHAnimation.h"

class MHPauseAnimation: public MHAnimation {

	float duration;
    
public:
	AnimationType getAnimationType();
	MHPauseAnimation(float duration);
	void update(float dt);

	virtual ~MHPauseAnimation();
};

#endif /* MHALPHAANIMATION_H_ */
