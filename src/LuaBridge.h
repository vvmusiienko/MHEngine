/*
 * LuaBridge.h
 *
 *	Base class for all objects who will share self methods between Lua and C++;
 *
 *  Created on: 18.05.2010
 *      Author: Администратор
 */

#ifndef LUABRIDGE_H_
#define LUABRIDGE_H_

#include "Log.h"
#include "LuaVM.h"

#include <string>
using namespace std;

#define BRIDGE_NAME		"go_br"

class LuaBridge {
private:
	static LuaVM* vm;
	static int l_centralMethodBridge(lua_State* luaVM);

	short int maxMethodIndex;
	short int currentMethodIndex;
	short int calledMethodIndex;
	string objectID;
	vector<string> registeredMethods;
	short int* offsets;
	short int currentOffset;
	bool indexTableInitialized;
	bool passCheking;

	void registerMethod(string object, short int methodIndex, string method);
	bool isAlreadyRegistered(const string& method);

protected:
	bool isCurrentMethod(const string& method);
	virtual int methodsBridge(lua_State* luaVM);
public:
	void registrateMethods(string forID);
	static void initStatic(LuaVM* luaVM);
	LuaBridge();
	virtual ~LuaBridge();
};

#endif /* LUABRIDGE_H_ */
