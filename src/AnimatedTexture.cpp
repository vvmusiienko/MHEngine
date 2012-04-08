/*
 * AnimatedTexture.cpp
 *
 *  Created on: 06.08.2010
 *      Author: Администратор
 */

#include "AnimatedTexture.h"
#include "TextureCache.h"

AnimatedTexture::AnimatedTexture(string name, string type, int frameCount) {
	this->frameCount = 0;
	currentFrame = 0;
	speed = 0.1f;
	repeatCount = 0;
	currentRepeatNum = 0;
	reversed = false;
	inProgress = false;
	onAnimationAtEndAction = NULL;

	for (int i = 0; i < frameCount; i++) {
		Texture* frame = TextureCache::getInstance()->load(FormatString("%s%i.%s", name.c_str(), i, type.c_str()));
		if (frame) {
			this->frameCount ++;
			frames.push_back(frame);
		}
	}

	if (this->frameCount > 0) {
		static Texture* frame;
		frame = frames.at((int)currentFrame);
		img = frame->img;
		name = frame->name();
		_handle = frame->handle();
        u = frame->getU();
        v = frame->getV();
        ul = frame->getUL();
        vl = frame->getVL();
	}

	this->addToWorld();
}


void AnimatedTexture::update(float dt) {
	if (!inProgress)
		return;

	if (currentRepeatNum < repeatCount || repeatCount == 0) {

		currentFrame += speed * dt;

		if (currentFrame >= frameCount) {
			currentRepeatNum ++;
			if (currentRepeatNum < repeatCount || repeatCount == 0) {
				currentFrame = 0.0f;
			} else {
				currentFrame = frameCount - 0.9;
			}

			if (onAnimationAtEndAction)
				onAnimationAtEndAction->execute(this);
			if (this->frameCount > 0) {
				static Texture* frame;
				frame = frames.at((int)currentFrame);
				img = frame->img;
				_name = frame->name();
				_handle = frame->handle();
                u = frame->getU();
                v = frame->getV();
                ul = frame->getUL();
                vl = frame->getVL();
			}
		}

	}
}


void AnimatedTexture::bind () {
	if (frameCount > 0) {
		static Texture* frame;
		if (reversed) {
			frame = frames.at((frameCount - 1) - (int)currentFrame);
		} else {
			frame = frames.at((int)currentFrame);
		}
		frame->bind();
		img = frame->img;
		_name = frame->name();
		_handle = frame->handle();
        u = frame->getU();
        v = frame->getV();
        ul = frame->getUL();
        vl = frame->getVL();
	}
}


float AnimatedTexture::getU() {
	return u;
}


float AnimatedTexture::getV() {
	return v;
}


float AnimatedTexture::getUL() {
	return ul;
}


float AnimatedTexture::getVL() {
	return vl;
}


void AnimatedTexture::start() {
	currentRepeatNum = 0;
	currentFrame = 0.0f;
	inProgress = true;
}


void AnimatedTexture::setReversed(bool v) {
	reversed = v;
}


void AnimatedTexture::setRepeatCount(int v) { // 0 - infinity, default 0
	repeatCount = v;
}


int AnimatedTexture::getRepeatCount() {
	return repeatCount;
}


void AnimatedTexture::setSpeed(float v) {
	speed = v;
}


float AnimatedTexture::getSpeed() {
	return speed;
}


void AnimatedTexture::setOnAnimationAtEndAction(Action* action) {
	onAnimationAtEndAction = action;
}


AnimatedTexture::~AnimatedTexture() {
	// TODO Auto-generated destructor stub
}
