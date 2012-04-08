/*
 * GOHingeConstraint.cpp
 *
 *  Created on: 14.02.2011
 *      Author: Администратор
 */

#include "GOHingeConstraint.h"
#include "../../Game.h"
#include "../../object/GameObject.h"
#include "../../MapManager.h"


GOHingeConstraint::GOHingeConstraint() : GOConstraint() {
	init();
	if (Game::instance->editorMode)
		bindLua();
}


GOHingeConstraint::GOHingeConstraint(string id) : GOConstraint(id) {
	init();
	bindLua();
}


GOHingeConstraint::GOHingeConstraint(FILE* f) : GOConstraint(f) {
	init();
	axis = readVector(f);
	secondAxis = readVector(f);
	if (hasID || Game::instance->editorMode)
		bindLua();
}

void GOHingeConstraint::init() {
	type = GO_HINGE_CONSTRAINT;
	axis = CVector(0,1,0);
	secondAxis = CVector(0,1,0);
}


void GOHingeConstraint::save(FILE* f) {
	GOConstraint::save(f);
	writeVector(axis, f);
	writeVector(secondAxis, f);
}


void GOHingeConstraint::createConstraint() {
	if (secondObjectID != "") {
		setSecondObject(Game::instance->mapManager->findByID(secondObjectID));
		if (secondObject) {
			secondObjectID = "";
		} else {
			return;
		}
	}
	if (secondObject && !secondObject->isPhysicsInitialized())
		return;

	if (axis.Length() < 0.01f || (secondObject && secondAxis.Length() < 0.01f)) {
		return;
	}

	removeConstraint();

	if (owner == NULL) // Constraint not added to some object
		return;

	if (secondObject) {
		// attach to second body
		constraint = new btHingeConstraint(*owner->getRigidBody(), *secondObject->getRigidBody(), btVector3(pivot.x / 100.f , pivot.y / 100.0f, pivot.z / 100.0f), btVector3(secondPivot.x / 100.f , secondPivot.y / 100.0f, secondPivot.z / 100.0f), btVector3(axis.x, axis.y, axis.z), btVector3(secondAxis.x, secondAxis.y, secondAxis.z));
	} else {
		constraint = new btHingeConstraint(*owner->getRigidBody(), btVector3(pivot.x / 100.f , pivot.y / 100.0f, pivot.z / 100.0f), btVector3(axis.x, axis.y, axis.z));
	}
	constraint->setUserConstraintPtr(this);
	addConstraintToWorld();

	needRecreateConstraint = false;
}

void GOHingeConstraint::setAxis(CVector v) {
	axis = v;
	needRecreateConstraint = true;
}


CVector GOHingeConstraint::getAxis() {
	return axis;
}


void GOHingeConstraint::setSecondAxis(CVector v) {
	secondAxis = v;
	needRecreateConstraint = true;
}


CVector GOHingeConstraint::getSecondAxis() {
	return secondAxis;
}


void GOHingeConstraint::bindLua() {
	registrateMethods(id);
}


int GOHingeConstraint::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("setSecondAxis")) {
		setSecondAxis(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getSecondAxis")) {
		luaPushVector(luaVM, getSecondAxis().x, getSecondAxis().y, getSecondAxis().z);
		return 1;
	}
	if (isCurrentMethod("setAxis")) {
		setAxis(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getAxis")) {
		luaPushVector(luaVM, getAxis().x, getAxis().y, getAxis().z);
		return 1;
	}

	return GOConstraint::methodsBridge(luaVM);
}


GOHingeConstraint::~GOHingeConstraint() {
}
