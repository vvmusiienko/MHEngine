/*
 * AnimatedTexture.h
 *
 *  Created on: 06.08.2010
 *      Author: Администратор
 */

#ifndef ANIMATEDTEXTURE_H_
#define ANIMATEDTEXTURE_H_

#include "Texture.h"
#include "./object/GameObject.h"

class AnimatedTexture: public Texture, public GameObject {
private:
	bool inProgress;
	float speed;
	float currentFrame;
	int frameCount;
	int repeatCount;
	int currentRepeatNum;
	bool reversed;
	std::vector<Texture*> frames;
	Action* onAnimationAtEndAction;

    // for atlases support 
    float u, v, ul, vl;
    
public:

	void start();
	void setReversed(bool v);
	void setRepeatCount(int v); // 0 - infinity, default 0
	int getRepeatCount();
	void setSpeed(float v);
	float getSpeed();



	void setOnAnimationAtEndAction(Action* action);

    virtual float getU();
    virtual float getV();
    virtual float getUL();
    virtual float getVL();
	void bind ();
	virtual GLenum target () const { return GL_TEXTURE_2D; }
	void update(float dt);

	AnimatedTexture(string name, string type, int frameCount);
	virtual ~AnimatedTexture();
};

#endif /* ANIMATEDTEXTURE_H_ */
