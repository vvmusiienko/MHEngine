/*
 * GOConstraint.h
 *
 *  Created on: 14.02.2011
 *      Author: Администратор
 */

#ifndef GOCONSTRAINT_H_
#define GOCONSTRAINT_H_

#include <btBulletDynamicsCommon.h>
#include "../../LuaBridge.h"
#include "../../CVector.h"

class PhysicObject;


class GOConstraint: public LuaBridge {
	friend class PhysicObject;

protected:
	GOConstraint();
	GOConstraint(string id);
	GOConstraint(FILE* f);
	btTypedConstraint* constraint;
	PhysicObject* owner;
	string secondObjectID;
	CVector pivot;
	PhysicObject* secondObject;
	CVector secondPivot;
	bool needRecreateConstraint;

	// Lua binding (for editor purposes only)
	bool hasID;
	string id;
	int type;
	void removeConstraint();
	void addConstraintToWorld();

	void init();
	void bindLua();
	virtual int methodsBridge(lua_State* luaVM);

public:
	string getGOID();
	int getConstraintType();
	bool isNeedRecreateConstraint();
	virtual void createConstraint();
	unsigned char getType();

	void setPivot(CVector v);
	CVector getPivot();
	void setSecondObject(PhysicObject* o);
	PhysicObject* getSecondObject();
	void setSecondPivot(CVector v);
	CVector getSecondPivot();

	virtual btTypedConstraint* getConstraint();
	virtual void save(FILE* f);
	virtual ~GOConstraint();
};

#endif /* GOCONSTRAINT_H_ */
