/*
 * LuaVM.h
 *
 *  Created on: 25.01.2010
 *      Author: Администратор
 */

#ifndef LUAVM_H_
#define LUAVM_H_

extern "C" {
# include "./lua/lua.h"
# include "./lua/lauxlib.h"
# include "./lua/lualib.h"
}
#include <string>
#include <vector>

using namespace std;

class Game;

enum PropertyType {
	MG_BOOL,
	MG_FLOAT,
	MG_INT,
	MG_VECTOR,
	MG_STRING,
	MG_COLOR
};

struct Property {
	PropertyType type;
	string name;
	void* pointer;
};


class LuaVM {
private:
	lua_State* luaVM;
	vector<Property*> props;

public:
	Game* game;

	void init();
	void restart();
	void doString(string s);
	void doFile(string path);
	void registerProperty(string name, PropertyType type, void* pointer);
	void registerFunc(string name, lua_CFunction f);
	lua_State* getVM();

	LuaVM(Game* game);
	virtual ~LuaVM();
};

// Lua bridge
int l_printInfo(lua_State* luaVM);
int l_printError(lua_State* luaVM);
int l_getProperty(lua_State* luaVM);
int l_setProperty(lua_State* luaVM);


#endif /* LUAVM_H_ */
