/*
 * MHAlphaAnimation.h
 *
 *  Created on: 25.02.2011
 *      Author: Администратор
 */

#ifndef MHALPHAANIMATION_H_
#define MHALPHAANIMATION_H_

#include "MHAnimation.h"

class MHAlphaAnimation: public MHAnimation {

	float duration;
	float alpha;
	float oldAlpha;
	float ddt;

public:
	AnimationType getAnimationType();
	MHAlphaAnimation(float newAlpha, float duration);
	void update(float dt);
	void onStart();

	virtual ~MHAlphaAnimation();
};

#endif /* MHALPHAANIMATION_H_ */
