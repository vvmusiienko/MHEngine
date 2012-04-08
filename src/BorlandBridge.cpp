#include "windows.h"
#include "platform/IOSAdapter.h"
#include <string>
#include "Game.h"
#include "GraphicEngine.h"
#include "MapManager.h"
#include "MGConfig.h"

using namespace std;

class BorlandOSAdapter : public IOSAdapter
{
private:
	HDC hDC;
	HINSTANCE hInstance;

public:
	BorlandOSAdapter(HDC _hDC, HINSTANCE _hInstance);
	void swapBuffers();
	unsigned long int getTickCount();
	string getWorkingPath();
	string getResourcePath();
	void terminate();
	void showMessage(const string title, const string text);
	int getScreenWidth();
	int getScreenHeight();

	virtual ~BorlandOSAdapter();
};

BorlandOSAdapter::BorlandOSAdapter(HDC _hDC, HINSTANCE _hInstance)
{
	hDC = _hDC;
	hInstance = _hInstance;
}

static int s_width = 640;
static int s_height = 480;

int BorlandOSAdapter::getScreenWidth() {
	return s_width;
}


int BorlandOSAdapter::getScreenHeight() {
	return s_height;
}


void BorlandOSAdapter::swapBuffers()
{
	SwapBuffers(hDC);
}


unsigned long int BorlandOSAdapter::getTickCount()
{
	return GetTickCount();
}


string BorlandOSAdapter::getWorkingPath()
{
	return string("E:\\cpp_projects\\MHEditor\\");
}


string BorlandOSAdapter::getResourcePath()
{
	return "E:\\cpp_projects\\MHGame\\";
}


void BorlandOSAdapter::terminate()
{
	exit(0);
}


void BorlandOSAdapter::showMessage(const string title, const string text)
{
	MessageBox(0, text.c_str(), title.c_str(), MB_OK);
}


BorlandOSAdapter::~BorlandOSAdapter()
{
}

#define DLL_FNC		__declspec(dllexport) __stdcall

static HDC __hDC;
static HINSTANCE __hInstance;
static Game* game = NULL;
static BorlandOSAdapter* osa;

extern "C" {

	void DLL_FNC game_init() {
		osa = new BorlandOSAdapter(__hDC, __hInstance);
		game = new Game(osa);
		game->initialize();
	}

	void DLL_FNC game_iterate(float dt) {
		game->iterate(dt);
	}

	void DLL_FNC game_finalize() {
		game->finalize();
		game = NULL;
	}

	void DLL_FNC game_update_screen_size() {
		game->graphicEngine->updateScreenSize();
	}

	void DLL_FNC adapter_set_fields(HDC _hDC, HINSTANCE _hInstance) {
		__hDC = _hDC;
		__hInstance = _hInstance;
	}

	void DLL_FNC game_set_screen_size(int width, int height) {
		s_width = width;
		s_height = height;
		if(game)
			game->updateScreenSize();
	}

	void DLL_FNC game_mousedown(int button) {
		game->cursorDown((CursorButton)button);
	}

	void DLL_FNC game_mouseup(int button) {
		game->cursorUp((CursorButton)button);
	}

	void DLL_FNC game_moved(float x, float y) {
		game->cursorMoved(x, y);
	}

	void DLL_FNC game_keydown(unsigned char key) {
		game->keyDown(key);
	}

	void DLL_FNC game_keyup(unsigned char key) {
		game->keyUp(key);
	}

	void DLL_FNC game_keypressed(unsigned char key) {
		game->charEntered(key);
	}

	void DLL_FNC game_doscript(const char* script) {
		game->luaVM->doString(script);
	}

	void DLL_FNC game_regfunc(const char* name, lua_CFunction f) {
		game->luaVM->registerFunc(name, f);
	}

	bool DLL_FNC game_idused(const char* id) {
		return game->mapManager->findByID(id);
	}

	bool DLL_FNC game_save(const char* path, const char* script) {
		return game->mapManager->saveMap(path, script);
	}

	bool DLL_FNC game_save_object(const char* objID, const char* path, const char* script) {
		GameObject* o = game->mapManager->findByID(objID);
		if (o) {
			game->mapManager->saveObject(o, path, script);
			return true;
		}
		return false;
	}

	bool DLL_FNC game_load(const char* name) {
		return game->mapManager->loadMap(name);
	}

	bool DLL_FNC game_load_object(const char* path, const char* objName) {
		return game->mapManager->loadObject(path, objName);
	}

	void DLL_FNC game_config_save_str(const char* key, char* value) {
		game->config->setString(key, value);
	}

	const char* DLL_FNC game_config_get_str(const char* key) {
		return game->config->getString(key).c_str();
	}

	bool DLL_FNC game_config_exist(const char* key) {
		return game->config->isExist(key);
	}


}

