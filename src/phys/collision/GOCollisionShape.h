/*
 * GOCollisionShape.h
 *
 *  Created on: 04.02.2011
 *      Author: Администратор
 */

#ifndef GOCOLLISIONSHAPE_H_
#define GOCOLLISIONSHAPE_H_

#include <btBulletDynamicsCommon.h>
#include "../../CVector.h"
#include "../../LuaBridge.h"


class Game;
class GOCompoundCollisionShape;
class PhysicObject;

enum GOCollisionShapeType {
	GOCST_COMPOUND_SHAPE = 0,
	GOCST_BOX,
	GOCST_SPHERE,
	GOCST_CYLINDER,
	GOCST_UNDEFINED,
	GOCST_CAPSULE,
	GOCST_CONE,
	GOCST_MESH
};


class GOCollisionShape : public LuaBridge {
	friend class GOCompoundCollisionShape;
	friend class PhysicObject;

protected:
	GOCollisionShape();
	GOCollisionShapeType type;
	btCollisionShape* collisionShape;
	CVector offset;
	CVector orientation;
	GOCompoundCollisionShape* parentShape;
	PhysicObject* owner;

	// Lua binding (for editor purposes only)
	string goID;
	int goType;

	void bindLua();
	virtual int methodsBridge(lua_State* luaVM);

public:
	void setOffset(CVector v);
	CVector getOffset();
	void setOrientation(CVector v);
	CVector getOrientation();
	string getGOID();
	GOCollisionShapeType getCollisionShapeType();

	virtual btCollisionShape* getCollisionShape();
	virtual void save(FILE* f);
	virtual void load(FILE* f);


	virtual ~GOCollisionShape();
};

#endif /* GOCOLLISIONSHAPE_H_ */
