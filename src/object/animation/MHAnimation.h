/*
 * MHAnimation.h
 *
 *  Created on: 25.02.2011
 *      Author: Администратор
 */

#ifndef MHANIMATION_H_
#define MHANIMATION_H_

#include "../../Common.h"
#include "../GameObject.h"

enum AnimationType {
	PositionAT,
	AlphaAT,
    BlinkAT,
    QueueAT,
    PauseAT
};


class MHAnimation {

private:
	bool finished; // true if execution ended
	bool needFinish;
	bool started; // true after start affecting this animation
	float secondsBeforeStart; // animation begin after some pause;


protected:
	MHAnimation();

	Action* onBeginAction;
	Action* onEndAction;
	string onBeginActionScript;
	string onEndActionScript;

	GameObject* owner;
	bool paused;

public:
	virtual AnimationType getAnimationType() = 0;
	bool isFinished();
	void finish();
	virtual void setOwner(GameObject* owner);
	virtual void onStart();

	void setOnBeginAction(Action* a);
	void setOnEndAction(Action* a);
	void setOnBeginAction(const string& script);
	void setOnEndAction(const string& script);

	void startAfter(float seconds);

	virtual void update(float dt);

	virtual ~MHAnimation();
};

#endif /* MHANIMATION_H_ */
