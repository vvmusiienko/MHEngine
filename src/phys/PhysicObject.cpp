/*
 * PhysicObject.cpp
 *
 *  Created on: 24.01.2011
 *      Author: Администратор
 */

#include "PhysicObject.h"
#include <btBulletDynamicsCommon.h>
#include "../Game.h"
#include "../object/GameObject.h"
#include "../CVector.h"
#include "./collision/GOBoxCollisionShape.h"
#include "./collision/GOSphereCollisionShape.h"
#include "./collision/GOCylinderCollisionShape.h"
#include "./collision/GOCompoundCollisionShape.h"
#include "./collision/GOUndefinedCollisionShape.h"
#include "./collision/GOCapsuleCollisionShape.h"
#include "./collision/GOConeCollisionShape.h"
#include "./collision/GOMeshCollisionShape.h"
#include "./constraint/GOP2PConstraint.h"
#include "./constraint/GOHingeConstraint.h"
#include "./constraint/GOSliderConstraint.h"


PhysicObject::PhysicObject() {
	//Log::info("ssssssssssssssssssssssssssssssss");
	init();
}


void PhysicObject::init() {
	rigidBody = NULL;
	collisionShape = NULL;
	mass = 1.0f;
	needRefreshCollisionShape = true;
	collisionShapeType = CST_AUTOMATIC;
	physEnabled = true;
	angularFactor = CVector(1.0f,1.0f,1.0f);
	linearFactor = CVector(1.0f,1.0f,1.0f);
	linearVelocity = CVector(.0f,.0f,.0f);;
	isAlreadyOnContactOcurred = false;
	transformMatrix = new float[4*4];
	enableDeactivation = Game::instance->editorMode;
	friction = 0.5f;
	restitution = 0.0f;
	angularVelocity = CVector(0.0f, 0.0f, 0.0f);
	linearDumping = 0.15f;
	angularDumping = 0.15f;
	needOnPhysicTick = false;
	availabilytiOnPhysicTickCheked = false;
	_isTrigger = false;
	_isKinematic = false;
	collisionShapeScale = CVector(1.0f,1.0f,1.0f);
}


void PhysicObject::savePhysProperties(FILE* f) {
	writeChar(isEnabledPhysics(), f);
	if (!isEnabledPhysics())
		return;
	writeFloat(getMass(), f);
	writeVector(getAngularFactor(), f);
	writeVector(getLinearFactor(), f);
	//writeVector(getLinearVelocity(), f);
	writeChar(isTrigger(), f);
	writeChar(getCollisionShapeType(), f);
	writeChar(isEnableDeactivation(), f);
	writeFloat(getFriction(), f);
	writeFloat(getRestitution(), f);
	writeFloat(getLinearDumping(), f);
	writeFloat(getAngularDumping(), f);

	// save custom collision shape
	if (getCollisionShapeType() == CST_CUSTOM) {
		if (getCollisionShape() == NULL)
			Log::error("Can't save custom col. shape. (shape is NULL) id=%s", objectID.c_str());
		writeChar(getCollisionShape()->getCollisionShapeType() , f);
		getCollisionShape()->save(f);
	}

	// save constraints
	writeChar(getConstrains().size(), f);
	for (unsigned int i = 0; i < getConstrains().size(); i++) {
		writeChar(getConstrains().at(i)->getType(), f);
		getConstrains().at(i)->save(f);
	}
}


