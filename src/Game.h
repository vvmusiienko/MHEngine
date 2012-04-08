#ifndef GAME_H_
#define GAME_H_

#include <vector>

#include "./platform/IOSAdapter.h"
#include "LuaBridge.h"
#include "CVector.h"
#include "Common.h"

#define PI					3.14159265
#define OBJ_MAP_WIDTH		1000
#define OBJ_MAP_HEIGHT		100
#define OBJ_MAP_CELL_SIZE	5	// X^2
#define ACTIVE_CURSORS_COUNT 10

class MapManager;
class MGConfig;
class GameObject;
class GOCube;
class GOPlayer;
class GOSkyBox;
class UIResponder;
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class PhysicObject;
class GOConstraint;
class GraphicEngine;
class Camera;
class Console;
class MapProperties;

class Game : LuaBridge
{
	friend class PhysicObject;
	friend class GraphicEngine;
	friend class GOConstraint;


private:
	bool gameStoped; // True if need stop the game
	bool needRenderWorld; // If true then game will be rendered
	bool needRenderMenu; // If true then menu will be showed
	bool needRenderConsole; // If true then show console 
	bool physIsDisabled;
    
	int msFromLast1000ms; // for periodic call of call1000ms()
	int msFromLast250ms; // for periodic call of call1000ms()
	
	int framesPassed; // frames passed from last second
	int fps;
	
	unsigned long int avgU;
	unsigned long int avgR;
	unsigned long int totalU;
	unsigned long int totalR;

	// Profiler
	int lastCallTime;

	void call250ms(); // coling this function each 250 ml passed
	void call1000ms(); // coling this function each 1000 ml passed
	
	float maxDT;
	int iterations; // count of phys iterations

	// PHYS VARIABLES
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;
	float physIterationsPerSecond;
	int maxPhysSubsteps;

	// constraints
	vector<GOConstraint*> constraints;

	bool needCheckCursorOver;
	CursorState* activeCursors[ACTIVE_CURSORS_COUNT]; // cursors is downed
	void calculateCursorsOverResponders(CursorState c, int cursorNum); // for onCursorOver event

    int textureQuality;
    bool mipmapEnabled;
    
protected:
	virtual int methodsBridge(lua_State* luaVM);
	static bool sortResponders(UIResponder* a, UIResponder* b);

public:
	static Game* instance;
	MapProperties* map;
	MapManager* mapManager;
	MGConfig* config;
	Console* console;
	LuaVM* luaVM;
	GOPlayer* player;
	GOSkyBox* skyBox;
	GraphicEngine *graphicEngine; // Class for rendering all objects and work with OpenGL
	CVector gravity;
	CVector airResist;
	bool needSaveToMap; // if true then each created GameObject has seted flag needSaveToMap
	int maxSubSteps;
	float fixedTimeStep;
	string firstObjectID; // if need override first object ID
	string oldFirstObjectID; // if need override first object ID
	bool needTmpIDWhileSaving;


	bool editorMode; // For editor specialy
	bool debugLabels; // if true then each label render self size
	bool passUpdate;
	bool drawDebugText;
	bool debugPhysDraw;
	bool debugPhysDrawIn2D;
	bool drawBBoxes;
	int throtle; // for farame limit
	float dt; // dt is dt
	unsigned long int  startTime; // time in ms after program starts
	// STATICTIC
	int _2dObjectsCount;
	int transparentObjectsCount;
	int enabledObjectsCount;
	int drawnObjectsCount;
	unsigned int totalBufferRefreshCount;

	Camera *camera;
	IOSAdapter* osAdapter; // bridge beetwen os and game engine
	
	GOCube *plate_s;
	
	vector<GameObject*> objects;

	// User Iteractions
	vector<UIResponder*> responders;
	vector<UIResponder*> firstResponders;
	void deleteDeadObjects();
	
    void disablePhys(bool v);
    bool isPhysDisabled();
    
    // Texture settings
    void setTextureQuality(int q); // 1 - load all testures from "textures_1", 2-first try load from "textures_2 then textures_1. etc... default 1"
    int getTextureQuality();
    void setMipmapEnabled(bool v); // enable or disable mipmaping. Default - true
    bool getMipmapEnabled();
    
    
	// SOUND
	void setListenerPosition(CVector v);
	void setListenerOrientation(CVector at, CVector up);

	void startProfilerZone();
	// QWE
	int endProfilerZone();
	// get from osadapter screen size for opengl and settinup opengl for it
	void updateScreenSize();
	// Return true if game stoping
	bool needStop();
	// Stop game and set needStop to true
	void stopGame();
	// Main initialization of game
	void initialize();
	// Iterate game cycle
	void iterate(float dt, int maxSubSteps = 1, float fixedTimeStep = 30.0f);
	// Funalize game
	void finalize();
	// array with current keyboard state
	bool keys[256];
	// key Adapters
	void keyDown(unsigned char key);
	void keyUp(unsigned char key);
	void charEntered(unsigned char c);

	// current mouse state
	CursorState mouseState;
	// mouse adapter
	void cursorDown(CursorButton button, int cursorNum = 0);
	void cursorUp(CursorButton button, int cursorNum = 0);
	void cursorMoved(float x, float y, int cursorNum = 0);
	void cursorWheelMoved(float dx, float dy, int cursorNum = 0);
	
	Game(IOSAdapter* osAdapter);
	virtual ~Game();
};

#endif /*GAME_H_*/
