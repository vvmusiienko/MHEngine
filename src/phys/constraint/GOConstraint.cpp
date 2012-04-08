/*
 * GOConstraint.cpp
 *
 *  Created on: 14.02.2011
 *      Author: Администратор
 */

#include "GOConstraint.h"
#include "../../Game.h"
#include "../PhysicObject.h"
#include "../../MapManager.h"

GOConstraint::GOConstraint() {
	hasID = false;
	init();
}


GOConstraint::GOConstraint(string id) {
	this->id = id;
	hasID = true;
	init();
	bindLua();
}


GOConstraint::GOConstraint(FILE* f) {
	hasID = false;
	id = readString(f);
	if (id != "") {
		hasID = true;
		bindLua();
	}
	init();
	pivot = readVector(f);
	secondObjectID = readString(f);
	secondPivot = readVector(f);
}


void GOConstraint::init() {
	if (Game::instance->editorMode && !hasID) {
		id = "cnst_" + generateRandomString(5);
		bindLua();
	}
	constraint = NULL;
	secondObject = NULL;
	owner = NULL;
	needRecreateConstraint = true;
	Game::instance->constraints.push_back(this);
}


void GOConstraint::bindLua() {
	buildIDPath(id);
	Game::instance->luaVM->doString(FormatString(id + ".cpointer = %i", this));
	Game::instance->luaVM->registerProperty(id + ".%sid", MG_STRING, &this->id);
	Game::instance->luaVM->registerProperty(id + ".%stype", MG_INT, &this->type);

	registrateMethods(id);
}


void GOConstraint::save(FILE* f) {
	if (hasID)
		writeString(id, f);
	else
		writeString("", f);
	writeVector(pivot, f);
	if (secondObject)
		writeString(secondObject->getObjectID(), f);
	else
		writeString("", f);
	writeVector(secondPivot, f);
}


void GOConstraint::createConstraint() {

}


unsigned char GOConstraint::getType() {
	return type;
}


void GOConstraint::setPivot(CVector v) {
	pivot = v;
	needRecreateConstraint = true;
}


CVector GOConstraint::getPivot() {
	return pivot;
}


void GOConstraint::removeConstraint() {
	if (constraint) {
		Game::instance->dynamicsWorld->removeConstraint(constraint);
		delete constraint;
		constraint = NULL;
	}
}


void GOConstraint::addConstraintToWorld() {
	Game::instance->dynamicsWorld->addConstraint(constraint);
}


bool GOConstraint::isNeedRecreateConstraint() {
	return needRecreateConstraint;
}


string GOConstraint::getGOID() {
	return id;
}


int GOConstraint::getConstraintType() {
	return type;
}


void GOConstraint::setSecondObject(PhysicObject* o) {
	if (secondObject != NULL) {
		secondObject->notUseAsSecondObjectForConstraint(this);
	}
	secondObject = o;
	if (secondObject != NULL) {
		secondObject->secondObjectForConstraint(this);
	}
	needRecreateConstraint = true;
}


PhysicObject* GOConstraint::getSecondObject() {
	return secondObject;
}


void GOConstraint::setSecondPivot(CVector v) {
	secondPivot = v;
	needRecreateConstraint = true;
}


CVector GOConstraint::getSecondPivot() {
	return secondPivot;
}


btTypedConstraint* GOConstraint::getConstraint() {
	return constraint;
}


int GOConstraint::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("getFullID")) {
		lua_pushstring(luaVM, id.c_str());
		return 1;
	}
	if (isCurrentMethod("setPivot")) {
		setPivot(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getPivot")) {
		luaPushVector(luaVM, getPivot().x, getPivot().y, getPivot().z);
		return 1;
	}
	if (isCurrentMethod("getConstraintType")) {
		lua_pushinteger(luaVM, getConstraintType());
		return 1;
	}
	if (isCurrentMethod("setSecondObject")) {
		if (lua_isnil(luaVM, 1)) {
			setSecondObject(NULL);
			return 0;
		}
		lua_pushstring(luaVM, "cpointer");
		lua_gettable(luaVM, -2);
		PhysicObject* o = (PhysicObject*)lua_tointeger(luaVM, -1);
		setSecondObject(o);
		lua_pop(luaVM, -1);
		return 0;
	}
	if (isCurrentMethod("getSecondObject")) {
		if (getSecondObject() == NULL || getSecondObject()->getObjectID() == "") {
			lua_pushnil(luaVM);
		}
		lua_getglobal(luaVM, getSecondObject()->getObjectID().c_str());
		return 1;
	}
	if (isCurrentMethod("setSecondPivot")) {
		setSecondPivot(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getSecondPivot")) {
		luaPushVector(luaVM, getSecondPivot().x, getSecondPivot().y, getSecondPivot().z);
		return 1;
	}
	if (isCurrentMethod("setSecondObjectID")) {
		string objid = lua_tostring(luaVM, 1);
		PhysicObject* obj = (PhysicObject*)Game::instance->mapManager->findByID(objid);
		setSecondObject(obj);
		return 0;
	}
	if (isCurrentMethod("getSecondObjectID")) {
		if (getSecondObject() == NULL) {
			lua_pushstring(luaVM, "");
			return 0;
		}
		lua_pushstring(luaVM, getSecondObject()->getObjectID().c_str());
		return 1;
	}


	return LuaBridge::methodsBridge(luaVM);
}


GOConstraint::~GOConstraint() {
	// remove from list
	vector<GOConstraint*>::iterator p = Game::instance->constraints.begin();
	for (; p < Game::instance->constraints.end(); p++) {
		if (this == *p) {
			Game::instance->constraints.erase(p);
			break;
		}
	}
	if (owner)
		owner->removeConstraint(this);
	setSecondObject(NULL);
	removeConstraint();
}
