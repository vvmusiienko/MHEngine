/*
 * GOCompoundCollisionShape.h
 *
 *  Created on: 04.02.2011
 *      Author: Администратор
 */

#ifndef GOCOMPOUNDCOLLISIONSHAPE_H_
#define GOCOMPOUNDCOLLISIONSHAPE_H_

#include "GOCollisionShape.h"
#include <vector>

class GOCompoundCollisionShape: public GOCollisionShape {

protected:
	void bindLua();
	void createShape();
	virtual int methodsBridge(lua_State* luaVM);

public:
	void addChild(GOCollisionShape* shape);
	void removeChild(GOCollisionShape* shape);
	vector<GOCollisionShape*> getChilds();

	void updateChildTransforms();

	virtual void save(FILE* f);

	GOCompoundCollisionShape();
	GOCompoundCollisionShape(FILE* f);

	virtual ~GOCompoundCollisionShape();
};

#endif /* GOCOMPOUNDCOLLISIONSHAPE_H_ */
