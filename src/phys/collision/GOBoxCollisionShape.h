/*
 * GOBoxCollisionShape.h
 *
 *  Created on: 04.02.2011
 *      Author: Администратор
 */

#ifndef GOBOXCOLLISIONSHAPE_H_
#define GOBOXCOLLISIONSHAPE_H_

#include "GOCollisionShape.h"

class GOBoxCollisionShape: public GOCollisionShape {

protected:
	CVector size;
	void createShape();
	void bindLua();
	virtual int methodsBridge(lua_State* luaVM);

public:

	void setSize(CVector v);
	CVector getSize();

	virtual void save(FILE* f);

	GOBoxCollisionShape(CVector size);
	GOBoxCollisionShape(FILE* f);
	virtual ~GOBoxCollisionShape();
};

#endif /* GOBOXCOLLISIONSHAPE_H_ */
