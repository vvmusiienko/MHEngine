/*
 * AbstractModel.h
 *
 *  Created on: 23.12.2010
 *      Author: Администратор
 */

#ifndef AbstractModel_H_
#define AbstractModel_H_

#include "../Texture.h"
#include "../CVector.h"


class AbstractModel {
public:

	virtual bool isStatic() = 0; // return true if this model have not animations
	virtual void setAnimation (const string& animName, bool cycle) = 0;
	virtual void setAnimationImmediate (const string& animName, bool cycle) = 0;
	virtual string getCurrentAnim() = 0;
	virtual string getNextAnim() = 0;
	virtual vector<string> getAllAnimations() = 0;
	virtual CVector getModelSize() = 0;
	virtual void setRepeat(int x, int y, int z) = 0;

	virtual void drawWithTexture(Texture* texture) = 0;
	virtual void animate (float dt) = 0;

	virtual AbstractModel* getCopy() = 0; // return new Instance of AbstractModel which is copy of current object
};

#endif /* MODELDATA_H_ */
