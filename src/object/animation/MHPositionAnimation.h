/*
 * MHPositionAnimation.h
 *
 *  Created on: 25.02.2011
 *      Author: Администратор
 */

#ifndef MHPOSITIONANIMATION_H_
#define MHPOSITIONANIMATION_H_

#include "MHAnimation.h"

class MHPositionAnimation: public MHAnimation {

private:

	// Animation
	float speed;
	float iterSpeed;
	CVector newPos;
	bool isLinear;
	bool movingInProgress;
	float movingStage;
	CVector movingDirection;
	bool stopImediately;
	CVector velocity;
	int iters_count;

	void init();

public:
	AnimationType getAnimationType();
	MHPositionAnimation(CVector endPosition, float duration, bool linear = false);
	void update(float dt);
	void onStart();

	virtual ~MHPositionAnimation();
};

#endif /* MHPOSITIONANIMATION_H_ */
