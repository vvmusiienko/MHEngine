/*
 * LuaVM.cpp
 *
 *  Created on: 25.01.2010
 *      Author: Администратор
 */

#include "LuaVM.h"
#include "Game.h"
#include "Log.h"
#include "object/GameObject.h"
#include "MapManager.h"


LuaVM::LuaVM(Game* game) {
	this->game = game;
	luaVM = luaL_newstate();

	if (NULL == luaVM)
	{
		Log::error("Error init LUA");
	}
	luaL_openlibs(luaVM);
}


void LuaVM::init() {
	// register functions
	lua_register( luaVM, "printInfo", l_printInfo );
	lua_register( luaVM, "printError", l_printError );
	lua_register( luaVM, "get", l_getProperty );
	lua_register( luaVM, "set", l_setProperty );

	doFile(game->osAdapter->getResourcePath() + "scripts/standart.lua");

	Log::info("LuaVM init OK");
}


void LuaVM::registerFunc(string name, lua_CFunction f) {
	lua_register(luaVM, name.c_str(), f);
}


void LuaVM::restart() {
	// porjadok!!!!!!!!! nado!!!!!!!!
	//init();
}


void LuaVM::doString(string s) {
	if (luaL_dostring(luaVM, s.c_str())) {
		Log::warning("LUA: %s", lua_tostring(luaVM, -1));
	}
}


void LuaVM::doFile(string path) {
	if (luaL_dofile(luaVM, path.c_str())) {
		Log::error("Error execute %s! %s", path.c_str(), lua_tostring(luaVM, -1));
	}
}


lua_State* LuaVM::getVM() {
	return luaVM;
}


void LuaVM::registerProperty(string name, PropertyType type, void* pointer) {
	Property* p = new Property;
	p->name = name;
	p->type = type;
	p->pointer = pointer;
	props.push_back(p);
	char getCommand[512];
	sprintf(getCommand, "%s = function() return get(%i) end", FormatString(name.c_str(), "g_").c_str(), (unsigned int)p);
	char setCommand[512];
	sprintf(setCommand, "%s = function(...) set(%i, ...) end", FormatString(name.c_str(), "s_").c_str(), (unsigned int)p);
	luaL_dostring(luaVM, setCommand);
	luaL_dostring(luaVM, getCommand);
}


LuaVM::~LuaVM() {
	// TODO Auto-generated destructor stub
}

int l_printInfo(lua_State* luaVM) {
	Log::info(string("LUA: ") + lua_tostring(luaVM, -1));
	return 0;
}


int l_printError(lua_State* luaVM) {
	Log::error(string("LUA: ") + lua_tostring(luaVM, -1));
	return 0;
}


int l_getProperty(lua_State* luaVM) {
	Property* p = (Property*)lua_tointeger(luaVM, -1);

	if (!p)
		Log::error("l_setProperty : destination object is nil!");

	switch (p->type) {
		case MG_BOOL:
			lua_pushboolean(luaVM, *(bool*)p->pointer);
			break;
		case MG_FLOAT:
			lua_pushnumber(luaVM, *(float*)p->pointer);
			break;
		case MG_INT:
			lua_pushinteger(luaVM, *(int*)p->pointer);
			break;
		case MG_VECTOR: {
			CVector* v = (CVector*)p->pointer;
			lua_newtable(luaVM);
			int tableIndex = lua_gettop(luaVM);
			lua_pushstring(luaVM, "x");
			lua_pushnumber(luaVM, v->x);
			lua_settable (luaVM, tableIndex);
			lua_pushstring(luaVM, "y");
			lua_pushnumber(luaVM, v->y);
			lua_settable (luaVM, tableIndex);
			lua_pushstring(luaVM, "z");
			lua_pushnumber(luaVM, v->z);
			lua_settable (luaVM, tableIndex);
			break;
		}
		case MG_STRING: {
			lua_pushstring(luaVM, ((string*)p->pointer)->c_str());
			break;
		}
		case MG_COLOR: {
			MGColor* v = (MGColor*)p->pointer;
			lua_newtable(luaVM);
			int tableIndex = lua_gettop(luaVM);
			lua_pushstring(luaVM, "r");
			lua_pushnumber(luaVM, v->getR());
			lua_settable (luaVM, tableIndex);
			lua_pushstring(luaVM, "g");
			lua_pushnumber(luaVM, v->getG());
			lua_settable (luaVM, tableIndex);
			lua_pushstring(luaVM, "b");
			lua_pushnumber(luaVM, v->getB());
			lua_settable (luaVM, tableIndex);
			lua_pushstring(luaVM, "a");
			lua_pushnumber(luaVM, v->getA());
			lua_settable (luaVM, tableIndex);
			break;
		}
		default:
			Log::error("l_getProperty incorrect variable type %i", p->type);
			break;
	}

	return 1;
}


int l_setProperty(lua_State* luaVM) {
	Property* p = (Property*)lua_tointeger(luaVM, 1);

	if (!p) {
		Log::warning("l_setProperty : destionation object is nil!");
		return 0;
	}
	switch (p->type) {
		case MG_BOOL:
			*(bool*)p->pointer = lua_toboolean(luaVM, 2);
			break;
		case MG_FLOAT:
			*(float*)p->pointer = lua_tonumber(luaVM, 2);
			break;
		case MG_INT:
			*(int*)p->pointer = lua_tointeger(luaVM, 2);
			break;
		case MG_VECTOR: {
			CVector* v = (CVector*)p->pointer;
			v->x = lua_tonumber(luaVM, 2);
			v->y = lua_tonumber(luaVM, 3);
			v->z = lua_tonumber(luaVM, 4);
			break;
		}
		case MG_STRING: {
			string* s = (string*)p->pointer;
			s->assign(lua_tostring(luaVM, 2));
			break;
		}
		case MG_COLOR: {
			MGColor* v = (MGColor*)p->pointer;
			v->setR((float)lua_tonumber(luaVM, 2));
			v->setG((float)lua_tonumber(luaVM, 3));
			v->setB((float)lua_tonumber(luaVM, 4));
			v->setA((float)lua_tonumber(luaVM, 5));
			break;
		}
		default:
			Log::warning("l_setProperty incorrect variable type %i", p->type);
			break;
	}

	return 0;
}
