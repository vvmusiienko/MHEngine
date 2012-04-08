/*
 * BorlandBridge.h
 *
 *  Created on: 24.02.2010
 *      Author: Администратор
 */

#ifndef BORLANDBRIDGE_H_
#define BORLANDBRIDGE_H_

#include "windows.h"
extern "C" {
# include "./lua/lua.h"
# include "./lua/lauxlib.h"
# include "./lua/lualib.h"
}

#define DLL_FNC		__declspec(dllimport) __stdcall

extern "C" {
	void DLL_FNC game_init();
	void DLL_FNC game_iterate(float dt);
	void DLL_FNC game_finalize();

	void DLL_FNC game_update_screen_size();
	void DLL_FNC adapter_set_fields(HDC _hDC, HINSTANCE _hInstance);
	void DLL_FNC game_set_screen_size(int width, int height);
	void DLL_FNC game_mousedown(int button);
	void DLL_FNC game_mouseup(int button);
	void DLL_FNC game_moved(float x, float y);
	void DLL_FNC game_keydown(unsigned char key);
	void DLL_FNC game_keyup(unsigned char key);
	void DLL_FNC game_keypressed(unsigned char key);
	void DLL_FNC game_doscript(const char* script);

	void DLL_FNC game_regfunc(const char* name, lua_CFunction f);
	bool DLL_FNC game_idused(const char* id);
	bool DLL_FNC game_save(const char* path, const char* script);
	bool DLL_FNC game_save_object(const char* objID, const char* path, const char* script);
	bool DLL_FNC game_load(const char* name);
	bool DLL_FNC game_load_object(const char* path, const char* objName);

	void DLL_FNC game_config_save_str(const char* key, char* value);
	const char* DLL_FNC game_config_get_str(const char* key);
	bool DLL_FNC game_config_exist(const char* key);
}


#endif /* BORLANDBRIDGE_H_ */
