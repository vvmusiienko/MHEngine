#include "MGConfig.h"
#include "Log.h"
#include "Game.h"


MGConfig::MGConfig(Game* game)
{
	this->game = game;
	loadConfig();
	// Lua
	registrateMethods("config");
}


void MGConfig::clearConfig()
{
	items.clear();
}


bool MGConfig::isExist(const string key)
{
	if (getItem(key))
		return true;
	return false;
}


int MGConfig::getInt(const string key)
{
	ConfigItem* item = getItem(key);
	if (!item)
		Log::error("Not found item in config with key [%s]", key.c_str());
	if (item->type != CI_INT)
		Log::error("Item in config not int [%s]", key.c_str());
	return item->intValue;
}


float MGConfig::getFloat(const string key)
{
	ConfigItem* item = getItem(key);
	if (!item)
		Log::error("Not found item in config with key [%s]", key.c_str());
	if (item->type != CI_FLOAT)
		Log::error("Item in config not float [%s]", key.c_str());
	return item->floatValue;
}


string MGConfig::getString(const string key)
{
	ConfigItem* item = getItem(key);
	if (!item)
		Log::error("Not found item in config with key [%s]", key.c_str());
	if (item->type != CI_STRING)
		Log::error("Item in config not string [%s]", key.c_str());
	return item->stringValue;
}


CVector MGConfig::getVector(const string key)
{
	ConfigItem* item = getItem(key);
	if (!item)
		Log::error("Not found item in config with key [%s]", key.c_str());
	if (item->type != CI_VECTOR)
		Log::error("Item in config not vector [%s]", key.c_str());
	return item->vectorValue;
}


void MGConfig::setInt(const string key, int value)
{
	ConfigItem* existItem = getItem(key);
	if (existItem) {
		if (existItem->type != CI_INT)
			Log::error("Existed item in config not int [%s]", key.c_str());
		existItem->intValue = value;
	} else {
		ConfigItem* newItem = new ConfigItem;
		newItem->type = CI_INT;
		newItem->key = key;
		newItem->intValue = value;
		items.push_back(newItem);
	}
}


void MGConfig::setFloat(const string key, float value)
{
	ConfigItem* existItem = getItem(key);
	if (existItem) {
		if (existItem->type != CI_FLOAT)
			Log::error("Existed item in config not float [%s]", key.c_str());
		existItem->floatValue = value;
	} else {
		ConfigItem* newItem = new ConfigItem;
		newItem->type = CI_FLOAT;
		newItem->key = key;
		newItem->floatValue = value;
		items.push_back(newItem);
	}
}


void MGConfig::setString(const string key, string value)
{
	ConfigItem* existItem = getItem(key);
	if (existItem) {
		if (existItem->type != CI_STRING)
			Log::error("Existed item in config not string [%s]", key.c_str());
		existItem->stringValue = value;
	} else {
		ConfigItem* newItem = new ConfigItem;
		newItem->type = CI_STRING;
		newItem->key = key;
		newItem->stringValue = value;
		items.push_back(newItem);
	}
}


void MGConfig::setVector(const string key, CVector value)
{
	ConfigItem* existItem = getItem(key);
	if (existItem) {
		if (existItem->type != CI_VECTOR)
			Log::error("Existed item in config not vector [%s]", key.c_str());
		existItem->vectorValue = value;
	} else {
		ConfigItem* newItem = new ConfigItem;
		newItem->type = CI_VECTOR;
		newItem->key = key;
		newItem->vectorValue = value;
		items.push_back(newItem);
	}
}


ConfigItem* MGConfig::getItem(const string key)
{
	for (unsigned int i = 0; i < items.size(); i++) {
		if (items.at(i)->key.compare(key) == 0) {
			return items.at(i);
		}
	}
	return NULL;
}



