/*
 * LuaBridge.cpp
 *
 *  Created on: 18.05.2010
 *      Author: Администратор
 */

#include "LuaBridge.h"
#include "Common.h"


LuaVM* LuaBridge::vm = NULL;

LuaBridge::LuaBridge() {
	maxMethodIndex = 0;
	currentMethodIndex = 0;
	offsets = new short int[300]; // TODO: hardcoded methods number
	memset(offsets, 0, 300 * 2);
	indexTableInitialized = false;
	passCheking = false;
}


int LuaBridge::l_centralMethodBridge(lua_State* luaVM) {
	LuaBridge* p = (LuaBridge*)lua_tointeger(luaVM, 1);
	short int methodIndex = lua_tointeger(luaVM, 2);

	// remove from stack system values
	lua_remove(luaVM, 1);
	lua_remove(luaVM, 1);

	if (!p) {
		Log::warning("LuaBridge::l_centralMethodBridge: incorrect pointer");
		return 0;
	}

	if (!p->indexTableInitialized) {
		// кожне значення индексу це x - (y - z)
		// x - теперішне значення индексу
		// y - кількість методів зареганих після останнього виклику registrateMethods
		// z зміщення від початку в момент виклику registrateMethods
		int lastV = p->offsets[p->maxMethodIndex-1];
		int offset = 0;
		for (int i = p->maxMethodIndex-1; i >= 0; i--) {
			if (p->offsets[i] != lastV) {
				offset = (p->maxMethodIndex-1) - i;
				lastV = p->offsets[i];
			}

			//Log::info("%i  %i %i   realIndex: %i", i, p->offsets[i], offset, (i+1) - (p->offsets[i] - offset));
			p->offsets[i+1] = (i+1) - (p->offsets[i] - offset);
			//         ^
			//         |
			//         shift by +1 index (щоб позбутися операції віднімання)
		}

		p->indexTableInitialized = true;
	}

	p->currentMethodIndex = 0;
	p->calledMethodIndex = p->offsets[methodIndex];
	static int ret;
	if (ret = p->methodsBridge(luaVM) != -1) {
		return ret;
	}

	Log::info("LuaBridge::l_centralMethodBridge: no found matches method handlers! (maybe name mihmash in registerMethod and methodsBridge)");
	return 0;
}


void LuaBridge::registerMethod(string object, short int methodIndex, string method) {
	vm->doString(FormatString( "%s.%s = function(...) return %s(%i, %i, ...) end", object.c_str(), method.c_str(), BRIDGE_NAME, this, methodIndex ));
	registeredMethods.push_back(method);
	if (maxMethodIndex < methodIndex)
		maxMethodIndex = methodIndex;
}


bool LuaBridge::isAlreadyRegistered(const string& method) {
	for (unsigned int i = 0; i < registeredMethods.size(); i++) {
		if (registeredMethods.at(i) == method)
			return true;
	}
	return false;
}


bool LuaBridge::isCurrentMethod(const string& method) {
	currentMethodIndex++;

	if (calledMethodIndex == 0 && !passCheking) {
		// it's registering
		if (!isAlreadyRegistered(method)) {
			offsets[currentMethodIndex-1] = currentOffset;
			registerMethod(objectID, currentMethodIndex, method.c_str());
			//Log::info("%i reg %s  curIndx %i   %i", this, method.c_str(), currentMethodIndex, offsets[currentMethodIndex]);
		} else {
			passCheking = true; // possibly is second loop, because of this pass the next chekings
		}
		return false;
	}

	if (calledMethodIndex == currentMethodIndex) {
		// it's method
		//Log::info("%i %s  indx %i",this, method.c_str(), calledMethodIndex);
		return true;
	}
	return false;


}


void LuaBridge::registrateMethods(string forID) {
	objectID = forID;
	calledMethodIndex = 0;
	currentOffset = currentMethodIndex;
	passCheking = false;
	if (!vm) Log::error("LuaBridge not initialized!");
	vm->doString("if "+objectID+" == nil then "+objectID+" = {} end ");
	methodsBridge(vm->getVM());
}


int LuaBridge::methodsBridge(lua_State* luaVM) {
	return -1;
}


void LuaBridge::initStatic(LuaVM* luaVM) {
	vm = luaVM;
	lua_register(vm->getVM(), BRIDGE_NAME, l_centralMethodBridge);
	Log::info("Central Lua bridge (%s) registered.", BRIDGE_NAME);
}


LuaBridge::~LuaBridge() {
	// TODO Auto-generated destructor stub
}
