/*
 * GOMeshCollisionShape.h
 *
 *  Created on: 18.02.2011
 *      Author: Администратор
 */

#ifndef GOMESHCOLLISIONSHAPE_H_
#define GOMESHCOLLISIONSHAPE_H_

#include "GOCollisionShape.h"

class GOMeshCollisionShape: public GOCollisionShape {

protected:
	btTriangleMesh* triMesh;
	string modelName;

	void init();
	void createShape();
	void bindLua();
	virtual int methodsBridge(lua_State* luaVM);

public:
	void addTriangle(CVector v1, CVector v2, CVector v3);
	void removeAllTriangles();
	void createFromModel(string model);
	string getModelName();

	virtual void save(FILE* f);

	GOMeshCollisionShape();
	GOMeshCollisionShape(FILE* f);
	virtual ~GOMeshCollisionShape();
};

#endif /* GOMESHCOLLISIONSHAPE_H_ */
