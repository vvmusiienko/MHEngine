/*
 * PhysicObject.h
 *
 *  Created on: 24.01.2011
 *      Author: Администратор
 */

#ifndef PHYSICOBJECT_H_
#define PHYSICOBJECT_H_

#include "../LuaBridge.h"
#include "../CVector.h"
#include "../Common.h"

class Game;
class GOCollisionShape;
class GOConstraint;
class btRigidBody;

enum CollisionShapeType {
	CST_AUTOMATIC = 0,
	CST_AUTOMATIC_GRAPHIC_MESH,
	CST_AUTOMATIC_BOX,
	CST_AUTOMATIC_SPHERE,
	CST_AUTOMATIC_CYLINDER,
	CST_CUSTOM,
};


class PhysicObject : public LuaBridge {

private:
	string objectID;
	float* transformMatrix;
	// OnPhysicTick optimization
	bool needOnPhysicTick;
	bool availabilytiOnPhysicTickCheked;

	btRigidBody* rigidBody;
	float mass;
	CVector angularFactor;
	CVector linearFactor;
	CVector linearVelocity;
	bool physEnabled;
	bool _isTrigger;
	bool _isKinematic;

	bool enableDeactivation;
	float friction;
	float restitution;
	CVector angularVelocity;
	float linearDumping;
	float angularDumping;
	CVector collisionShapeScale;

	// Constrains
	vector<GOConstraint*> constraints;
	vector<GOConstraint*> secondObjectForConstraints;


	GOCollisionShape* collisionShape;
	CVector initialPosition;
	CVector initialOrientation;
	CollisionShapeType collisionShapeType;
	bool needRefreshCollisionShape;
	void init();
	void refreshInertia();

protected:

	// Lua binding
	void bindLua();
	virtual int methodsBridge(lua_State* luaVM);
public:
	bool isAlreadyOnContactOcurred;

	// accesors
	void setObjectID(string id);
	string getObjectID();
	btRigidBody* getRigidBody();
	void setMass(float v);
	float getMass();
	void setAngularFactor(CVector v);
	CVector getAngularFactor();
	void setLinearFactor(CVector v);
	CVector getLinearFactor();
	void setLinearVelocity(CVector v);
	CVector getLinearVelocity();
	void setTrigger(bool v);
	bool isTrigger();
	void setIsKinematic(bool v);
	bool getIsKinematic();
	void enablePhysics(bool v);
	bool isEnabledPhysics();
	CVector getPhysPosition();
	void setPhysPosition(CVector pos);
	CVector getPhysOrientation();
	void setPhysOrientation(CVector v);
	float* getTransformMatrix();
	void setInitialPosition(CVector v);
	bool isPhysicsInitialized();
	void setCollisionShape(GOCollisionShape* shape);
	GOCollisionShape* getCollisionShape();
	bool isEnableDeactivation();
	void setEnableDeactivation(bool v);
	float getFriction();
	void setFriction(float v);
	float getRestitution();
	void setRestitution(float v);
	CVector getAngularVelocity();
	void setAngularVelocity(CVector v);
	float getLinearDumping();
	void setLinearDumping(float v);
	float getAngularDumping();
	void setAngularDumping(float v);
	void setCollisionShapeScale(CVector v);
	CVector getCollisionShapeScale();
	BBox getPhysBBox();

	// Serialization
	void savePhysProperties(FILE* f);
	void loadPhysProperties(FILE* f);


	// COLLISION SHAPE
	void setCollisionShapeType(CollisionShapeType collisionShapeType);
	CollisionShapeType getCollisionShapeType();
	void setNeedRefreshCollisionShape(); // need call if size or form of object was changed
	bool isNeedRefreshCollisionShape();
	void makeBoxShape(CVector size);
	void makeSphereShape(float radius);
	void makeCylinderShape(CVector size);
	virtual void makeCollisionShape(); // Make collision shape by actual parameters

	// Events
	virtual void onPhysicTick(float dt);

	// Constrains
	void addConstraint(GOConstraint* c);
	vector<GOConstraint*> getConstrains();
	void removeConstraint(GOConstraint* c);
	void removeAllConstraints();
	void secondObjectForConstraint(GOConstraint* c);
	vector<GOConstraint*> isSecondObjectForConstraints();
	void notUseAsSecondObjectForConstraint(GOConstraint* c);


	// Other
	void applyImpulse(CVector impulse, CVector offset);
	void applyForce(CVector force, CVector offset);
	void createPhysObject();
	void removePhysObjectFromSimulation();

	PhysicObject();
	virtual ~PhysicObject();
};

#endif /* PHYSICOBJECT_H_ */
