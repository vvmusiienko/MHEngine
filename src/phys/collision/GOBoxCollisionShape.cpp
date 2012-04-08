/*
 * GOBoxCollisionShape.cpp
 *
 *  Created on: 04.02.2011
 *      Author: Администратор
 */

#include "GOBoxCollisionShape.h"
#include "../../Common.h"
#include "../../object/GameObject.h"


GOBoxCollisionShape::GOBoxCollisionShape(CVector size) {
	this->size = size;
	createShape();
	if (Game::instance->editorMode)
		bindLua();
}


GOBoxCollisionShape::GOBoxCollisionShape(FILE* f) {
	GOCollisionShape::load(f);
	size = readVector(f);
	createShape();
	if (Game::instance->editorMode)
		bindLua();
}


void GOBoxCollisionShape::createShape() {
	if (collisionShape)
		delete collisionShape;
	collisionShape = new btBoxShape(btVector3(size.x / 200.f, size.y / 200.f, size.z / 200.f));
	collisionShape->setUserPointer(this);
	goType = GO_BOX_CS;
	type = GOCST_BOX;
}


void GOBoxCollisionShape::save(FILE* f) {
	GOCollisionShape::save(f);
	writeVector(size, f);
}


void GOBoxCollisionShape::bindLua() {
	registrateMethods(goID);
}


void GOBoxCollisionShape::setSize(CVector v) {
	size = v;
	createShape();
	if (owner)
		owner->makeCollisionShape();
}


CVector GOBoxCollisionShape::getSize() {
	return size;
}


int GOBoxCollisionShape::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("setSize")) {
		setSize(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getSize")) {
		luaPushVector(luaVM, getSize().x, getSize().y, getSize().z);
		return 1;
	}

	return GOCollisionShape::methodsBridge(luaVM);
}




GOBoxCollisionShape::~GOBoxCollisionShape() {
}