void PhysicObject::loadPhysProperties(FILE* f) {
	enablePhysics(readChar(f));
	if (!isEnabledPhysics())
		return;
	setMass(readFloat(f));
	setAngularFactor(readVector(f));
	setLinearFactor(readVector(f));
	//writeVector(getLinearVelocity(), f);
	setTrigger(readChar(f));
	setCollisionShapeType((CollisionShapeType)readChar(f));
	setEnableDeactivation(readChar(f));
	setFriction(readFloat(f));
	setRestitution(readFloat(f));
	setLinearDumping(readFloat(f));
	setAngularDumping(readFloat(f));

	// load custom collision shape
	if (getCollisionShapeType() == CST_CUSTOM) {
		switch ((GOCollisionShapeType)readChar(f)) {
			case  GOCST_COMPOUND_SHAPE:
				setCollisionShape(new GOCompoundCollisionShape(f));
				break;
			case  GOCST_BOX:
				setCollisionShape(new GOBoxCollisionShape(f));
				break;
			case  GOCST_SPHERE:
				setCollisionShape(new GOSphereCollisionShape(f));
				break;
			case  GOCST_CYLINDER:
				setCollisionShape(new GOCylinderCollisionShape(f));
				break;
			case  GOCST_UNDEFINED:
				setCollisionShape(new GOUndefinedCollisionShape(f));
				break;
			case  GOCST_CAPSULE:
				setCollisionShape(new GOCapsuleCollisionShape(f));
				break;
			case  GOCST_CONE:
				setCollisionShape(new GOConeCollisionShape(f));
				break;
			case  GOCST_MESH:
				setCollisionShape(new GOMeshCollisionShape(f));
				break;
			default:
				Log::error("PhysicObject::loadPhysProperties(FILE* f): Undefined custom shape type.");
		}
	}

	// load constraints
	int c_count = readChar(f);
	for (int i = 0; i < c_count; i++) {
		GOConstraint* cs;
		unsigned char type = readChar(f);
		switch (type) {
			case  GO_P2P_CONSTRAINT:
				cs = new GOP2PConstraint(f);
				break;
			case  GO_HINGE_CONSTRAINT:
				cs = new GOHingeConstraint(f);
				break;
			case  GO_SLIDER_CONSTRAINT:
				cs = new GOSliderConstraint(f);
				break;
			default:
				Log::error("PhysicObject::loadPhysProperties(FILE* f): Undefined constraint type.");
		}
		addConstraint(cs);
	}
}


void PhysicObject::onPhysicTick(float dt) {
	if (needOnPhysicTick) {
		if (!availabilytiOnPhysicTickCheked) {
			Game::instance->luaVM->doString(FormatString("if %s.onPhysicTick == nil then %s.s_needOnPhysicTick(false) end", objectID.c_str(), objectID.c_str()));
			availabilytiOnPhysicTickCheked = true;
		}
		Game::instance->luaVM->doString(FormatString("se(%s.onPhysicTick, %s, %f)", objectID.c_str(), objectID.c_str(), dt));
	}
}


BBox PhysicObject::getPhysBBox() {
	if (!isPhysicsInitialized())
		return BBox();

	btVector3 min;
	btVector3 max;
	rigidBody->getAabb(min, max);

	return BBox(CVector(min.x() * 100.0f, max.y() * 100.0f, min.z() * 100.0f),
			CVector(max.x() * 100.0f, min.y() * 100.0f, max.z() * 100.0f));
}


void PhysicObject::setCollisionShapeScale(CVector v) {
	collisionShapeScale = v;
	if (isPhysicsInitialized() && collisionShape) {
		collisionShape->getCollisionShape()->setLocalScaling(btVector3(collisionShapeScale.x, collisionShapeScale.y, collisionShapeScale.z));
	}
}


CVector PhysicObject::getCollisionShapeScale() {
	return collisionShapeScale;
}


void PhysicObject::applyImpulse(CVector impulse, CVector offset) {
	if (isPhysicsInitialized()) {
		rigidBody->applyImpulse(btVector3(impulse.x, impulse.y, impulse.z), btVector3(offset.x, offset.y, offset.z));
	}
}


void PhysicObject::applyForce(CVector force, CVector offset) {
	if (isPhysicsInitialized()) {
		rigidBody->applyForce(btVector3(force.x, force.y, force.z), btVector3(offset.x, offset.y, offset.z));
	}
}


void PhysicObject::setCollisionShapeType(CollisionShapeType collisionShapeType) {
	this->collisionShapeType = collisionShapeType;
	setNeedRefreshCollisionShape();
}


CollisionShapeType PhysicObject::getCollisionShapeType() {
	return collisionShapeType;
}


void PhysicObject::setNeedRefreshCollisionShape() {
	needRefreshCollisionShape = true;
}


bool PhysicObject::isNeedRefreshCollisionShape() {
	return needRefreshCollisionShape;
}


