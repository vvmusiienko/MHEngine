/*
 * MapManager.cpp
 *
 *  Created on: 28.01.2010
 *      Author: Администратор
 */

#include "MapManager.h"
#include "Game.h"
#include "object/GameObject.h"
#include "object/GOCube.h"
#include "object/GOPlate.h"
#include "object/GOModel.h"
#include "object/GOLight.h"
#include "object/GOPlayer.h"
#include "object/GOGround.h"
#include "object/GOSkyBox.h"
#include "TextureCache.h"
#include "object/ui/UIButton.h"
#include "object/ui/UILabel.h"
#include "./phys/collision/GOBoxCollisionShape.h"
#include "./phys/collision/GOSphereCollisionShape.h"
#include "./phys/collision/GOCylinderCollisionShape.h"
#include "./phys/collision/GOCompoundCollisionShape.h"
#include "./phys/collision/GOUndefinedCollisionShape.h"
#include "./phys/collision/GOCapsuleCollisionShape.h"
#include "./phys/collision/GOConeCollisionShape.h"
#include "./phys/collision/GOMeshCollisionShape.h"
#include "./phys/constraint/GOP2PConstraint.h"
#include "./phys/constraint/GOHingeConstraint.h"
#include "./phys/constraint/GOSliderConstraint.h"
#include "MapProperties.h"


MapManager::MapManager(Game* game) {
	this->game = game;
	currentMap = "";
	// create map
	objMap = new GameObject**[OBJ_MAP_WIDTH];
	for (int i = 0; i < OBJ_MAP_WIDTH; ++i)
	{
	   objMap[i] = new GameObject*[OBJ_MAP_HEIGHT];
	   memset(objMap[i], 0, sizeof(GameObject*) * OBJ_MAP_HEIGHT);
	}

	mapUnderLoading = false;

	registrateMethods("game");
}


bool MapManager::createObject(FILE* f, int type, GameObject* parent) {
	GameObject::fParentID = parent;
	switch (type) {
		case GO_CUBE:
			Game::instance->mapManager->addObject((new GOCube(f))->objectCreated());
			break;
		case GO_GROUND:
			Game::instance->mapManager->addObject((new GOGround(f))->objectCreated());
			break;
		case GO_PLATE:
			Game::instance->mapManager->addObject((new GOPlate(f))->objectCreated());
			break;
		case GO_MODEL:
			Game::instance->mapManager->addObject((new GOModel(f))->objectCreated());
			break;
		case GO_CREATURE:
			Game::instance->mapManager->addObject((new GOCreature(f))->objectCreated());
			break;
		case GO_LIGHT:
			Game::instance->mapManager->addObject((new GOLight(f))->objectCreated());
			break;
		case GO_PLAYER:
			Game::instance->player = (GOPlayer*)(new GOPlayer(f))->objectCreated();
			Game::instance->mapManager->addObject(Game::instance->player);
			break;
		case GO_UNDEFINED:
			Game::instance->mapManager->addObject((new GameObject(f))->objectCreated());
			break;
	}
	GameObject::fParentID = NULL;
	return true;
}


void MapManager::saveObject(GameObject* o, string path, string script) {
	FILE* f = fopen(path.c_str(), "wb");
	if (!f) {
		Log::warning("Can't open file for saving object %s", path.c_str());
		return;
	}
	// Write count of objects
	int count = /*script*/ 1 + /* one object*/ 1;
	writeInt(count, f);
	fseek(f, sizeof(int), SEEK_CUR);
	// write map prop
	o->save(f);
	// write script
	writeChar(GO_SCRIPT, f);
	writeString(script, f);
	// write to editorOffset end of file offset
	int editorOffset = ftell(f);
	fseek(f, sizeof count, SEEK_SET);
	writeInt(editorOffset, f);
	fclose(f);
	Log::info("MapManager::saveObject done.");
}


