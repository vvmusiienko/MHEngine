/*
 * GOSphereCollisionShape.cpp
 *
 *  Created on: 04.02.2011
 *      Author: Администратор
 */

#include "GOSphereCollisionShape.h"
#include "../../Common.h"
#include "../../object/GameObject.h"


GOSphereCollisionShape::GOSphereCollisionShape(float diameter) {
	this->diameter = diameter;
	createShape();
	if (Game::instance->editorMode)
		bindLua();
}


GOSphereCollisionShape::GOSphereCollisionShape(FILE* f) {
	GOCollisionShape::load(f);
	diameter = readFloat(f);
	createShape();
	if (Game::instance->editorMode)
		bindLua();
}


void GOSphereCollisionShape::createShape() {
	if (collisionShape)
		delete collisionShape;
	collisionShape = new btSphereShape(diameter / 200.0f);
	collisionShape->setUserPointer(this);
	goType = GO_SPHERE_CS;
	type = GOCST_SPHERE;
}


void GOSphereCollisionShape::bindLua() {
	registrateMethods(goID);
}


void GOSphereCollisionShape::save(FILE* f) {
	GOCollisionShape::save(f);
	writeFloat(diameter, f);
}


void GOSphereCollisionShape::setDiameter(float v) {
	diameter = v;
	createShape();
	if (owner)
		owner->makeCollisionShape();

}


float GOSphereCollisionShape::getDiameter() {
	return diameter;
}


int GOSphereCollisionShape::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("setDiameter")) {
		setDiameter(lua_tonumber(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("getDiameter")) {
		lua_pushnumber(luaVM, getDiameter());
		return 1;
	}

	return GOCollisionShape::methodsBridge(luaVM);
}



GOSphereCollisionShape::~GOSphereCollisionShape() {
}