void PhysicObject::setCollisionShape(GOCollisionShape* shape) {
	if (collisionShape)
		delete collisionShape;
	if (shape == NULL) {
		shape = new GOUndefinedCollisionShape();
	}
	shape->owner = this;
	collisionShape = shape;
	collisionShape->getCollisionShape()->setLocalScaling(btVector3(collisionShapeScale.x, collisionShapeScale.y, collisionShapeScale.z));
	if (rigidBody)
			rigidBody->setCollisionShape(collisionShape->getCollisionShape());

	refreshInertia();
}


GOCollisionShape* PhysicObject::getCollisionShape() {
	if (collisionShape == NULL) {
		collisionShape = new GOUndefinedCollisionShape();
	}
	return collisionShape;
}

string PhysicObject::getObjectID() {
	return objectID;
}


btRigidBody* PhysicObject::getRigidBody() {
	return rigidBody;
}


// +++++++++++++ CONSTRAINS ++++++++++++++++
void PhysicObject::addConstraint(GOConstraint* c) {
	c->owner = this;
	c->needRecreateConstraint = true;
	constraints.push_back(c);
}


vector<GOConstraint*> PhysicObject::getConstrains() {
	return constraints;
}


void PhysicObject::removeConstraint(GOConstraint* c) {
	// remove from list
	vector<GOConstraint*>::iterator p = constraints.begin();
	for (; p < constraints.end(); p++) {
		if (c == *p) {
			(*p)->owner = NULL;
			(*p)->needRecreateConstraint = true;
			constraints.erase(p);
			break;
		}
	}
}


void PhysicObject::removeAllConstraints() {
	vector<GOConstraint*>::iterator p = constraints.begin();
	for (; p < constraints.end(); p++) {
		(*p)->owner = NULL;
		(*p)->needRecreateConstraint = true;
	}
	constraints.clear();
}



void PhysicObject::secondObjectForConstraint(GOConstraint* c) {
	secondObjectForConstraints.push_back(c);
}


vector<GOConstraint*> PhysicObject::isSecondObjectForConstraints() {
	return secondObjectForConstraints;
}


void PhysicObject::notUseAsSecondObjectForConstraint(GOConstraint* c) {
	// remove from list
	vector<GOConstraint*>::iterator p = secondObjectForConstraints.begin();
	for (; p < secondObjectForConstraints.end(); p++) {
		if (c == *p) {
			(*p)->secondObject = NULL;
			(*p)->needRecreateConstraint = true;
			secondObjectForConstraints.erase(p);
			break;
		}
	}
}


void PhysicObject::makeCollisionShape() {
	// Logic for create box cylinder and sphere with setup parameters


	if (rigidBody) {
		if (collisionShape)
			rigidBody->setCollisionShape(collisionShape->getCollisionShape());
	} else {
		if (collisionShape)
			createPhysObject();
	}
	refreshInertia();
	needRefreshCollisionShape = false;
}


void PhysicObject::makeBoxShape(CVector size) {
	setCollisionShape(new GOBoxCollisionShape(size));
}


void PhysicObject::makeSphereShape(float radius) {
	setCollisionShape(new GOSphereCollisionShape(radius));
}


void PhysicObject::makeCylinderShape(CVector size) {
	setCollisionShape(new GOCylinderCollisionShape(size));
}



void PhysicObject::setPhysPosition(CVector pos) {
	if (rigidBody) {
		btTransform trans;
        if (getIsKinematic()) {
            rigidBody->getMotionState()->getWorldTransform(trans);
            trans.setOrigin(btVector3(pos.x / 100.0f, pos.y / 100.0f, pos.z / 100.0f));
            rigidBody->getMotionState()->setWorldTransform(trans);
        } else {
            trans = rigidBody->getWorldTransform();
            trans.setOrigin(btVector3(pos.x / 100.0f, pos.y / 100.0f, pos.z / 100.0f));
            rigidBody->setWorldTransform(trans);
        }
        
		return;
	}
	initialPosition = pos / 100.0f;
}


CVector PhysicObject::getPhysPosition() {
	if (!rigidBody)
    	return initialPosition;
	btTransform trans;
    if (getIsKinematic()) {
        rigidBody->getMotionState()->getWorldTransform(trans);
    } else {
        trans = rigidBody->getWorldTransform();
    }

	CVector res = CVector(trans.getOrigin().getX() * 100.0f, trans.getOrigin().getY() * 100.0f, trans.getOrigin().getZ() * 100.0f);
    return res;
}


