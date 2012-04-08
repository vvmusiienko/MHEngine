/*
 * GOCollisionShape.cpp
 *
 *  Created on: 04.02.2011
 *      Author: Администратор
 */

#include "GOCollisionShape.h"
#include "GOCompoundCollisionShape.h"
#include "../../Game.h"
#include "../PhysicObject.h"

GOCollisionShape::GOCollisionShape() {
	collisionShape = NULL;
	parentShape = NULL;
	owner = NULL;
	if (Game::instance->editorMode) {
		goID = "ch_" + generateRandomString(10);
		bindLua();
	}
}


void GOCollisionShape::bindLua() {
	Game::instance->luaVM->doString("if "+goID+" == nil then "+goID+" = {} end");
	Game::instance->luaVM->doString(FormatString(goID+".cpointer = %i", this));
	Game::instance->luaVM->registerProperty(goID + ".%sid", MG_STRING, &this->goID);
	Game::instance->luaVM->registerProperty(goID + ".%stype", MG_INT, &this->goType);
	registrateMethods(goID);
}


void GOCollisionShape::setOffset(CVector v) {
	offset = v;
	if (parentShape)
		parentShape->updateChildTransforms();
}


CVector GOCollisionShape::getOffset() {
	return offset;
}


void GOCollisionShape::setOrientation(CVector v) {
	orientation = v;
	if (parentShape)
		parentShape->updateChildTransforms();
}


CVector GOCollisionShape::getOrientation() {
	return orientation;
}

string GOCollisionShape::getGOID() {
	return goID;
}

void GOCollisionShape::save(FILE* f) {
	writeVector(getOffset(), f);
	writeVector(getOrientation(), f);
}


void GOCollisionShape::load(FILE* f) {
	setOffset(readVector(f));
	setOrientation(readVector(f));
}


btCollisionShape* GOCollisionShape::getCollisionShape() {
	return collisionShape;
}


GOCollisionShapeType GOCollisionShape::getCollisionShapeType() {
	return type;
}



int GOCollisionShape::methodsBridge(lua_State* luaVM) {

	if (isCurrentMethod("setOffset")) {
		setOffset(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getOffset")) {
		luaPushVector(luaVM, getOffset().x, getOffset().y, getOffset().z);
		return 1;
	}
	if (isCurrentMethod("setOrientation")) {
		setOrientation(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getOrientation")) {
		luaPushVector(luaVM, getOrientation().x, getOrientation().y, getOrientation().z);
		return 1;
	}
	if (isCurrentMethod("getCollisionShapeType")) {
		lua_pushinteger(luaVM, getCollisionShapeType());
		return 1;
	}

	return LuaBridge::methodsBridge(luaVM);
}


GOCollisionShape::~GOCollisionShape() {
	if (collisionShape) {
		delete collisionShape;
		collisionShape = NULL;
	}
}
