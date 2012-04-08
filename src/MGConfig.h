#ifndef MGCONFIG_H_
#define MGCONFIG_H_

#include <string>
#include <vector>
#include "CVector.h"
#include "LuaBridge.h"

using namespace std;

#define	CONFIG_FILE_NAME	"config.dat"

enum ConfigItemType {
	CI_INT,
	CI_FLOAT,
	CI_STRING,
	CI_VECTOR
};

struct ConfigItem {
	ConfigItemType type;
	string key;
	int intValue;
	float floatValue;
	string stringValue;
	CVector vectorValue;
};

class Game;

class MGConfig : LuaBridge
{

private:
	Game* game;
    vector<ConfigItem*> items;

protected:
	virtual int methodsBridge(lua_State* luaVM);
	
public:
	
	int getInt(const string key);
	float getFloat(const string key);
	string getString(const string key);
	CVector getVector(const string key);
	
	void setInt(const string key, int value);
	void setFloat(const string key, float value);
	void setString(const string key, string value);
	void setVector(const string key, CVector value);
	
	bool isExist(const string key);
	ConfigItem* getItem(const string key);

	void saveConfig();
	void loadConfig();
	void clearConfig();

	MGConfig(Game* game);
	virtual ~MGConfig();
};

#endif /*MGCONFIG_H_*/
