/*
 * GOCapsuleCollisionShape.h
 *
 *  Created on: 10.02.2011
 *      Author: Администратор
 */

#ifndef GOCAPSULECOLLISIONSHAPE_H_
#define GOCAPSULECOLLISIONSHAPE_H_

#include "GOCollisionShape.h"

class GOCapsuleCollisionShape: public GOCollisionShape {

protected:
	float diameter;
	float height;

	void createShape();
	void bindLua();
	virtual int methodsBridge(lua_State* luaVM);

public:
	void setDiameter(float v);
	float getDiameter();
	void setHeight(float v);
	float getHeight();

	virtual void save(FILE* f);
	GOCapsuleCollisionShape(float diameter, float height);
	GOCapsuleCollisionShape(FILE* f);
	virtual ~GOCapsuleCollisionShape();
};

#endif /* GOCAPSULECOLLISIONSHAPE_H_ */
