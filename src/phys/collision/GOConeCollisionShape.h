/*
 * GOConeCollisionShape.h
 *
 *  Created on: 10.02.2011
 *      Author: Администратор
 */

#ifndef GOCONECOLLISIONSHAPE_H_
#define GOCONECOLLISIONSHAPE_H_

#include "GOCollisionShape.h"

class GOConeCollisionShape: public GOCollisionShape {

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
	GOConeCollisionShape(float diameter, float height);
	GOConeCollisionShape(FILE* f);
	virtual ~GOConeCollisionShape();
};

#endif /* GOCONECOLLISIONSHAPE_H_ */
