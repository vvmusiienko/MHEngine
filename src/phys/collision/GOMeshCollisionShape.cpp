/*
 * GOMeshCollisionShape.cpp
 *
 *  Created on: 18.02.2011
 *      Author: Администратор
 */

#include "GOMeshCollisionShape.h"
#include "../../Common.h"
#include "../../object/GameObject.h"
#include "../../model/ModelCache.h"


GOMeshCollisionShape::GOMeshCollisionShape() {
	init();
	createShape();
	if (Game::instance->editorMode)
		bindLua();
}


GOMeshCollisionShape::GOMeshCollisionShape(FILE* f) {
	init();
	GOCollisionShape::load(f);
	modelName = readString(f);
	createShape();
	if (Game::instance->editorMode)
		bindLua();
}


void GOMeshCollisionShape::save(FILE* f) {
	GOCollisionShape::save(f);
	writeString(modelName, f);
}


void GOMeshCollisionShape::init() {
	goType = GO_MESH_CS;
	type = GOCST_MESH;
	triMesh = new btTriangleMesh(true, false);
}


void GOMeshCollisionShape::createShape() {
	if (collisionShape)
		delete collisionShape;

	if (modelName != "") {
		AbstractModelData* m = ModelCache::getInstance()->load(modelName);
		if (m) {
			int triCount = m->getTrianglesCount();
			float* tris = m->getTriangles();
			for (int i = 0; i < triCount; i++) {
/*				triMesh->addTriangle(btVector3(tris[i * 9 + 0]  / 100.0f, tris[i * 9 + 1] / 100.0f, tris[i * 9 + 2] / 100.0f),
						btVector3(tris[i * 9 + 3] / 100.0f, tris[i * 9 + 4] / 100.0f, tris[i * 9 + 5] / 100.0f),
						btVector3(tris[i * 9 + 6] / 100.0f, tris[i * 9 + 7] / 100.0f, tris[i * 9 + 8] / 100.0f));
*/				triMesh->addTriangle(btVector3(tris[i * 9 + 6] / 100.0f, tris[i * 9 + 7] / 100.0f, tris[i * 9 + 8] / 100.0f),
		btVector3(tris[i * 9 + 3] / 100.0f, tris[i * 9 + 4] / 100.0f, tris[i * 9 + 5] / 100.0f),
		btVector3(tris[i * 9 + 0]  / 100.0f, tris[i * 9 + 1] / 100.0f, tris[i * 9 + 2] / 100.0f));
			}
			delete [] tris;
		} else {
			Log::warning("Can't create GOMeshCollisionShape from \"%s\"", modelName.c_str());
		}
	}
	collisionShape = new btConvexTriangleMeshShape(triMesh,true);
	collisionShape->setUserPointer(this);
}


void GOMeshCollisionShape::addTriangle(CVector v1, CVector v2, CVector v3) {
	// normalize offset;
    v1 = v1 * 0.8f;
    v2 = v2 * 0.8f;
    v3 = v3 * 0.8f;
    
    triMesh->addTriangle(btVector3(v1.x,v1.y,v1.z),btVector3(v2.x,v2.y,v2.z),btVector3(v3.x,v3.y,v3.z));
	createShape();
	if (owner)
		owner->makeCollisionShape();
}


void GOMeshCollisionShape::removeAllTriangles() {
	if (triMesh)
		delete triMesh;
	triMesh = new btTriangleMesh(true, false);
	createShape();
	if (owner)
		owner->makeCollisionShape();
}


void GOMeshCollisionShape::createFromModel(string model) {
	modelName = model;
	createShape();
	if (owner)
		owner->makeCollisionShape();
}


string GOMeshCollisionShape::getModelName() {
	return modelName;
}


void GOMeshCollisionShape::bindLua() {
	registrateMethods(goID);
}


int GOMeshCollisionShape::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("addTriangle")) {
		addTriangle(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)), CVector(lua_tonumber(luaVM, 4), lua_tonumber(luaVM, 5), lua_tonumber(luaVM, 6)), CVector(lua_tonumber(luaVM, 7), lua_tonumber(luaVM, 8), lua_tonumber(luaVM, 9)));
		return 0;
	}
	if (isCurrentMethod("removeAllTriangles")) {
		removeAllTriangles();
		return 0;
	}
	if (isCurrentMethod("createFromModel")) {
		createFromModel(lua_tostring(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("getModelName")) {
		lua_pushstring(luaVM, getModelName().c_str());
		return 1;
	}

	return GOCollisionShape::methodsBridge(luaVM);
}

GOMeshCollisionShape::~GOMeshCollisionShape() {

}
