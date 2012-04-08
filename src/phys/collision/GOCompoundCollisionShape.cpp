/*
 * GOCompoundCollisionShape.cpp
 *
 *  Created on: 04.02.2011
 *      Author: Администратор
 */

#include "GOCompoundCollisionShape.h"
#include "../../object/GameObject.h"
#include "./GOBoxCollisionShape.h"
#include "./GOSphereCollisionShape.h"
#include "./GOCylinderCollisionShape.h"
#include "./GOUndefinedCollisionShape.h"
#include "./GOCapsuleCollisionShape.h"
#include "./GOConeCollisionShape.h"
#include "./GOMeshCollisionShape.h"


GOCompoundCollisionShape::GOCompoundCollisionShape() {
	bindLua();
	createShape();
}


GOCompoundCollisionShape::GOCompoundCollisionShape(FILE* f) {
	GOCollisionShape::load(f);
	bindLua();
	createShape();

	// child count
	int count = readInt(f);
	for (int i = 0; i < count; i++) {
		switch ((GOCollisionShapeType)readChar(f)) {
			case  GOCST_COMPOUND_SHAPE:
				addChild(new GOCompoundCollisionShape(f));
				break;
			case  GOCST_BOX:
				addChild(new GOBoxCollisionShape(f));
				break;
			case  GOCST_SPHERE:
				addChild(new GOSphereCollisionShape(f));
				break;
			case  GOCST_CYLINDER:
				addChild(new GOCylinderCollisionShape(f));
				break;
			case  GOCST_UNDEFINED:
				addChild(new GOUndefinedCollisionShape(f));
				break;
			case  GOCST_CAPSULE:
				addChild(new GOCapsuleCollisionShape(f));
				break;
			case  GOCST_CONE:
				addChild(new GOConeCollisionShape(f));
				break;
			default:
				Log::error("GOCompoundCollisionShape::GOCompoundCollisionShape(FILE* f): Undefined custom shape type.");
		}
	}
}


void GOCompoundCollisionShape::bindLua() {
	if (!Game::instance->editorMode)
		return;
	registrateMethods(goID);
}


void GOCompoundCollisionShape::createShape() {
	collisionShape = new btCompoundShape();
	collisionShape->setUserPointer(this);
	goType = GO_COMPOUND_CS;
	type = GOCST_COMPOUND_SHAPE;
}


void GOCompoundCollisionShape::save(FILE* f) {
	GOCollisionShape::save(f);
	vector<GOCollisionShape*> childs = getChilds();

	// count
	writeInt(childs.size(), f);
	for (unsigned int i = 0; i < childs.size(); i++) {
		GOCollisionShape* child = childs.at(i);
		writeChar(child->getCollisionShapeType() , f);
		child->save(f);
	}
}


void GOCompoundCollisionShape::addChild(GOCollisionShape* shape) {
	btTransform trans;
	btQuaternion q;
	trans.setOrigin(btVector3(shape->getOffset().x / 100.0f, shape->getOffset().y / 100.0f, shape->getOffset().z / 100.0f));
	q.setEulerZYX((shape->getOrientation().z*3.141592/180), (shape->getOrientation().y*3.141592/180), (shape->getOrientation().x*3.141592/180));
	trans.setRotation(q);
	((btCompoundShape*)collisionShape)->addChildShape(trans, shape->getCollisionShape());
	shape->parentShape = this;
}


void GOCompoundCollisionShape::removeChild(GOCollisionShape* shape) {
	shape->parentShape = NULL;
	((btCompoundShape*)collisionShape)->removeChildShape(shape->getCollisionShape());
}


void GOCompoundCollisionShape::updateChildTransforms() {
	for (int i = 0; i < ((btCompoundShape*)collisionShape)->getNumChildShapes(); i++) {
		GOCollisionShape* shape = (GOCollisionShape*)((btCompoundShape*)collisionShape)->getChildShape(i)->getUserPointer();
		btTransform trans;
		btQuaternion q;
		trans.setOrigin(btVector3(shape->getOffset().x / 100.0f, shape->getOffset().y / 100.0f, shape->getOffset().z / 100.0f));
		q.setEulerZYX((shape->getOrientation().z*3.141592/180), (shape->getOrientation().y*3.141592/180), (shape->getOrientation().x*3.141592/180));
		trans.setRotation(q);
		((btCompoundShape*)collisionShape)->updateChildTransform(i, trans);
	}
}


vector<GOCollisionShape*> GOCompoundCollisionShape::getChilds() {
	vector<GOCollisionShape*> childs;
	for (int i = 0; i < ((btCompoundShape*)collisionShape)->getNumChildShapes(); i++) {
		GOCollisionShape* shape = (GOCollisionShape*)((btCompoundShape*)collisionShape)->getChildShape(i)->getUserPointer();
		childs.push_back(shape);
	}
	return childs;
}



int GOCompoundCollisionShape::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("getChilds")) {
		lua_newtable(luaVM);
		int tableIndex = lua_gettop(luaVM);
		vector<GOCollisionShape*> objs = getChilds();
		for (unsigned int i = 0; i < objs.size(); ++i) {
			lua_pushinteger(luaVM, i+1);
			lua_getglobal(luaVM, objs.at(i)->getGOID().c_str());
			lua_settable (luaVM, tableIndex);
		}
		return 1;
	}
	if (isCurrentMethod("addChild")) {
		if (lua_isnil(luaVM, 1)) {
			return 0;
		}
		lua_pushstring(luaVM, "cpointer");
		lua_gettable(luaVM, -2);
		GOCollisionShape* o = (GOCollisionShape*)lua_tointeger(luaVM, -1);
		addChild(o);
		lua_pop(luaVM, -1);
		return 0;
	}
	if (isCurrentMethod("removeChild")) {
		if (lua_isnil(luaVM, 1)) {
			return 0;
		}
		lua_pushstring(luaVM, "cpointer");
		lua_gettable(luaVM, -2);
		GOCollisionShape* o = (GOCollisionShape*)lua_tointeger(luaVM, -1);
		removeChild(o);
		lua_pop(luaVM, -1);
		return 0;
	}

	return GOCollisionShape::methodsBridge(luaVM);
}

GOCompoundCollisionShape::~GOCompoundCollisionShape() {
	// TODO Auto-generated destructor stub
}
