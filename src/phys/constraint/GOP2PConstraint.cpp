/*
 * GOP2PConstraint.cpp
 *
 *  Created on: 14.02.2011
 *      Author: Администратор
 */

#include "GOP2PConstraint.h"
#include "../../Game.h"
#include "../../object/GameObject.h"
#include "../../MapManager.h"

GOP2PConstraint::GOP2PConstraint() : GOConstraint() {
	init();
}


GOP2PConstraint::GOP2PConstraint(string id) : GOConstraint(id) {
	init();
	bindLua();
}


GOP2PConstraint::GOP2PConstraint(FILE* f) : GOConstraint(f) {
	init();
}


void GOP2PConstraint::init() {
	type = GO_P2P_CONSTRAINT;
}


void GOP2PConstraint::save(FILE* f) {
	GOConstraint::save(f);
}


void GOP2PConstraint::createConstraint() {
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
		constraint = new btPoint2PointConstraint(*owner->getRigidBody(), *secondObject->getRigidBody(), btVector3(pivot.x / 100.f , pivot.y / 100.0f, pivot.z / 100.0f), btVector3(secondPivot.x / 100.f , secondPivot.y / 100.0f, secondPivot.z / 100.0f));
	} else {
		constraint = new btPoint2PointConstraint(*owner->getRigidBody(), btVector3(pivot.x / 100.f , pivot.y / 100.0f, pivot.z / 100.0f));
	}
	constraint->setUserConstraintPtr(this);
	addConstraintToWorld();

	needRecreateConstraint = false;
}

void GOP2PConstraint::bindLua() {
	registrateMethods(id);
}


int GOP2PConstraint::methodsBridge(lua_State* luaVM) {

	return GOConstraint::methodsBridge(luaVM);
}


GOP2PConstraint::~GOP2PConstraint() {
}