bool MapManager::saveMap(string path, string script) {
	FILE* f = fopen(path.c_str(), "wb");
	if (!f) {
		Log::warning("Can't open file for saving map %s", path.c_str());
		return false;
	}

	// Write count of objects
	int count = /*script*/ 1 + /*map props*/ 1;
	for (unsigned int i = 0; i < game->objects.size(); i++) {
		if (game->objects.at(i)->needSaveToMap && game->objects.at(i)->getParent() == NULL)
			count++;
	}
	writeInt(count, f);
	fseek(f, sizeof(int), SEEK_CUR);

	// write map prop
	game->map->save(f);

	for (unsigned int i = 0; i < game->objects.size(); i++) {
		GameObject* go = game->objects.at(i);
		if (go->needSaveToMap && game->objects.at(i)->getParent() == NULL)
			go->save(f);
	}
	// write script
	writeChar(GO_SCRIPT, f);
	writeString(script, f);

	// write to editorOffset end of file offset
	int editorOffset = ftell(f);
	fseek(f, sizeof count, SEEK_SET);
	writeInt(editorOffset, f);

	fclose(f);
	game->luaVM->doString("se(onMapSaved)");
	Log::info("MapManager::saveMap() saved %i objects", count);
	return true;
}


int MapManager::loadObjects(string mapPath, string firstObjectID) {
	FILE* f = fopen(mapPath.c_str(), "rb");
	if (!f) {
		Log::warning("Can't open file with objects %s", mapPath.c_str());
		return false;
	}

	Game::instance->firstObjectID = firstObjectID;
	// Read count of objects
	int count = readInt(f);
	// Pass the editor offset
	fseek(f, sizeof(int), SEEK_CUR);
	// load objects
	for (int i = 0; i < count; i++) {
		// read type
		unsigned char type = readChar(f);
		switch (type) {
			case GO_SCRIPT:
				game->luaVM->doString(readString(f));
				break;
			case GO_MAP_PROPS:
				game->map->load(f);
				break;
			default:
				createObject(f, type, NULL);
				break;
		}
	}

	fclose(f);
	return count;
}


bool MapManager::loadObject(string mapPath, string firstObjectID) {
	mapUnderLoading = true;
	game->needSaveToMap = true;
	int count = loadObjects(mapPath, firstObjectID);
	mapUnderLoading = false;
	game->needSaveToMap = false;
	Log::info("Objects from %s loaded with %i objects.", mapPath.c_str(), count);
	return count >= 1;
}


bool MapManager::loadMap(string path) {
	try {
		unsigned long int startTime = game->osAdapter->getTickCount();

		mapUnderLoading = true;
		game->needSaveToMap = true;
		clearMap();
		game->luaVM->doString("_onMapWillLoad()");

		currentMap = path;

		int count = loadObjects(path, "satana");

		game->luaVM->doString("_onMapLoaded()");
		mapUnderLoading = false;
		game->needSaveToMap = false;
		Log::info("Map %s loaded with %i objects. TIME: %d ms", path.c_str(), count, game->osAdapter->getTickCount() - startTime);
	} catch (std::exception& e) {
		Log::warning("MapManager::loadMap(string name): error load map! Reason:\n %s", e.what());
		return false;
	}

	return true;
}


void MapManager::addObject(GameObject* obj) {
	obj->loadedFromMap = obj->needDeath = mapUnderLoading;
	game->objects.push_back(obj);
	obj->onAddedToWorld();
}


GameObject* MapManager::findByID(string id) {
	for (unsigned int i = 0; i < game->objects.size(); i++) {
		if (game->objects.at(i)->getFullID().compare(id) == 0) {
			return game->objects.at(i);
		}
	}
	return NULL;
}


void MapManager::clearMap() {

	for (unsigned int i = 0; i < game->objects.size(); ++i) {
		if (game->objects.at(i)->needDeath)
			game->objects.at(i)->dead = true;
	}

	game->deleteDeadObjects();
//	game->objects.clear();

	for (int i = 0; i < OBJ_MAP_WIDTH; ++i)
	{
		for (int j = 0; j < OBJ_MAP_HEIGHT; ++j)
		{
			setObjectAt(i, j, NULL);
		}
	}
	game->luaVM->doString("_cleanUp()");
	game->luaVM->restart();
}


