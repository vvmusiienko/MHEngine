#ifndef GRAPHICENGINE_H_
#define GRAPHICENGINE_H_

#include "./platform/opengl.h"
#include <string>
#include <vector>
#include "./glfont2/glfont2.h"
#include "Common.h"
#include "CVector.h"
#include "LuaBridge.h"

class RenderFilter;

using namespace std;

typedef struct
{
	unsigned char imageTypeCode;
	short int imageWidth;
	short int imageHeight;
	unsigned char bitCount;
	unsigned char *imageData;
} TGAFILE;


// +++++++++++++++++++ FONTS +++++++++++++++++++++++++

enum FontStyle {
	STANDART_14_NORMAL,
	CONSOLE_8_NORMAL
};


class Game;
class GameObject;

class GraphicEngine : LuaBridge
{
private:
	Game* game;
	
	// Text output
	vector<string> debugStrings;
	bool needSaveScreenShot;
	bool renderWhireFrame;
	GLenum currentBlendFuncS;
	GLenum currentBlendFuncD;
	CVector masterPositionOffset;
	CVector masterOrientationOffset;
	bool lightingEnabled;
	RenderFilter* renderFilter;

protected:
	virtual int methodsBridge(lua_State* luaVM);

public:

	static unsigned int lastBindedTexture;

	// Blending
	GLenum getDefaultBlendFuncS();
	GLenum getDefaultBlendFuncD();
	GLenum getCurrentBlendFuncS();
	GLenum getCurrentBlendFuncD();
	void setBlendFunc(GLenum s, GLenum d);

	bool isLightingEnabled();
	void enableLighting(bool v);

	void setMasterPositionOffset(CVector v);
	void setMasterOrientationOffset(CVector v);
	CVector getMasterPositionOffset();
	CVector getMasterOrientationOffset();


	glfont::GLFont* standart14Normal;
	glfont::GLFont* console8Normal;
	bool blockDebugTexts;

	void updateScreenSize();
	void initGL(); // Init OpenGL library
	void beginRender(); // Begin rendering the game 
	void endRender(); // Ending rendering the game 
	void renderWorld(); // Render world objects: walls, players, camera positioning...
	bool saveScreenshot();
	void getScreenshot();
	void setRenderFilter(RenderFilter* f);
	
	void start2D();
	void end2D();

	void setAmbient(MGColor c);
	void setClearColor(MGColor c);

	// Text output
	void writeText(string text, CVector position, FontStyle style, MGColor color);
	void writeDebugText(const char *format, ...);
	
	bool toggleWhireFrame(); // set wireframe mode. return current mode

	// return object at point of screen
	GameObject* getObjectAtPoint(int x, int y);

	void drawLine(const CVector& from,const CVector& to,const CVector& color);
	void drawBBox(BBox bbox);
	static bool sortTransparent(GameObject* a, GameObject* b);
	static bool sort2D(GameObject* a, GameObject* b);

	GraphicEngine(Game *game);
	virtual ~GraphicEngine();
};

// Lua bridge

int l_writeDebugText(lua_State* luaVM);
int l_writeText(lua_State* luaVM);

#endif /*GRAPHICENGINE_H_*/