void MGConfig::saveConfig()
{
	FILE* f = fopen((game->osAdapter->getWorkingPath() + CONFIG_FILE_NAME).c_str(), "w+b");
	// Write count of objects
	int count = items.size();
	writeInt(count,f);
	for (int i = 0; i < count; i++) {
		ConfigItem* item = items.at(i);

		fwrite(&item->type, sizeof item->type, 1, f); // Type; use raw fwrite because I dont now type of enum

		writeString(item->key, f);

		switch (item->type) {
			case CI_INT:
				writeInt(item->intValue, f);
				break;
			case CI_FLOAT:
				writeFloat(item->floatValue, f);
				break;
			case CI_STRING:
				writeString(item->stringValue, f);
				break;
			case CI_VECTOR:
				writeVector(item->vectorValue, f);
				break;
			default:
				Log::error("MGConfig::saveConfig() incorrect type of item [%s]", item->key.c_str());
				break;
		}
	}
	fclose(f);
	Log::info("MGConfig::saveConfig() saved %i objects", count);
}


void MGConfig::loadConfig()
{
	try {
		FILE* f = fopen((game->osAdapter->getWorkingPath() + CONFIG_FILE_NAME).c_str(), "r+b");

		if (!f) {
			Log::warning("MGConfig::loadConfig() can't open storage file");
			return;
		}

		// Read count of objects
		int count = readInt(f);
		for (int i = 0; i < count; i++) {
			ConfigItem* item = new ConfigItem;

			fread(&item->type, sizeof item->type, 1, f); // read Type
			item->key = readString(f);

			switch (item->type) {
				case CI_INT:
					item->intValue = readInt(f);
					break;
				case CI_FLOAT:
					item->floatValue = readFloat(f);
					break;
				case CI_STRING:
					item->stringValue = readString(f);
					break;
				case CI_VECTOR:
					item->vectorValue = readVector(f);
					break;
				default:
					Log::error("MGConfig::loadConfig() incorrect type of item #%i", i);
					break;
			}
			items.push_back(item);
		}
		fclose(f);
		Log::info("MGConfig::loadConfig() loaded %i objects", count);

	} catch (...) {
		Log::error("can't do MGConfig::loadConfig");
	}

}


int MGConfig::methodsBridge(lua_State* luaVM) {

	if (isCurrentMethod("getInt")) {
		lua_pushinteger(luaVM, getInt(lua_tostring(luaVM, -1)));
		return 1;
	}
	if (isCurrentMethod("getFloat")) {
		lua_pushnumber(luaVM, getFloat(lua_tostring(luaVM, -1)));
		return 1;
	}
	if (isCurrentMethod("getString")) {
		lua_pushstring(luaVM, getString(lua_tostring(luaVM, -1)).c_str());
		return 1;
	}
	if (isCurrentMethod("getVector")) {
		CVector v = getVector(lua_tostring(luaVM, -1));
		luaPushVector(luaVM, v.x, v.y, v.z);
		return 1;
	}
	if (isCurrentMethod("setInt")) {
		setInt(lua_tostring(luaVM, 1), lua_tointeger(luaVM, 2));
		return 0;
	}
	if (isCurrentMethod("setFloat")) {
		setFloat(lua_tostring(luaVM, 1), lua_tonumber(luaVM, 2));
		return 0;
	}
	if (isCurrentMethod("setString")) {
		setString(lua_tostring(luaVM, 1), string(lua_tostring(luaVM, 2)));
		return 0;
	}
	if (isCurrentMethod("setVector")) {
		setVector(lua_tostring(luaVM, 1), CVector(lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3), lua_tonumber(luaVM, 4)));
		return 0;
	}
	if (isCurrentMethod("isExist")) {
		int res = isExist(lua_tostring(luaVM, -1));
		lua_pushboolean(luaVM, res);
		return 1;
	}
	if (isCurrentMethod("clearConfig")) {
		clearConfig();
		return 0;
	}

	return LuaBridge::methodsBridge(luaVM);
}



MGConfig::~MGConfig()
{
}

