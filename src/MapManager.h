/*
 * MapManager.h
 *
 *  Created on: 28.01.2010
 *      Author: Администратор
 */

#ifndef MAPMANAGER_H_
#define MAPMANAGER_H_

#include <string>
#include "LuaBridge.h"

class Game;
class GameObject;

class MapManager : LuaBridge {
private:
	string currentMap;
	GameObject*** objMap;
	bool mapUnderLoading; // stays true while map loading

protected:
	virtual int methodsBridge(lua_State* luaVM);

public:

	static bool createObject(FILE* f, int type, GameObject* parent);
	Game* game;
	bool saveMap(string path, string script);
	void saveObject(GameObject* o, string path, string script);
	int loadObjects(string mapPath, string firstObjectID = "");
	bool loadObject(string mapPath, string firstObjectID);
	bool loadMap(string name);
	void addObject(GameObject* obj);
	GameObject* findByID(string id);
	void restart();
	void clearMap();

	// Working with objects MAP
	GameObject* getObjectAt(float x, float y);
	GameObject* getObjectAt(int x, int y);
	void setObjectAt(int x, int y, GameObject* obj);

	MapManager(Game* game);
	virtual ~MapManager();
};


#endif /* MAPMANAGER_H_ */