CVector PhysicObject::getPhysOrientation() {
	if (!rigidBody)
		return initialOrientation;

	btQuaternion q = rigidBody->getWorldTransform().getRotation();
	double test = q.x()*q.y() + q.z()*q.w();
	float heading;
	float attitude;
	float bank;

	if (test > 0.499) { // singularity at north pole
		heading = 2 * atan2(q.x(),q.w());
		attitude = PI/2;
		bank = 0;
		return CVector(bank, heading, attitude) * 180/PI;
	}
	if (test < -0.499) { // singularity at south pole
		heading = -2 * atan2(q.x(),q.w());
		attitude = - PI/2;
		bank = 0;
		return CVector(bank, heading, attitude) * 180/PI;
	}
    double sqx = q.x()*q.x();
    double sqy = q.y()*q.y();
    double sqz = q.z()*q.z();
    heading = atan2(2*q.y()*q.w()-2*q.x()*q.z() , 1 - 2*sqy - 2*sqz);
	attitude = asin(2*test);
	bank = atan2(2*q.x()*q.w()-2*q.y()*q.z() , 1 - 2*sqx - 2*sqz);
	return CVector(bank, heading, attitude) * 180/PI;
}


void PhysicObject::setPhysOrientation(CVector v) {
	if (rigidBody) {
		btTransform trans = rigidBody->getWorldTransform();
		btQuaternion q = trans.getRotation();
		q.setEulerZYX((v.z*GRAD_TO_RAD_COEF), (v.y*GRAD_TO_RAD_COEF), (v.x*GRAD_TO_RAD_COEF));
		trans.setRotation(q);
		rigidBody->setWorldTransform(trans);
		return;
	}
	initialOrientation = v;
}


float* PhysicObject::getTransformMatrix() {
	if (rigidBody == NULL)
		return NULL;

    GOCollisionShape* cs = getCollisionShape();

	btTransform trans = rigidBody->getWorldTransform();
	if (cs != NULL) {
		CVector orient = cs->getOrientation();
		if (orient.x != 0.0f || orient.y != 0.0f || orient.z != 0.0f)
			trans.setRotation(trans.getRotation() * btQuaternion((orient.y*GRAD_TO_RAD_COEF), (orient.x*GRAD_TO_RAD_COEF), (orient.z*GRAD_TO_RAD_COEF)));
	}

    trans.getOpenGLMatrix(transformMatrix);
    transformMatrix[12] = transformMatrix[12] * 100.0f;
    transformMatrix[13] = transformMatrix[13] * 100.0f;
    transformMatrix[14] = transformMatrix[14] * 100.0f;

    if (cs != NULL) {
    	transformMatrix[12] = transformMatrix[12] + cs->getOffset().x;
    	transformMatrix[13] = transformMatrix[13] + cs->getOffset().y;
    	transformMatrix[14] = transformMatrix[14] + cs->getOffset().z;
    }

    return transformMatrix;
}


bool PhysicObject::isPhysicsInitialized() {
	return rigidBody != NULL;
}


void PhysicObject::setInitialPosition(CVector v) {
	initialPosition = v / 100.0f;
}


void PhysicObject::setAngularFactor(CVector v) {
	angularFactor = v;
	if (rigidBody) {
		rigidBody->setAngularFactor(btVector3(angularFactor.x, angularFactor.y, angularFactor.z));
	}
}


CVector PhysicObject::getAngularFactor() {
	return angularFactor;
}


void PhysicObject::setLinearFactor(CVector v) {
	linearFactor = v;
	if (rigidBody) {
		rigidBody->setLinearFactor(btVector3(linearFactor.x, linearFactor.y, linearFactor.z));
	}
}


CVector PhysicObject::getLinearFactor() {
	return linearFactor;
}


void PhysicObject::setLinearVelocity(CVector v) {
	linearVelocity = v;
	if (rigidBody) {
		rigidBody->setLinearVelocity(btVector3(linearVelocity.x, linearVelocity.y, linearVelocity.z));
	}
}


CVector PhysicObject::getLinearVelocity() {
	if (rigidBody) {
		btVector3 v = rigidBody->getLinearVelocity();
		return CVector(v.x(), v.y(), v.z());
	}
	return linearVelocity;
}


void PhysicObject::setIsKinematic(bool v) {
	_isKinematic = v;
}


