/*
 * MHAnimationQueue.h
 *
 *  Created on: 25.02.2011
 *      Author: Администратор
 */

#ifndef MHANIMATIONQUEUE_H_
#define MHANIMATIONQUEUE_H_

#include "MHAnimation.h"

class MHAnimationQueue: public MHAnimation {

private:
	vector<MHAnimation*> animations;
	MHAnimation* lastStarted;

public:
	AnimationType getAnimationType();
	MHAnimationQueue();
	void addAnimation(MHAnimation* a);

	void update(float dt);
	virtual ~MHAnimationQueue();
};

#endif /* MHANIMATIONQUEUE_H_ */
