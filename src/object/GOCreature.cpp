/*
 * GOCreature.cpp
 *
 *  Created on: 05.04.2010
 *      Author: Администратор
 */

#include "GOCreature.h"
#include "GOModel.h"
#include "../phys/collision/GOMeshCollisionShape.h"
#include "../phys/collision/GOCapsuleCollisionShape.h"
#include "../GraphicEngine.h"

GOCreature::GOCreature(string id) : GameObject(id) {
	init();
	bindLua();
}


GOCreature::GOCreature(FILE* f) : GameObject(f) {
	try {
		init();
		setModel(readString(f));
		bindLua();
	} catch (std::runtime_error e) {
		throw std::runtime_error(FormatString("GOCreature::GOCreature(FILE* f): Can't create object from stream! Reason:\n%s", e.what()));
	}
}


void GOCreature::save(FILE* f) {
	GameObject::save(f);
	writeString(model->getModelName(), f);
}


void GOCreature::init() {
	type = GO_CREATURE;
	currentState = CS_STAND;
	model = new GOModel();
	model->enablePhysics(false);
	addChild(model);
	//model->setScale(CVector(0.6f, 0.6f, 0.6f));
	model->setFrameRate(0.90);
	model->setPosition(CVector(0, 0, 0));
	jumpPower = CVector(0, 5, 0);
	//resistance = CVector(0.025, 0.0, 0.0);
	walk = false;
	onGround = false;
	movingSpeed = 2;
	onGroundObject = NULL;
}


void GOCreature::bindLua() {
	game->luaVM->registerProperty(getFullID() + ".%smovingSpeed", MG_FLOAT, &movingSpeed);
	game->luaVM->registerProperty(getFullID() + ".%sonGround", MG_BOOL, &onGround);
	game->luaVM->registerProperty(getFullID() + ".%swalk", MG_BOOL, &walk);
	game->luaVM->registerProperty(getFullID() + ".%sjump_power", MG_VECTOR, &jumpPower);

	SPEED_1 = 200.0f;
	SPEED_2 = 1200.0f;
	AIR_SPEED_1 = 100.0f;
	AIR_SPEED_2 = 8.0f;

	game->luaVM->registerProperty(getFullID() + ".%sSPEED_1", MG_FLOAT, &SPEED_1);
	game->luaVM->registerProperty(getFullID() + ".%sSPEED_2", MG_FLOAT, &SPEED_2);
	/*float SPEED_1 = 200.0f;
	float SPEED_2 = 120.0f;
	float AIR_SPEED_1 = 100.0f;
	float AIR_SPEED_2 = 8.0f;*/
	registrateMethods(getFullID());
}


void GOCreature::onContactOccured(GameObject* target, ContactInfo* ci) {
	//Log::info("occured %i %s", currentContacts.size(),target->id.c_str());
	GameObject::onContactOccured(target, ci);
	if (ci->contactPointA.y < -model->getModelSize().y * 0.2f) {
		onGroundObject = target;
		onGround = true;
	}
}


void GOCreature::onContactLost(GameObject* target, ContactInfo* ci) {
	//Log::info("lost %s", target->id.c_str());
	GameObject::onContactLost(target, ci);
	if (target == onGroundObject)
		onGround = false;
}


