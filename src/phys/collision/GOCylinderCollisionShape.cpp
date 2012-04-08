/*
 * GOCylinderCollisionShape.cpp
 *
 *  Created on: 04.02.2011
 *      Author: Администратор
 */

#include "GOCylinderCollisionShape.h"
#include "../../Common.h"
#include "../../object/GameObject.h"

GOCylinderCollisionShape::GOCylinderCollisionShape(CVector size) {
	this->size = size;
	createShape();
	if (Game::instance->editorMode)
		bindLua();
}


GOCylinderCollisionShape::GOCylinderCollisionShape(FILE* f) {
	GOCollisionShape::load(f);
	size = readVector(f);
	createShape();
	if (Game::instance->editorMode)
		bindLua();
}


void GOCylinderCollisionShape::createShape() {
	if (collisionShape)
		delete collisionShape;
	collisionShape = new btCylinderShape(btVector3(size.x / 200.f, size.y / 200.f, size.z / 200.f));
	collisionShape->setUserPointer(this);
	goType = GO_CYLINDER_CS;
	type = GOCST_CYLINDER;
}


void GOCylinderCollisionShape::bindLua() {
	registrateMethods(goID);
}


void GOCylinderCollisionShape::save(FILE* f) {
	GOCollisionShape::save(f);
	writeVector(size, f);
}


void GOCylinderCollisionShape::setSize(CVector v) {
	size = v;
	createShape();
	if (owner)
		owner->makeCollisionShape();
}


CVector GOCylinderCollisionShape::getSize() {
	return size;
}


int GOCylinderCollisionShape::methodsBridge(lua_State* luaVM) {
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



GOCylinderCollisionShape::~GOCylinderCollisionShape() {
}
