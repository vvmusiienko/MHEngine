/*
 * GOCapsuleCollisionShape.cpp
 *
 *  Created on: 10.02.2011
 *      Author: Администратор
 */

#include "GOCapsuleCollisionShape.h"
#include "../../object/GameObject.h"


GOCapsuleCollisionShape::GOCapsuleCollisionShape(float diameter, float height) {
	this->diameter = diameter;
	this->height = height;
	createShape();
	if (Game::instance->editorMode)
		bindLua();
}


GOCapsuleCollisionShape::GOCapsuleCollisionShape(FILE* f) {
	GOCollisionShape::load(f);
	diameter = readFloat(f);
	height = readFloat(f);
	createShape();
	if (Game::instance->editorMode)
		bindLua();
}


void GOCapsuleCollisionShape::createShape() {
	if (collisionShape)
		delete collisionShape;
	collisionShape = new btCapsuleShape(diameter / 200.0f, height / 100.0f);
	collisionShape->setUserPointer(this);
	goType = GO_CAPSULE_CS;
	type = GOCST_CAPSULE;
}


void GOCapsuleCollisionShape::save(FILE* f) {
	GOCollisionShape::save(f);
	writeFloat(diameter, f);
	writeFloat(height, f);
}


void GOCapsuleCollisionShape::bindLua() {
	registrateMethods(goID);
}


void GOCapsuleCollisionShape::setDiameter(float v) {
	diameter = v;
	createShape();
	if (owner)
		owner->makeCollisionShape();
}


float GOCapsuleCollisionShape::getDiameter() {
	return diameter;
}


void GOCapsuleCollisionShape::setHeight(float v) {
	height = v;
	createShape();
	if (owner)
		owner->makeCollisionShape();
}


float GOCapsuleCollisionShape::getHeight() {
	return height;
}


int GOCapsuleCollisionShape::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("setDiameter")) {
		setDiameter(lua_tonumber(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("getDiameter")) {
		lua_pushnumber(luaVM, getDiameter());
		return 1;
	}
	if (isCurrentMethod("setHeight")) {
		setHeight(lua_tonumber(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("getHeight")) {
		lua_pushnumber(luaVM, getHeight());
		return 1;
	}

	return GOCollisionShape::methodsBridge(luaVM);
}



GOCapsuleCollisionShape::~GOCapsuleCollisionShape() {
	// TODO Auto-generated destructor stub
}