void GOCreature::setState(int state) {

/*	static const float SPEED_1 = 200.0f;
	static const float SPEED_2 = 120.0f;
	static const float AIR_SPEED_1 = 100.0f;
	static const float AIR_SPEED_2 = 8.0f;
*/
	switch (state) {
		case CS_MOVE_RIGHT:
			currentState = CS_MOVE_RIGHT;
			if (getLinearVelocity().x < 1) {
				setLinearVelocity(getLinearVelocity() + CVector(1,0,0));
			}
			if (onGround)
				model->setAnimationImmediate("run", true);
			walk = true;
			break;
		case CS_MOVE_LEFT:
			currentState = CS_MOVE_LEFT;
				if (getLinearVelocity().x > -1) {
					setLinearVelocity(getLinearVelocity() + CVector(-1,0,0));
				}
			if (onGround)
				model->setAnimationImmediate("run", true);
			walk = true;
			break;
		case CS_JUMP:
			if (onGround) {
				setLinearVelocity(getLinearVelocity() * CVector(1, 0 ,1) + CVector(0, 3,0));
				model->setAnimationImmediate("jumpin", false);
				onGround = false;
			}
			break;
		case CS_STAND:
			//setLinearVelocity(getLinearVelocity() * CVector(0, 1, 1));
			walk = false;
			break;
	}
}


void GOCreature::setModel(string _model) {
	model->setModel(_model);
	model->setTexture(_model + ".tga");
	model->setAnimation("stand", true);
	modelName = _model;
}


void GOCreature::draw() {
	glPushMatrix();
	GameObject::draw();
	glPopMatrix();
	model->color = color;
	model->setScale(getScale());
	model->draw();
}


void GOCreature::update(float dt) {
	GameObject::update(dt);
	_dt = dt;
	game->graphicEngine->writeDebugText("Player onGround: %i", onGround);
	//game->graphicEngine->writeDebugText("Player velocity: %0.2f  %0.2f  %0.2f ", getLinearVelocity().x, getLinearVelocity().y, getLinearVelocity().z);

	//model->position = position;
	model->update(dt);


	if (getLinearVelocity().y < 0 && model->getCurrentAnimation() == "jumpin")
		model->setAnimationImmediate("jumpout", false);

	if (onGround && !walk) {
		model->setAnimationImmediate("stand", false);
	}

	switch (currentState) {
		case CS_MOVE_RIGHT:
			if (model->getOrientation().y < 90) {
				model->setOrientation(model->getOrientation() + CVector(0, 30.0 * dt, 0));
			} else {
				model->setOrientation(CVector(0, 90, 0));
			}
			break;
		case CS_MOVE_LEFT:
			if (model->getOrientation().y > -90) {
				model->setOrientation(model->getOrientation() + CVector(0, -30.0 * dt, 0));
			} else {
				model->setOrientation(CVector(0, -90, 0));
			}
			break;
	}
}


BBox GOCreature::getBBox() {
	return model->getBBox();
}


void GOCreature::makeCollisionShape() {
	CVector size = model->getModelSize();
	switch (getCollisionShapeType()) {
	case CST_AUTOMATIC:
		setCollisionShape(new GOCapsuleCollisionShape(size.x * 1.0f / getScale().y, (size.y - size.x) * 1.0f / getScale().y));
		break;
	case CST_AUTOMATIC_GRAPHIC_MESH:
		if (modelName != "") {
			GOMeshCollisionShape* m = new GOMeshCollisionShape();
			m->createFromModel(modelName);
			setCollisionShape(m);
		}
		break;
	case CST_AUTOMATIC_BOX:
		makeBoxShape(size);
		break;
	case CST_AUTOMATIC_SPHERE: {
		float radius = size.x;
		if (radius < size.y)
			radius = size.y;
		if (radius < size.z)
			radius = size.z;
		makeSphereShape(radius);
		}
		break;
	case CST_AUTOMATIC_CYLINDER: {
		float diameter = sqrt(size.x * size.x + size.z * size.z);
		makeCylinderShape(CVector(diameter, size.y));
		}
		break;
	default:
		break;
	}
	PhysicObject::makeCollisionShape();
}


int GOCreature::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("setModel")) {
		setModel(lua_tostring(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("setState")) {
		setState(lua_tointeger(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("getModel")) {
		lua_pushstring(luaVM,  model->getModelName().c_str());
		return 1;
	}
	return GameObject::methodsBridge(luaVM);
}



GOCreature::~GOCreature() {
	// TODO Auto-generated destructor stub
}