bool PhysicObject::getIsKinematic() {
	return _isKinematic;
}


void PhysicObject::createPhysObject() {
	btDefaultMotionState* fallMotionState =
			new btDefaultMotionState(btTransform(btQuaternion((initialOrientation.y*GRAD_TO_RAD_COEF), (initialOrientation.x*GRAD_TO_RAD_COEF), (initialOrientation.z*GRAD_TO_RAD_COEF)),btVector3(initialPosition.x, initialPosition.y, initialPosition.z)));
	btVector3 fallInertia(0,0,0);
	collisionShape->getCollisionShape()->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, collisionShape->getCollisionShape(), fallInertia);
	rigidBody = new btRigidBody(fallRigidBodyCI);
	setAngularFactor(angularFactor);
	setLinearFactor(linearFactor);
	setLinearVelocity(linearVelocity);
	rigidBody->setFriction(friction);
	rigidBody->setRestitution(restitution);
	setAngularVelocity(angularVelocity);
	rigidBody->setDamping(linearDumping, angularDumping);
	if (_isTrigger) {
		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	} else
	if (_isKinematic) {
		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	}

	rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	rigidBody->setUserPointer(this);

	if (!isEnableDeactivation())
		rigidBody->setActivationState(DISABLE_DEACTIVATION);
	Game::instance->dynamicsWorld->addRigidBody(rigidBody);
}


void PhysicObject::refreshInertia() {
	if (collisionShape == NULL || rigidBody == NULL)
		return;
	btVector3 fallInertia(0,0,0);
	collisionShape->getCollisionShape()->calculateLocalInertia(mass, fallInertia);
	rigidBody->setMassProps(mass, fallInertia);
}


void PhysicObject::removePhysObjectFromSimulation() {
	// remove constraints
	vector<GOConstraint*>::iterator p = constraints.begin();
	for (; p < constraints.end(); p++) {
		delete (*p);
	}
	constraints.clear();
	p = secondObjectForConstraints.begin();
	for (; p < secondObjectForConstraints.end(); p++) {
		(*p)->setSecondObject(NULL);
	}
	secondObjectForConstraints.clear();
	if (rigidBody && rigidBody->isInWorld())
		Game::instance->dynamicsWorld->removeCollisionObject(rigidBody);
	if (rigidBody && rigidBody->getMotionState())
		delete rigidBody->getMotionState();
	if (rigidBody)
		delete rigidBody;
	if (collisionShape)
		delete collisionShape;
	rigidBody = NULL;
}


void PhysicObject::setMass(float v) {
	mass = v;
	refreshInertia();
}


float PhysicObject::getMass() {
	return mass;
}


