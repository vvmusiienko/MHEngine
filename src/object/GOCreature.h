/*
 * GOCreature.h
 *
 *  Created on: 05.04.2010
 *      Author: Администратор
 */

#ifndef GOCREATURE_H_
#define GOCREATURE_H_

#include "GameObject.h"

class GOModel;


// Creature states
#define CS_MOVE_RIGHT		0
#define CS_MOVE_LEFT		1
#define CS_JUMP				2
#define CS_STAND			3


class GOCreature: public GameObject {
// fields
private:
	bool onGround; // if creature stay on ground - true
	GameObject* onGroundObject;
	bool walk; // true if player walking
	float SPEED_1;
	float SPEED_2;
	float AIR_SPEED_1;
	float AIR_SPEED_2;
	float _dt;
	string modelName;

public:
	float movingSpeed; // speed of movement
	CVector jumpPower; // define height of jump
	GOModel *model; // 3d mesh
	int currentState;

protected:
	virtual int methodsBridge(lua_State* luaVM);
	virtual void onContactOccured(GameObject* target, ContactInfo* ci);
	virtual void onContactLost(GameObject* target, ContactInfo* ci);

// methods
private:
	void init();
	void bindLua();
	static int l_methodsBridge(lua_State* luaVM); // Lua
public:
	// LUA
	static void registerInLua(lua_State* luaVM);

	void setState(int state);
	void setModel(string _model);

	void draw();
	void update(float dt);
	void save(FILE* f);
	void makeCollisionShape();
	BBox getBBox();

	GOCreature(string id);
	GOCreature(FILE* f);
	virtual ~GOCreature();
};

#endif /* GOCREATURE_H_ */
