/*
 * GOUndefinedCollisionShape.cpp
 *
 *  Created on: 07.02.2011
 *      Author: Администратор
 */

#include "GOUndefinedCollisionShape.h"
#include "../../object/GameObject.h"

GOUndefinedCollisionShape::GOUndefinedCollisionShape() {
	createShape();
}


GOUndefinedCollisionShape::GOUndefinedCollisionShape(FILE* f) {
	createShape();
}


void GOUndefinedCollisionShape::save(FILE* f) {
}


void GOUndefinedCollisionShape::createShape() {
	collisionShape = new btCompoundShape();
	collisionShape->setUserPointer(this);
	goType = GO_UNDEFINED_CS;
	type = GOCST_UNDEFINED;
}



GOUndefinedCollisionShape::~GOUndefinedCollisionShape() {
}
