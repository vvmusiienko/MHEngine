/*
 * GOGround.cpp
 *
 *  Created on: 14.02.2010
 *      Author: Администратор
 */

#include "GOGround.h"
#include "../TextureCache.h"
#include <stdexcept>
#include "../phys/collision/GOCompoundCollisionShape.h"
#include "../phys/collision/GOBoxCollisionShape.h"


GOGround::GOGround() : GameObject() {
	init();
}

GOGround::GOGround(string id) : GameObject(id) {
	init();
	bindLua();
	setMass(0.0f);
}


GOGround::GOGround(FILE* f) : GameObject(f) {
	try {
		init();
		setSize(readVector(f));
		setModelPrefix(readString(f));
		setTexturePrefix(readString(f));
		bindLua();
	} catch (std::runtime_error e) {
		throw std::runtime_error(FormatString("GOGround::GOGround(FILE* f): Can't create object from stream! Reason:\n%s", e.what()));
	}
}


void GOGround::save(FILE* f) {
	GameObject::save(f);
	writeVector(size, f);
	writeString(modelPrefix, f);
	writeString(texturePrefix, f);
}


void GOGround::init()
{
	type = GO_GROUND;
	topAndBottom = new GOPlate();
	//topAndBottom->setDensity(100.0f);
	topAndBottom->setArea(CVector(64, 64), CVector(64, 64));

	base1 = new GOPlate();
	//base1->setDensity(100.0f);
	base2 = new GOPlate();
	//base2->setDensity(100.0f);

	edgeModel = new GOModel();
	centerModel1 = new GOModel();
	centerModel2 = new GOModel();
}

void GOGround::bindLua() {
	game->luaVM->registerProperty(getFullID() + ".%ssize", MG_VECTOR, &this->size);
	game->luaVM->registerProperty(getFullID() + ".%stexturePrefix", MG_STRING, &this->texturePrefix);
	game->luaVM->registerProperty(getFullID() + ".%smodelPrefix", MG_STRING, &this->modelPrefix);
	registrateMethods(getFullID());
}

void GOGround::setModelPrefix(string name)
{
	edgeModel->setModel("ground_" + name + "_edge.md2");
	centerModel1->setModel("ground_" + name + "_center.md2");
	centerModel2->setModel("ground_" + name + "_center.md2");
	modelPrefix = name;
}


void GOGround::setTexturePrefix(string name)
{
	edgeModel->setTexture(name + "_model.tga");
	centerModel1->setTexture(name + "_model.tga");
	centerModel2->setTexture(name + "_model.tga");
	topAndBottom->setTexture(name + "_top.tga");
	base1->setTexture(name + "_base.tga");
	base2->setTexture(name + "_base.tga");
	texturePrefix = name;
}


void GOGround::setSize(CVector v) {
	size = v;
	topAndBottom->setTextureZoomS(0.5 / size.x);
	topAndBottom->setTextureZoomT(0.5 / size.z);
	base1->setTextureZoomS(5 / size.x);
	base1->setTextureZoomT(5 / size.y);
	base2->setTextureZoomS(5 / size.z);
	base2->setTextureZoomT(5 / size.y);
	centerModel1->setRepeat(CVector(size.z, 1, 1));
	centerModel2->setRepeat(CVector(size.x, 1, 1));
}


CVector GOGround::getSize() {
	return size;
}


