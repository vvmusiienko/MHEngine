/*
 * GOSliderConstraint.cpp
 *
 *  Created on: 14.02.2011
 *      Author: Администратор
 */

#include "GOSliderConstraint.h"
#include "../../Game.h"
#include "../../object/GameObject.h"
#include "../../MapManager.h"


GOSliderConstraint::GOSliderConstraint() : GOConstraint() {
	init();
	if (Game::instance->editorMode)
		bindLua();
}


GOSliderConstraint::GOSliderConstraint(string id) : GOConstraint(id) {
	init();
	bindLua();
}


GOSliderConstraint::GOSliderConstraint(FILE* f) : GOConstraint(f) {
	init();
	axis = readVector(f);
	secondAxis = readVector(f);
	if (hasID || Game::instance->editorMode)
		bindLua();
}

void GOSliderConstraint::init() {
	type = GO_SLIDER_CONSTRAINT;
	axis = CVector(0,1,0);
	secondAxis = CVector(0,1,0);
}


void GOSliderConstraint::save(FILE* f) {
	GOConstraint::save(f);
	writeVector(axis, f);
	writeVector(secondAxis, f);
}


void GOSliderConstraint::createConstraint() {
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

	removeConstraint();

	if (owner == NULL) // Constraint not added to some object
		return;

	if (secondObject) {
		// attach to second body
		constraint = new btSliderConstraint(*owner->getRigidBody(), *secondObject->getRigidBody(), btTransform(btQuaternion((axis.y * GRAD_TO_RAD_COEF), (axis.x*GRAD_TO_RAD_COEF), (axis.z*GRAD_TO_RAD_COEF)), btVector3(pivot.x / 100.0f, pivot.y / 100.0f, pivot.z / 100.0f)),
				btTransform(btQuaternion((secondAxis.y * GRAD_TO_RAD_COEF), (secondAxis.x*GRAD_TO_RAD_COEF), (secondAxis.z*GRAD_TO_RAD_COEF)), btVector3(secondPivot.x / 100.0f, secondPivot.y / 100.0f, secondPivot.z / 100.0f)), false);
	} else {
		constraint = new btSliderConstraint(*owner->getRigidBody(), btTransform(btQuaternion((axis.y * GRAD_TO_RAD_COEF), (axis.x*GRAD_TO_RAD_COEF), (axis.z*GRAD_TO_RAD_COEF)), btVector3(pivot.x / 100.0f, pivot.y / 100.0f, pivot.z / 100.0f)), false);
	}
	constraint->setUserConstraintPtr(this);
	addConstraintToWorld();

	needRecreateConstraint = false;
}

void GOSliderConstraint::setAxis(CVector v) {
	axis = v;
	needRecreateConstraint = true;
}


CVector GOSliderConstraint::getAxis() {
	return axis;
}


void GOSliderConstraint::setSecondAxis(CVector v) {
	secondAxis = v;
	needRecreateConstraint = true;
}


CVector GOSliderConstraint::getSecondAxis() {
	return secondAxis;
}


void GOSliderConstraint::bindLua() {
	registrateMethods(id);
}


int GOSliderConstraint::methodsBridge(lua_State* luaVM) {
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




GOSliderConstraint::~GOSliderConstraint() {
}
