/*
 * GOModel.cpp
 *
 *  Created on: 16.11.2010
 *      Author: Администратор
 */

#include "GOModel.h"
#include "../TextureCache.h"
#include <stdexcept>
#include "../model/MD2Model.h"
#include "../phys/collision/GOMeshCollisionShape.h"

GOModel::GOModel() : GameObject() {
	init();
}


GOModel::GOModel(string id) : GameObject(id) {
	init();
	bindLua();
}


GOModel::GOModel(FILE* f) : GameObject(f) {
	try {
		init();
		setModel(readString(f));
		texScale = readVector(f);
		setTexture(readString(f));
		setAnimationImmediate(readString(f), true);
		setFrameRate(readFloat(f));
		bindLua();
	} catch (std::exception& e) {
		throw std::runtime_error(FormatString("GOModel::GOModel(FILE* f): Can't create object from stream! Reason:\n%s", e.what()));
	}
}


void GOModel::save(FILE* f) {
	GameObject::save(f);
	writeString(currentModel, f);
	writeVector(texScale, f);
	writeString(currentTexture, f);
	writeString(currentAnim, f);
	writeFloat(frameRate, f);
}


void GOModel::init() {
	type = GO_MODEL;
	frameRate = 1.0f;
	texScale = CVector(1.0f,1.0f,1.0f);
	modelData = NULL;
}


CVector GOModel::getModelSize() {
	if (!modelData)
		return CVector();
	return modelData->getModelSize();
}


void GOModel::setModel (const string& modelName) {
	try {
		currentModel = modelName;
		modelData = new MD2Model(modelName);
		modelSize = modelData->getModelSize();
		modelData->setRepeat((int)repeating.x, (int)repeating.y, (int)repeating.z);
	} catch (std::exception& e) {
		//Log::warning("GOModel::setModel(const string &modelName): Can't load model %s . Reason: %s ", modelName.c_str(), e.what());
	}

}


string GOModel::getModelName () {
	return currentModel;
}


void GOModel::setRepeat(CVector v) {
	repeating = v;
	if (modelData)
		modelData->setRepeat((int)repeating.x, (int)repeating.y, (int)repeating.z);
}


CVector GOModel::getRepeat() {
	return repeating;
}


void GOModel::setTexture (const string& textureName) {
	currentTexture = textureName;
	if (textureName == "") {
		texture = NULL;
		return;
	}
	texture = TextureCache::getInstance()->load(textureName);
}


void GOModel::setTexture (Texture* texture) {
	this->texture = texture;
	if (texture) {
		currentTexture = texture->name();
	} else {
		currentTexture = "";
	}
}


Texture* GOModel::getTexture () {
	return texture;
}


string GOModel::getTextureName () {
	return currentTexture;
}


void GOModel::setAnimation (const string& animName, bool cycle) {
	if (modelData) {
		modelData->setAnimation(animName, cycle);
		currentAnim = animName;
	}
}


void GOModel::setAnimationImmediate (const string &animName, bool cycle){
	if (modelData) {
		modelData->setAnimationImmediate(animName, cycle);
		currentAnim = animName;
	}
}


string GOModel::getCurrentAnimation () {
	return currentAnim;
}


string GOModel::getNextAnimation () {
	if (modelData) {
		return modelData->getNextAnim();
	}
	return "";
}


vector<string> GOModel::getAllAnimations () {
	if (modelData) {
		return modelData->getAllAnimations();
	}
	return vector<string>();
}


void GOModel::setFrameRate (const float frameRate) {
	this->frameRate = frameRate;
}


float GOModel::getFrameRate () {
	return frameRate;
}



void GOModel::draw() {
	glPushMatrix();
	GameObject::draw();

	if (modelData)
		modelData->drawWithTexture(texture);

	glPopMatrix();
}


void GOModel::update(float dt) {
	GameObject::update(dt);
	if (modelData)
		modelData->animate(dt * frameRate * 0.5f);
}


BBox GOModel::getBBox() {
	if (isEnabledPhysics()) {
		return getPhysBBox();
	}
	return BBox(CVector(getAbsolutePosition().x - (modelSize.x / 2.0f), getAbsolutePosition().y + (modelSize.y / 2.0f)), CVector(getAbsolutePosition().x + (modelSize.x / 2.0f), getAbsolutePosition().y - (modelSize.y / 2.0f)));
}


void GOModel::makeCollisionShape() {
	CVector size = getModelSize();
	switch (getCollisionShapeType()) {
	case CST_AUTOMATIC:
	case CST_AUTOMATIC_GRAPHIC_MESH: {
		GOMeshCollisionShape* m = new GOMeshCollisionShape();
		m->createFromModel(currentModel);
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



void GOModel::bindLua() {
	game->luaVM->registerProperty(getFullID() + ".%stexScale", MG_VECTOR, &this->texScale);
	game->luaVM->registerProperty(getFullID() + ".%sframeRate", MG_FLOAT, &this->frameRate);
	game->luaVM->registerProperty(getFullID() + ".%stexture", MG_STRING, &this->currentTexture);
	game->luaVM->registerProperty(getFullID() + ".%smodel", MG_STRING, &this->currentModel);
	game->luaVM->registerProperty(getFullID() + ".%sanim", MG_STRING, &this->currentAnim);
	registrateMethods(getFullID());
}


int GOModel::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("setTexture")) {
		setTexture(lua_tostring(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("setModel")) {
		setModel(lua_tostring(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("setFrameRate")) {
		setFrameRate(lua_tonumber(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("setAnim")) {
		setAnimation(lua_tostring(luaVM, 1), lua_toboolean(luaVM, 2));
		return 0;
	}
	if (isCurrentMethod("setTexScale")) {
		//setTexScale(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getAnimNames")) {
		vector<string> anims = getAllAnimations();
		lua_newtable(luaVM);
		int tableIndex = lua_gettop(luaVM);
		for (unsigned int i = 0; i < anims.size(); i++) {
			lua_pushinteger(luaVM, i + 1);
			lua_pushstring(luaVM, anims.at(i).c_str());
			lua_settable (luaVM, tableIndex);
		}
		return 1;
	}
	if (isCurrentMethod("getCurrentAnim")) {
		lua_pushstring(luaVM, getCurrentAnimation().c_str());
		return 1;
	}

	return GameObject::methodsBridge(luaVM);
}



GOModel::~GOModel() {
	// TODO Auto-generated destructor stub


}