void PhysicObject::setTrigger(bool v) {
	_isTrigger = v;
	if (rigidBody) {
		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
}


bool PhysicObject::isTrigger() {
	return _isTrigger;
}



void PhysicObject::enablePhysics(bool v) {
/*	if (v) {
		if (rigidBody && !rigidBody->isInWorld())
			rigidBody->setActivationState(DISABLE_SIMULATION);
	} else {
		if (rigidBody && rigidBody->isInWorld())
			rigidBody->activate(true);
	}
*/

	physEnabled = v;
}


bool PhysicObject::isEnabledPhysics() {
	return physEnabled;
}


void PhysicObject::setObjectID(string id) {
	objectID = id;
}

bool PhysicObject::isEnableDeactivation() {
	return enableDeactivation;
}


void PhysicObject::setEnableDeactivation(bool v) {
	enableDeactivation = v;
	if (rigidBody) {
		if (enableDeactivation) {
			rigidBody->setActivationState(WANTS_DEACTIVATION);
		} else {
			rigidBody->setActivationState(DISABLE_DEACTIVATION);
		}
	}
}


float PhysicObject::getFriction() {
	return friction;
}


void PhysicObject::setFriction(float v) {
	friction = v;
	if (rigidBody) {
		rigidBody->setFriction(friction);
	}
}


float PhysicObject::getRestitution() {
	return restitution;
}


void PhysicObject::setRestitution(float v) {
	restitution = v;
	if (rigidBody) {
		rigidBody->setRestitution(restitution);
	}
}


CVector PhysicObject::getAngularVelocity() {
	if (rigidBody) {
		btVector3 av = rigidBody->getAngularVelocity();
		return CVector(av.x(), av.y(), av.z());
	}

	return angularVelocity;
}


void PhysicObject::setAngularVelocity(CVector v) {
	angularVelocity = v;
	if (rigidBody) {
		rigidBody->setAngularVelocity(btVector3(v.x, v.y, v.z));
	}
}


float PhysicObject::getLinearDumping() {
	return linearDumping;
}


void PhysicObject::setLinearDumping(float v) {
	linearDumping = v;
	if (rigidBody)
		rigidBody->setDamping(linearDumping, angularDumping);
}


float PhysicObject::getAngularDumping() {
	return angularDumping;
}


void PhysicObject::setAngularDumping(float v) {
	angularDumping = v;
	if (rigidBody)
		rigidBody->setDamping(linearDumping, angularDumping);
}


void PhysicObject::bindLua() {
	needOnPhysicTick = true;
	Game::instance->luaVM->registerProperty(objectID + ".%sneedOnPhysicTick", MG_BOOL, &this->needOnPhysicTick);
	registrateMethods(objectID);
}



int PhysicObject::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("applyImpulse")) {
		applyImpulse(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)), CVector(lua_tonumber(luaVM, 4), lua_tonumber(luaVM, 5), lua_tonumber(luaVM, 6)));
		return 0;
	}
	if (isCurrentMethod("applyForce")) {
		applyForce(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)), CVector(lua_tonumber(luaVM, 4), lua_tonumber(luaVM, 5), lua_tonumber(luaVM, 6)));
		return 0;
	}
	if (isCurrentMethod("setLinearVelocity")) {
		setLinearVelocity(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getLinearVelocity")) {
		luaPushVector(luaVM, getLinearVelocity().x, getLinearVelocity().y, getLinearVelocity().z);
		return 1;
	}
	if (isCurrentMethod("setMass")) {
		setMass(lua_tonumber(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("getMass")) {
		lua_pushnumber(luaVM, getMass());
		return 1;
	}
	if (isCurrentMethod("setCollisionShapeType")) {
		setCollisionShapeType((CollisionShapeType)lua_tointeger(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("getCollisionShapeType")) {
		lua_pushinteger(luaVM, getCollisionShapeType());
		return 1;
	}
	if (isCurrentMethod("enablePhysics")) {
		enablePhysics(lua_toboolean(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("isEnabledPhysics")) {
		lua_pushboolean(luaVM, isEnabledPhysics());
		return 1;
	}
	if (isCurrentMethod("setAngularFactor")) {
		setAngularFactor(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getAngularFactor")) {
		luaPushVector(luaVM, getAngularFactor().x, getAngularFactor().y, getAngularFactor().z);
		return 1;
	}
	if (isCurrentMethod("setLinearFactor")) {
		setLinearFactor(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getLinearFactor")) {
		luaPushVector(luaVM, getLinearFactor().x, getLinearFactor().y, getLinearFactor().z);
		return 1;
	}
	if (isCurrentMethod("setTrigger")) {
		setTrigger(lua_toboolean(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("isTrigger")) {
		lua_pushboolean(luaVM, isTrigger());
		return 1;
	}
	if (isCurrentMethod("getCollisionShape")) {
		if (getCollisionShape() == NULL) {
			lua_pushnil(luaVM);
		} else {
			lua_getglobal(luaVM, getCollisionShape()->getGOID().c_str());
		}
		return 1;
	}
	if (isCurrentMethod("setCollisionShape")) {
		if (lua_isnil(luaVM, 1)) {
			setCollisionShape(NULL);
			return 0;
		}
		lua_pushstring(luaVM, "cpointer");
		lua_gettable(luaVM, -2);
		GOCollisionShape* o = (GOCollisionShape*)lua_tointeger(luaVM, -1);
		setCollisionShape(o);
		lua_pop(luaVM, -1);
		return 0;
	}
	if (isCurrentMethod("setEnableDeactivation")) {
		setEnableDeactivation(lua_toboolean(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("isEnableDeactivation")) {
		lua_pushboolean(luaVM, isEnableDeactivation());
		return 1;
	}
	if (isCurrentMethod("setFriction")) {
		setFriction(lua_tonumber(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("getFriction")) {
		lua_pushnumber(luaVM, getFriction());
		return 1;
	}
	if (isCurrentMethod("setRestitution")) {
		setRestitution(lua_tonumber(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("getRestitution")) {
		lua_pushnumber(luaVM, getRestitution());
		return 1;
	}
	if (isCurrentMethod("setLinearDumping")) {
		setLinearDumping(lua_tonumber(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("getLinearDumping")) {
		lua_pushnumber(luaVM, getLinearDumping());
		return 1;
	}
	if (isCurrentMethod("setAngularDumping")) {
		setAngularDumping(lua_tonumber(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("getAngularDumping")) {
		lua_pushnumber(luaVM, getAngularDumping());
		return 1;
	}
	if (isCurrentMethod("setAngularVelocity")) {
		setAngularVelocity(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getAngularVelocity")) {
		CVector av = getAngularVelocity();
		luaPushVector(luaVM, av.x, av.y, av.z);
		return 1;
	}
	if (isCurrentMethod("addConstraint")) {
		if (lua_isnil(luaVM, 1)) {
			return 0;
		}
		lua_pushstring(luaVM, "cpointer");
		lua_gettable(luaVM, -2);
		GOConstraint* o = (GOConstraint*)lua_tointeger(luaVM, -1);
		addConstraint(o);
		lua_pop(luaVM, -1);
		return 0;
	}
	if (isCurrentMethod("getConstraints")) {
		lua_newtable(luaVM);
		int tableIndex = lua_gettop(luaVM);
		vector<GOConstraint*> objs;
		for (unsigned int i = 0; i < constraints.size(); ++i) {
			if (constraints.at(i)->id == "undefined" || constraints.at(i)->id == "") continue;
			objs.push_back(constraints.at(i));
		}
		for (unsigned int i = 0; i < objs.size(); ++i) {
			lua_pushinteger(luaVM, i+1);
			lua_getglobal(luaVM, objs.at(i)->id.c_str());
			lua_settable (luaVM, tableIndex);
		}
		return 1;
	}
	if (isCurrentMethod("removeConstraint")) {
		if (lua_isnil(luaVM, 1)) {
			return 0;
		}
		lua_pushstring(luaVM, "cpointer");
		lua_gettable(luaVM, -2);
		GOConstraint* o = (GOConstraint*)lua_tointeger(luaVM, -1);
		removeConstraint(o);
		lua_pop(luaVM, -1);
		return 0;
	}
	if (isCurrentMethod("removeAllConstrains")) {
		removeAllConstraints();
		return 0;
	}
	if (isCurrentMethod("secondObjectForConstraint")) {
		if (lua_isnil(luaVM, 1)) {
			return 0;
		}
		lua_pushstring(luaVM, "cpointer");
		lua_gettable(luaVM, -2);
		GOConstraint* o = (GOConstraint*)lua_tointeger(luaVM, -1);
		secondObjectForConstraint(o);
		lua_pop(luaVM, -1);
		return 0;
	}
	if (isCurrentMethod("isSecondObjectForConstraints")) {
		lua_newtable(luaVM);
		int tableIndex = lua_gettop(luaVM);
		vector<GOConstraint*> objs;
		for (unsigned int i = 0; i < secondObjectForConstraints.size(); ++i) {
			if (secondObjectForConstraints.at(i)->id == "undefined" || secondObjectForConstraints.at(i)->id == "") continue;
			objs.push_back(secondObjectForConstraints.at(i));
		}
		for (unsigned int i = 0; i < objs.size(); ++i) {
			lua_pushinteger(luaVM, i+1);
			lua_getglobal(luaVM, objs.at(i)->id.c_str());
			lua_settable (luaVM, tableIndex);
		}
		return 1;
	}
	if (isCurrentMethod("notUseAsSecondObjectForConstraint")) {
		if (lua_isnil(luaVM, 1)) {
			return 0;
		}
		lua_pushstring(luaVM, "cpointer");
		lua_gettable(luaVM, -2);
		GOConstraint* o = (GOConstraint*)lua_tointeger(luaVM, -1);
		notUseAsSecondObjectForConstraint(o);
		lua_pop(luaVM, -1);
		return 0;
	}

	return LuaBridge::methodsBridge(luaVM);
}



PhysicObject::~PhysicObject() {
}
