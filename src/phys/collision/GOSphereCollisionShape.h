/*
 * GOSphereCollisionShape.h
 *
 *  Created on: 04.02.2011
 *      Author: Администратор
 */

#ifndef GOSPHERECOLLISIONSHAPE_H_
#define GOSPHERECOLLISIONSHAPE_H_

#include "GOCollisionShape.h"

class GOSphereCollisionShape: public GOCollisionShape {
private:
	float diameter;
	void createShape();
	void bindLua();
	virtual int methodsBridge(lua_State* luaVM);

public:
	void setDiameter(float v);
	float getDiameter();

	virtual void save(FILE* f);

	GOSphereCollisionShape(float diameter);
	GOSphereCollisionShape(FILE* f);
	virtual ~GOSphereCollisionShape();
};

#endif /* GOSPHERECOLLISIONSHAPE_H_ */