void GOGround::draw()
{
	glPushMatrix();
	GameObject::draw();

	// -- draw edges
	edgeModel->color = color;
	//+-
	//--
	edgeModel->setOrientation(CVector(0, -90, 0));
	edgeModel->setPosition(CVector(-(size.x * OBJ_MAP_CELL_SIZE) - OBJ_MAP_CELL_SIZE, 0, -(size.z * OBJ_MAP_CELL_SIZE) - OBJ_MAP_CELL_SIZE));
	edgeModel->draw();
	//--
	//+-
	edgeModel->setOrientation(CVector(0, 0, 0));
	edgeModel->setPosition(CVector(-(size.x * OBJ_MAP_CELL_SIZE) - OBJ_MAP_CELL_SIZE, 0, (size.z * OBJ_MAP_CELL_SIZE) + OBJ_MAP_CELL_SIZE));
	edgeModel->draw();
	//--
	//-+
	edgeModel->setOrientation(CVector(0, 90, 0));
	edgeModel->setPosition(CVector((size.x * OBJ_MAP_CELL_SIZE) + OBJ_MAP_CELL_SIZE, 0, (size.z * OBJ_MAP_CELL_SIZE) + OBJ_MAP_CELL_SIZE));
	edgeModel->draw();
	//-+
	//--
	edgeModel->setOrientation(CVector(0, 180, 0));
	edgeModel->setPosition(CVector((size.x * OBJ_MAP_CELL_SIZE) + OBJ_MAP_CELL_SIZE, 0, -(size.z * OBJ_MAP_CELL_SIZE) - OBJ_MAP_CELL_SIZE));
	edgeModel->draw();

	// -- draw centers
	centerModel1->color = color;
	centerModel2->color = color;
	//---
	//+--
	//---
	centerModel1->setOrientation(CVector(0, -90, 0));
	centerModel1->setPosition(CVector(-(size.x * OBJ_MAP_CELL_SIZE) - OBJ_MAP_CELL_SIZE, 0, -size.z * OBJ_MAP_CELL_SIZE + OBJ_MAP_CELL_SIZE));
	centerModel1->draw();
	//---
	//---
	//-+-
	centerModel2->setOrientation(CVector(0, 0, 0));
	centerModel2->setPosition(CVector(-size.x * OBJ_MAP_CELL_SIZE + OBJ_MAP_CELL_SIZE, 0, (size.z * OBJ_MAP_CELL_SIZE) + OBJ_MAP_CELL_SIZE));
	centerModel2->draw();
	//---
	//--+
	//---
	centerModel1->setOrientation(CVector(0, 90, 0));
	centerModel1->setPosition(CVector((size.x * OBJ_MAP_CELL_SIZE) + OBJ_MAP_CELL_SIZE, 0, size.z * OBJ_MAP_CELL_SIZE - OBJ_MAP_CELL_SIZE));
	centerModel1->draw();

	//-+-
	//---
	//---
	centerModel2->setOrientation(CVector(0, -180, 0));
	centerModel2->setPosition(CVector(size.x * OBJ_MAP_CELL_SIZE - OBJ_MAP_CELL_SIZE, 0, -(size.z * OBJ_MAP_CELL_SIZE) - OBJ_MAP_CELL_SIZE));
	centerModel2->draw();

	topAndBottom->color = color;
	// -- draw top and bottom plates
	topAndBottom->setSize(CVector(size.x * OBJ_MAP_CELL_SIZE * 2 + 0.06, size.z * OBJ_MAP_CELL_SIZE * 2 + 0.06, 0));
	topAndBottom->setPosition(CVector(0, 4.5, 0));
	topAndBottom->setOrientation(CVector(-90, 0, 0));
	topAndBottom->draw();

	// -- base
	base1->color = color;

	//---
	//- -
	//-+-
	base1->setSize(CVector(size.x * OBJ_MAP_CELL_SIZE * 2, size.y * OBJ_MAP_CELL_SIZE * 2, 0));
	base1->setPosition(CVector(0, -size.y * OBJ_MAP_CELL_SIZE, size.z * OBJ_MAP_CELL_SIZE));
	base1->setOrientation(CVector(0, 0, 0));
	base1->draw();
	//-+-
	//- -
	//---
	/*base1->setSize(CVector(size.x * OBJ_MAP_CELL_SIZE * 2, size.y * OBJ_MAP_CELL_SIZE * 2, 0));
	base1->setPosition(CVector(0, -size.y * OBJ_MAP_CELL_SIZE, -size.z * OBJ_MAP_CELL_SIZE));
	base1->setOrientation(CVector(0, 180, 0));
	base1->draw();*/

	//---
	//+ -
	//---
	base2->setSize(CVector(size.z * OBJ_MAP_CELL_SIZE * 2, size.y * OBJ_MAP_CELL_SIZE * 2, 0));
	base2->setPosition(CVector(-size.x * OBJ_MAP_CELL_SIZE, -size.y * OBJ_MAP_CELL_SIZE, 0));
	base2->setOrientation(CVector(0, -90, 0));
	base2->draw();
	//---
	//- +
	//---
	base2->setSize(CVector(size.z * OBJ_MAP_CELL_SIZE * 2, size.y * OBJ_MAP_CELL_SIZE * 2, 0));
	base2->setPosition(CVector(size.x * OBJ_MAP_CELL_SIZE, -size.y * OBJ_MAP_CELL_SIZE, 0));
	base2->setOrientation(CVector(0, 90, 0));
	base2->draw();


	/*CVector s = edgeModel->getModelSize();
	Log::info("%f  %f   %f", s.x, s.y, s.z);*/

	glPopMatrix();
}


void GOGround::update(float dt) {
	GameObject::update(dt);
}


BBox GOGround::getBBox() {
	if (isEnabledPhysics()) {
		return getPhysBBox();
	}
	return BBox(CVector(getPosition().x - (size.x * OBJ_MAP_CELL_SIZE + OBJ_MAP_CELL_SIZE * 2), getPosition().y), CVector(getPosition().x + (size.x * OBJ_MAP_CELL_SIZE + OBJ_MAP_CELL_SIZE * 2), getPosition().y - size.y * OBJ_MAP_CELL_SIZE * 2));
}


void GOGround::makeCollisionShape() {
	switch (getCollisionShapeType()) {
	case CST_AUTOMATIC: {
		GOCompoundCollisionShape* s = new GOCompoundCollisionShape();
		GOBoxCollisionShape* b1 = new GOBoxCollisionShape(CVector(size.x * OBJ_MAP_CELL_SIZE * 2 + OBJ_MAP_CELL_SIZE * 4, OBJ_MAP_CELL_SIZE * 4, size.z * OBJ_MAP_CELL_SIZE * 2 + OBJ_MAP_CELL_SIZE * 4));
		b1->setOffset(CVector(0, -(OBJ_MAP_CELL_SIZE * 1), 0));
		s->addChild(b1);
		GOBoxCollisionShape* b2 = new GOBoxCollisionShape(CVector(size.x * OBJ_MAP_CELL_SIZE * 2, size.y * OBJ_MAP_CELL_SIZE * 2, size.z * OBJ_MAP_CELL_SIZE * 2));
		b2->setOffset(CVector(0, -(size.y * OBJ_MAP_CELL_SIZE + OBJ_MAP_CELL_SIZE * 2), 0));
		s->addChild(b2);
		setCollisionShape(s);
		}
		break;
	case CST_AUTOMATIC_BOX:
		makeBoxShape(size * OBJ_MAP_CELL_SIZE * 2);
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


int GOGround::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("setTexturePrefix")) {
		setTexturePrefix(lua_tostring(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("setModelPrefix")) {
		setModelPrefix(lua_tostring(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("setSize")) {
		setSize(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getSize")) {
		luaPushVector(luaVM, getSize().x, getSize().y, getSize().z);
		return 1;
	}
	return GameObject::methodsBridge(luaVM);
}


GOGround::~GOGround() {
	// TODO Auto-generated destructor stub
}
