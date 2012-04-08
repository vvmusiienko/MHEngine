/*
 * GOCylinderCollisionShape.h
 *
 *  Created on: 04.02.2011
 *      Author: Администратор
 */

#ifndef GOCYLINDERCOLLISIONSHAPE_H_
#define GOCYLINDERCOLLISIONSHAPE_H_

#include "GOCollisionShape.h"

class GOCylinderCollisionShape: public GOCollisionShape {
protected:
	CVector size;
	void createShape();
	void bindLua();
	virtual int methodsBridge(lua_State* luaVM);

public:
	void setSize(CVector v);
	CVector getSize();

	virtual void save(FILE* f);

	GOCylinderCollisionShape(CVector size);
	GOCylinderCollisionShape(FILE* f);
	virtual ~GOCylinderCollisionShape();
};

#endif /* GOCYLINDERCOLLISIONSHAPE_H_ */
