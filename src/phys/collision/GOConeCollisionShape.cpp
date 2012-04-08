/*
 * GOConeCollisionShape.cpp
 *
 *  Created on: 10.02.2011
 *      Author: Администратор
 */

#include "GOConeCollisionShape.h"
#include "../../object/GameObject.h"


GOConeCollisionShape::GOConeCollisionShape(float diameter, float height) {
	this->diameter = diameter;
	this->height = height;
	createShape();
	if (Game::instance->editorMode)
		bindLua();
}


GOConeCollisionShape::GOConeCollisionShape(FILE* f) {
	GOCollisionShape::load(f);
	diameter = readFloat(f);
	height = readFloat(f);
	createShape();
	if (Game::instance->editorMode)
		bindLua();
}


void GOConeCollisionShape::createShape() {
	if (collisionShape)
		delete collisionShape;
	collisionShape = new btConeShape(diameter / 200.0f, height / 100.0f);
	collisionShape->setUserPointer(this);
	goType = GO_CONE_CS;
	type = GOCST_CONE;
}


void GOConeCollisionShape::save(FILE* f) {
	GOCollisionShape::save(f);
	writeFloat(diameter, f);
	writeFloat(height, f);
}


void GOConeCollisionShape::bindLua() {
	registrateMethods(goID);
}


void GOConeCollisionShape::setDiameter(float v) {
	diameter = v;
	createShape();
	if (owner)
		owner->makeCollisionShape();
}


float GOConeCollisionShape::getDiameter() {
	return diameter;
}


void GOConeCollisionShape::setHeight(float v) {
	height = v;
	createShape();
	if (owner)
		owner->makeCollisionShape();
}


float GOConeCollisionShape::getHeight() {
	return height;
}


int GOConeCollisionShape::methodsBridge(lua_State* luaVM) {
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


GOConeCollisionShape::~GOConeCollisionShape() {
	// TODO Auto-generated destructor stub
}