void MapManager::restart() {
	if (currentMap != "")
		loadMap(currentMap);
}


GameObject* MapManager::getObjectAt(int x, int y)
{
	if (x < 0 || y < 0 || x > OBJ_MAP_WIDTH - 1 || y > OBJ_MAP_HEIGHT - 1)
		return NULL;
	return objMap[x][y];
}


GameObject* MapManager::getObjectAt(float x, float y)
{
	static int ix;
	static int iy;
	ix = (int)(x / OBJ_MAP_CELL_SIZE);
	iy = (int)(y / OBJ_MAP_CELL_SIZE);
	if (ix < 0 || iy < 0 || ix > OBJ_MAP_WIDTH - 1 || iy > OBJ_MAP_HEIGHT - 1)
		return NULL;
	return objMap[ix][iy];
}


void MapManager::setObjectAt(int x, int y, GameObject* obj)
{
	if (x < 0 || y < 0 || x > OBJ_MAP_WIDTH - 1 || y > OBJ_MAP_HEIGHT - 1)
		return;

	objMap[x][y] = obj;
}


int MapManager::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("saveObject")) {
		string objID = lua_tostring(luaVM, 1);
		string path = lua_tostring(luaVM, 2);
		string script = lua_tostring(luaVM, 3);
		GameObject* o = findByID(objID);
		if (o) {
			saveObject(o, path, script);
		} else {
			Log::warning("Can't find object with id %s", objID.c_str());
		}
		return 0;
	}
	if (isCurrentMethod("loadObjects")) {
		string name = lua_tostring(luaVM, 1);
		string firstObjName = lua_tostring(luaVM, 2);
		string path = game->osAdapter->getResourcePath() + "objects/" + name + ".objs";
		loadObject(path, firstObjName);
		return 0;
	}
	if (isCurrentMethod("addChild")) {
		int type = lua_tointeger(luaVM, 1);
		string id = "";
		if (!lua_isnil(luaVM, 2))
			id = lua_tostring(luaVM, 2);
		GameObject* obj = NULL;

		switch (type) {
			case GO_GROUND:
				obj = new GOGround(id);
				break;
			case GO_CUBE:
				obj = new GOCube(id);
				break;
			case GO_MODEL:
				obj = new GOModel(id);
				break;
			case GO_PLATE:
				obj = new GOPlate(id);
				break;
			case GO_CREATURE:
				obj = new GOCreature(id);
				break;
			case GO_LIGHT:
				obj = new GOLight(id);
				break;
			case GO_PLAYER:
				obj = new GOPlayer(id);
				game->player = (GOPlayer*)obj;
				break;
			case GO_BUTTON:
				obj = new UIButton(id);
				break;
			case GO_LABEL:
				obj = new UILabel(id);
				break;
			// For Collision Objects
			case GO_COMPOUND_CS:
				lua_getglobal(luaVM, (new GOCompoundCollisionShape())->getGOID().c_str());
				return 1;
				break;
			case GO_BOX_CS:
				lua_getglobal(luaVM, (new GOBoxCollisionShape(CVector(50,50,50)))->getGOID().c_str());
				return 1;
				break;
			case GO_SPHERE_CS:
				lua_getglobal(luaVM, (new GOSphereCollisionShape(50))->getGOID().c_str());
				return 1;
				break;
			case GO_CYLINDER_CS:
				lua_getglobal(luaVM, (new GOCylinderCollisionShape(CVector(50,50,50)))->getGOID().c_str());
				return 1;
				break;
			case GO_CAPSULE_CS:
				lua_getglobal(luaVM, (new GOCapsuleCollisionShape(50, 100))->getGOID().c_str());
				return 1;
				break;
			case GO_CONE_CS:
				lua_getglobal(luaVM, (new GOConeCollisionShape(50, 100))->getGOID().c_str());
				return 1;
				break;
			case GO_MESH_CS:
				lua_getglobal(luaVM, (new GOMeshCollisionShape())->getGOID().c_str());
				return 1;
				break;
			// Constraints
			case GO_P2P_CONSTRAINT:
				if (id != "")
					lua_getglobal(luaVM, (new GOP2PConstraint(id))->getGOID().c_str());
				else
					lua_getglobal(luaVM, (new GOP2PConstraint())->getGOID().c_str());
				return 1;
				break;
			case GO_HINGE_CONSTRAINT:
				if (id != "")
					lua_getglobal(luaVM, (new GOHingeConstraint(id))->getGOID().c_str());
				else
					lua_getglobal(luaVM, (new GOHingeConstraint())->getGOID().c_str());
				return 1;
				break;
			case GO_SLIDER_CONSTRAINT:
				if (id != "")
					lua_getglobal(luaVM, (new GOSliderConstraint(id))->getGOID().c_str());
				else
					lua_getglobal(luaVM, (new GOSliderConstraint())->getGOID().c_str());
				return 1;
				break;


			case GO_UNDEFINED:
				obj = new GameObject(id);
				break;
		}

		if (!obj) {
			Log::warning("l_addObject: incorrect object type");
			lua_pushnil(luaVM);
			return 1;
		}
		addObject(obj);
		lua_getglobal(luaVM, id.c_str());
		return 1;
	}
	if (isCurrentMethod("restart")) {
		restart();
		return 0;
	}
	if (isCurrentMethod("findByID")) {
		string id = lua_tostring(luaVM, 1);
		GameObject* go = findByID(id);
		if (go) {
			lua_getglobal(luaVM, id.c_str());
		} else {
			lua_pushnil(luaVM);
		}
		return 1;
	}
	if (isCurrentMethod("saveMap")) {
		string path = lua_tostring(luaVM, 1);
		string script = lua_tostring(luaVM, 2);
		saveMap(path, script);
		return 0;
	}
	if (isCurrentMethod("loadMap")) {
		string name = lua_tostring(luaVM, 1);
		loadMap(game->osAdapter->getResourcePath() + "maps/" + name + ".map");
		return 0;
	}
	if (isCurrentMethod("clearMap")) {
		clearMap();
		return 0;
	}
	if (isCurrentMethod("getObjects")) {
		lua_newtable(luaVM);
		int tableIndex = lua_gettop(luaVM);
		vector<GameObject*> objs;
		for (unsigned int i = 0; i < game->objects.size(); ++i) {
			if (game->objects.at(i)->id == "undefined" || game->objects.at(i)->dead) continue;
			objs.push_back(Game::instance->objects.at(i));
		}
		for (unsigned int i = 0; i < objs.size(); ++i) {
			lua_pushinteger(luaVM, i+1);
			lua_getglobal(luaVM, objs.at(i)->id.c_str());
			lua_settable (luaVM, tableIndex);
		}
		return 1;
	}
	if (isCurrentMethod("getRootObjects")) {
		lua_newtable(luaVM);
		int tableIndex = lua_gettop(luaVM);
		vector<GameObject*> objs;
		for (unsigned int i = 0; i < game->objects.size(); ++i) {
			if (game->objects.at(i)->id == "" || game->objects.at(i)->getParent() != NULL || game->objects.at(i)->dead) continue;
			objs.push_back(Game::instance->objects.at(i));
		}
		for (unsigned int i = 0; i < objs.size(); ++i) {
			lua_pushinteger(luaVM, i+1);
			lua_getglobal(luaVM, objs.at(i)->getFullID().c_str());
			lua_settable (luaVM, tableIndex);
		}
		return 1;
	}
	if (isCurrentMethod("showObjects")) {
		for (unsigned int i = 0; i < game->objects.size(); ++i) {
			GameObject* o = game->objects.at(i);
			Log::info("id:'%s' type:'%i'", o->id.c_str(), o->type);
		}
		return 0;
	}

	return LuaBridge::methodsBridge(luaVM);
}



MapManager::~MapManager()
{
}


