#include "GraphicEngine.h"
#include "Game.h"
#include "Camera.h"
#include "object/GOCube.h"
#include "object/GOPlate.h"
#include "object/GOLight.h"
#include <vector>
#include "time.h"
#include <stdio.h>
#include "Image.h"
#include "BuildSettings.h"
#include "./FontCache.h"
#include "Console.h"
#include "MGConfig.h"
#include "Log.h"
#include "./renderfilter/PlatformerRenderFilter.h"
#include <btBulletDynamicsCommon.h>

unsigned int GraphicEngine::lastBindedTexture = 0;


GraphicEngine::GraphicEngine(Game *game)
{
	this->game = game;
	needSaveScreenShot = false;
	renderWhireFrame = false;	  
	registrateMethods("game");
	blockDebugTexts = false;
	lightingEnabled = true;
	renderFilter = new PlatformerRenderFilter(CVector(500, 400));

#ifndef IPHONE_OS
	GLenum err = glewInit();
	if (GLEW_OK != err) {
	  Log::error(FormatString("Error: %s\n", glewGetErrorString(err)));
	}
#endif
}

GraphicEngine::~GraphicEngine()
{
}


void GraphicEngine::setAmbient(MGColor c) {
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, c.getFloatComponents());
}


void GraphicEngine::setClearColor(MGColor c) {
	glClearColor (c.getR(), c.getG(), c.getB(), c.getA());
}


void GraphicEngine::initGL() {
	
	// Light init
	if (game->config->getInt("lighting"))
		glEnable(GL_LIGHTING);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

	setAmbient(MGColor(0.5f, 0.5f, 0.5f, 1.0f));
	setClearColor(MGColor(0.0f, 0.0f, 0.0f, 1.0f));

	//glPolygonMode(GL_FRONT, GL_LINE);
	//glPolygonMode(GL_BACK, GL_LINE);	
	
	glShadeModel (GL_SMOOTH); // Метод закраски: СГЛАЖЕНЫЙ 
	glEnable (GL_DEPTH_TEST); // Включаем тест глубины 
	// ВКЛ. АВТОМАТИЧЕСКИЙ ПЕРЕСЧЕТ НОРМАЛЕЙ 
	//glEnable (GL_AUTO_NORMAL); 
	glEnable (GL_NORMALIZE); 
	// ВКЛ. ОТСЕЧЕНИЕ ЗАДНИХ ГРАНЕЙ 
	glEnable (GL_CULL_FACE); // Включаем отсечение граней 
	glCullFace (GL_BACK); // Указываем, что отсекать будем задние грани	

	updateScreenSize();
	
	// Init fonts
	glEnable(GL_BLEND);
	setBlendFunc(getDefaultBlendFuncS(), getDefaultBlendFuncD());
	

	standart14Normal = FontCache::getInstance()->load("standart_14_normal");
	console8Normal = FontCache::getInstance()->load("console_8_normal");
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	
	GLenum err = glGetError ();
	if (GL_NO_ERROR != err)
		Log::error("GraphicEngine::initGL(): OpenGL Error: %s", gluErrorString (err));
	
	Log::info("OpenGL initialized");
	  
}

GLenum GraphicEngine::getDefaultBlendFuncS() {
	return GL_SRC_ALPHA;
}

GLenum GraphicEngine::getDefaultBlendFuncD() {
	return GL_ONE_MINUS_SRC_ALPHA;
}

GLenum GraphicEngine::getCurrentBlendFuncS() {
	return currentBlendFuncS;
}

GLenum GraphicEngine::getCurrentBlendFuncD() {
	return currentBlendFuncD;
}

void GraphicEngine::setBlendFunc(GLenum s, GLenum d) {
	glBlendFunc(s, d);
	currentBlendFuncS = s;
	currentBlendFuncD = d;
}


bool GraphicEngine::isLightingEnabled() {
	return lightingEnabled;
}


void GraphicEngine::enableLighting(bool v) {
	lightingEnabled = v;
	if (lightingEnabled) {
		glEnable(GL_LIGHTING);
	} else {
		glDisable(GL_LIGHTING);
	}
}


void GraphicEngine::updateScreenSize() {
	float width = game->osAdapter->getViewportWidth();
	float height = game->osAdapter->getViewportHeight();
	
    glViewport(0, 0, width, height);
    
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,width/height,1.0f,10000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

GameObject* GraphicEngine::getObjectAtPoint(int x, int y) {
#if OPENGL_ES
	return NULL;
#else
	GLuint buff[64] = {0};
	GLint hits, view[4];

	//	This choose the buffer where store the values for the selection data
	glSelectBuffer(64, buff);

	//	This retrieve info about the viewport
	glGetIntegerv(GL_VIEWPORT, view);

	//	Switching in selecton mode
	glRenderMode(GL_SELECT);

	//	Clearing the name's stack
	//	This stack contains all the info about the objects
	glInitNames();

	//	Now fill the stack with one element (or glLoadName will generate an error)
	glPushName(0);

	//	Now modify the vieving volume, restricting selection area around the cursor
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();

		//	restrict the draw to an area around the cursor
		gluPickMatrix(x, y, 1.0, 1.0, view);

		int width = game->osAdapter->getScreenWidth();
		int height = game->osAdapter->getScreenHeight();
		gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,1.0f,10000.0f);

		//	Draw the objects onto the screen
		glMatrixMode(GL_MODELVIEW);

		//	draw only the names in the stack, and fill the array
		game->osAdapter->swapBuffers();
		renderWorld();

		//	Do you remeber? We do pushMatrix in PROJECTION mode
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	//	get number of objects drawed in that area
	//	and return to render mode
	hits = glRenderMode(GL_RENDER);

	 	/*
	 		For each hit in the buffer are allocated 4 bytes:
	 		1. Number of hits selected (always one,
	 									beacuse when we draw each object
	 									we use glLoadName, so we replace the
	 									prevous name in the stack)
	 		2. Min Z
	 		3. Max Z
	 		4. Name of the hit (glLoadName)
	 	*/

	 	//Log::info("%d hits:\n", hits);

	 	GameObject* res = NULL;
	 	vector<GameObject*> objects;

	 	for (int i = 0; i < hits; i++) {
	 		for (unsigned int j = 0; j < game->objects.size(); j++) {
	 			if (game->objects.at(j)->glName == (GLubyte)buff[i * 4 + 3] && game->objects.at(j)->id != "undefined") {
	 				objects.push_back(game->objects.at(j));
 					//Log::info("Object: %s  %i  ", game->objects.at(j)->id.c_str(), (GLubyte)buff[i * 4 + 3]);
	 			}
	 		}
	 	}

	 	if (objects.size() > 1) {
	 		objects.erase(objects.begin() + objects.size() -1);
	 	}

	 	if (objects.size() > 0 && objects.at(objects.size() -1)->id != "undefined") {
	 		res = objects.at(objects.size() -1);
	 	}

	glMatrixMode(GL_MODELVIEW);
	return res;
#endif
}


void GraphicEngine::writeText(string text, CVector position, FontStyle style, MGColor color) {
	
	glColor4f(color.getR(), color.getG(), color.getB(), color.getA());
	
	switch(style) {
		case STANDART_14_NORMAL:
			standart14Normal->Begin();
			//standart14Normal->DrawString(text.c_str(), 1.0f, position.x, position.y);
			standart14Normal->DrawString(text.c_str(), 1.0, position.x, position.y, MGColor(255,128,128,255).getByteComponents(), MGColor(128,255,128,255).getByteComponents());
			break;
		case CONSOLE_8_NORMAL:
			console8Normal->Begin();
			//console8Normal->DrawString(text.c_str(), 1.0f, position.x, position.y);
			console8Normal->DrawString(text.c_str(), 1.0, position.x, position.y);
			break;
		default:
			Log::error("undefined font style");
	}
	
}

void GraphicEngine::writeDebugText(const char *format, ...) {
	if (blockDebugTexts)
		return;

	char buffer[1024];
	va_list arg;
	int ret;
	
	// Format the text
	va_start (arg, format);
	  ret = vsnprintf(buffer, sizeof (buffer), format, arg);
	va_end (arg);
	
	debugStrings.push_back(buffer);	
}


bool GraphicEngine::saveScreenshot()
{
	unsigned char *imageData;	
	imageData = (unsigned char *)malloc(game->osAdapter->getScreenWidth()*game->osAdapter->getScreenHeight()*3); // allocate memory for the imageData
	memset(imageData, 0, game->osAdapter->getScreenWidth()*game->osAdapter->getScreenHeight()*3); // clear imageData memory contents
	// read the image data from the window
	glReadPixels(0, 0, game->osAdapter->getScreenWidth() - 1, game->osAdapter->getScreenHeight() - 1, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	// write the image data to a file
	char pathToImage[2048];
	strcpy(pathToImage, game->osAdapter->getWorkingPath().c_str());
	char fileName[1024];
	time_t myTime = time(NULL);
	tm *ut = localtime(&myTime);	
	sprintf(fileName, "screenshot_%i-%i-%i_%i-%i.tga", ut->tm_mday, ut->tm_mon+1, 1900+ut->tm_year, ut->tm_hour, ut->tm_min);
	
	strcat(pathToImage, fileName);

	ImageTGA::writeTGAFile(pathToImage, game->osAdapter->getScreenWidth(), game->osAdapter->getScreenHeight(), (unsigned char*)imageData);
	// free the image data memory
	free(imageData);	
	return true;
}

void GraphicEngine::getScreenshot()
{
	needSaveScreenShot = true;
}



bool GraphicEngine::toggleWhireFrame()
{
	renderWhireFrame = !renderWhireFrame;
	return renderWhireFrame;
}


void GraphicEngine::setMasterPositionOffset(CVector v) {
	masterPositionOffset = v;
}


void GraphicEngine::setMasterOrientationOffset(CVector v) {
	masterOrientationOffset = v;
}


CVector GraphicEngine::getMasterPositionOffset() {
	return masterPositionOffset;
}


CVector GraphicEngine::getMasterOrientationOffset() {
	return masterOrientationOffset;
}


void GraphicEngine::start2D() {
    glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glOrtho(0, game->osAdapter->getScreenWidth(), 0, game->osAdapter->getScreenHeight(), INT_MIN, INT_MAX);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable (GL_DEPTH_TEST);

	if (game->config->isExist("rotating")) {
		// Offsets
        glTranslatef(masterPositionOffset.x, masterPositionOffset.y, masterPositionOffset.z);
        glRotatef(masterOrientationOffset.x, 1.0f, 0.0f, 0.0f);
        glRotatef(masterOrientationOffset.y, 0.0f, 1.0f, 0.0f);
        glRotatef(masterOrientationOffset.z, 0.0f, 0.0f, 1.0f);

		gluLookAt(game->camera->position.x, game->camera->position.y, game->camera->position.z,
				  game->camera->lookAt.x + game->camera->position.x, game->camera->lookAt.y + game->camera->position.y, game->camera->lookAt.z + game->camera->position.z,
				  game->camera->up.x, game->camera->up.y, game->camera->up.z);
	}
    //glScalef(0.5f, 0.5f, 1.0f);

}


void GraphicEngine::end2D() {
	glEnable (GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
	glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


void GraphicEngine::beginRender()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Очищаем буфера 
	
#ifndef OPENGL_ES
	if (renderWhireFrame) {
		glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	}
#endif
}


bool GraphicEngine::sortTransparent(GameObject* a, GameObject* b) {
	return ( (a->getPosition() - Game::instance->camera->position).Length() > (b->getPosition() - Game::instance->camera->position).Length());
}


bool GraphicEngine::sort2D(GameObject* a, GameObject* b) {
	return a->getAbsolutePosition().z < b->getAbsolutePosition().z;
}


void GraphicEngine::setRenderFilter(RenderFilter* f) {
	renderFilter = f;
}


void GraphicEngine::drawLine(const CVector& from,const CVector& to,const CVector& color) {
#ifndef OPENGL_ES
	static bool lightingEnabled;
	lightingEnabled = Game::instance->graphicEngine->isLightingEnabled();
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glLineWidth(2);
	glColor3f(color.x, color.y, color.z);
	glBegin(GL_LINES);
		glVertex3d(from.x, from.y, from.z);
		glVertex3d(to.x, to.y, to.z);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	Game::instance->graphicEngine->enableLighting(lightingEnabled);
#endif // OPENGL_ES
}


void GraphicEngine::drawBBox(BBox bbox) {
	// top, bottom, left, right
	drawLine(bbox.upperLeft * CVector(1,1,0) + CVector(0,0,bbox.lowerRight.z), CVector(bbox.lowerRight.x, bbox.upperLeft.y, bbox.lowerRight.z), CVector(0,0,1.0f));
	drawLine(CVector(bbox.upperLeft.x, bbox.lowerRight.y, bbox.lowerRight.z), bbox.lowerRight, CVector(0,0,1.0f));
	drawLine(bbox.upperLeft * CVector(1,1,0) + CVector(0,0,bbox.lowerRight.z), CVector(bbox.upperLeft.x, bbox.lowerRight.y, bbox.lowerRight.z), CVector(0,0,1.0f));
	drawLine(CVector(bbox.lowerRight.x, bbox.upperLeft.y, bbox.lowerRight.z), bbox.lowerRight, CVector(0,0,1.0f));
	// back
	drawLine(bbox.upperLeft, CVector(bbox.lowerRight.x, bbox.upperLeft.y, bbox.upperLeft.z), CVector(0,0,1.0f));
	drawLine(CVector(bbox.upperLeft.x, bbox.lowerRight.y, bbox.upperLeft.z), bbox.lowerRight * CVector(1,1,0) + CVector(0,0,bbox.upperLeft.z), CVector(0,0,1.0f));
	drawLine(bbox.upperLeft, CVector(bbox.upperLeft.x, bbox.lowerRight.y, bbox.upperLeft.z), CVector(0,0,1.0f));
	drawLine(CVector(bbox.lowerRight.x, bbox.upperLeft.y, bbox.upperLeft.z), bbox.lowerRight * CVector(1,1,0) + CVector(0,0,bbox.upperLeft.z), CVector(0,0,1.0f));

	drawLine(bbox.upperLeft * CVector(1,1,0) + CVector(0,0,bbox.lowerRight.z), bbox.upperLeft, CVector(0,0,1.0f));
	drawLine(CVector(bbox.lowerRight.x, bbox.upperLeft.y, bbox.lowerRight.z) , CVector(bbox.lowerRight.x, bbox.upperLeft.y, bbox.upperLeft.z), CVector(0,0,1.0f));
	drawLine(CVector(bbox.upperLeft.x, bbox.lowerRight.y, bbox.lowerRight.z) , CVector(bbox.upperLeft.x, bbox.lowerRight.y, bbox.upperLeft.z), CVector(0,0,1.0f));
	drawLine(bbox.lowerRight, bbox.lowerRight * CVector(1,1,0) + CVector(0,0,bbox.upperLeft.z), CVector(0,0,1.0f));

}


void GraphicEngine::renderWorld()
{
	glMatrixMode(GL_MODELVIEW); // set modelview matrix
	glLoadIdentity(); // clear the current matrix

	// Offsets
	glTranslatef(masterPositionOffset.x, masterPositionOffset.y, masterPositionOffset.z);
	glRotatef(masterOrientationOffset.x, 1.0f, 0.0f, 0.0f);
	glRotatef(masterOrientationOffset.y, 0.0f, 1.0f, 0.0f);
	glRotatef(masterOrientationOffset.z, 0.0f, 0.0f, 1.0f);

	gluLookAt(game->camera->position.x, game->camera->position.y, game->camera->position.z,
			game->camera->lookAt.x + game->camera->position.x, game->camera->lookAt.y + game->camera->position.y, game->camera->lookAt.z + game->camera->position.z,
					game->camera->up.x, game->camera->up.y, game->camera->up.z);
	writeDebugText("Camera position: %0.0f  %0.0f  %0.0f ", game->camera->position.x, game->camera->position.y, game->camera->position.z);
	//writeDebugText("Camera look: %0.2f  %0.2f  %0.2f ", game->camera->lookAt.x, game->camera->lookAt.y, game->camera->lookAt.z);


	static vector<GameObject*> objects_for_drawing;
	static vector<GOLight*> lights;
	static vector<GameObject*> opaque_objects;
	static vector<GameObject*> transparent_objects;
	static vector<GameObject*> _2d_objects;
	objects_for_drawing.clear();
	lights.clear();
	opaque_objects.clear();
	transparent_objects.clear();
	_2d_objects.clear();

	for (unsigned int i = 0; i < game->objects.size(); i++ ) {
		if (game->objects.at(i)->getNeedDraw()) {
			objects_for_drawing.push_back(game->objects.at(i));
		}
	}

	if (Game::instance->drawBBoxes) {
		if (renderFilter) {
			renderFilter->debugDraw();
			for (unsigned int i = 0; i < objects_for_drawing.size(); i++ ) {
				GameObject* o = objects_for_drawing.at(i);
				renderFilter->debugObjectDraw(o);
			}
		} else {
			for (unsigned int i = 0; i < objects_for_drawing.size(); i++ ) {
				drawBBox(objects_for_drawing.at(i)->getBBox());
			}
		}
	}

	for (unsigned int i = 0; i < objects_for_drawing.size(); i++ ) {
		if (objects_for_drawing.at(i)->type == GO_LIGHT) {
			lights.push_back((GOLight*)objects_for_drawing.at(i));
		} else
		if (objects_for_drawing.at(i)->is2D) {
			_2d_objects.push_back(objects_for_drawing.at(i));
		} else
		if (objects_for_drawing.at(i)->opaque) {
			opaque_objects.push_back(objects_for_drawing.at(i));
		} else {
			transparent_objects.push_back(objects_for_drawing.at(i));
		}
	}

	if (renderFilter && !Game::instance->editorMode) {
		opaque_objects = renderFilter->doFilter(opaque_objects);
		transparent_objects = renderFilter->doFilter(transparent_objects);
	}
	game->drawnObjectsCount = opaque_objects.size();
	game->drawnObjectsCount += transparent_objects.size();
	game->drawnObjectsCount += _2d_objects.size();

	sort(transparent_objects.begin(), transparent_objects.end(), sortTransparent);

	for (unsigned int i = 0; i < lights.size(); i++ ) {
		lights.at(i)->draw();
	}

	for (unsigned int i = 0; i < opaque_objects.size(); i++ ) {
		opaque_objects.at(i)->draw();
		//writeDebugText("Type: %i, id: %s, z:%f", opaque_objects.at(i)->type, opaque_objects.at(i)->id.c_str(), opaque_objects.at(i)->position.z);
	}

	game->transparentObjectsCount = transparent_objects.size();
	for (unsigned int i = 0; i < transparent_objects.size(); i++ ) {
		transparent_objects.at(i)->draw();
		//writeDebugText("Type: %i, id: %s, z:%f", transparent_objects.at(i)->type, transparent_objects.at(i)->id.c_str(), (transparent_objects.at(i)->position - game->camera->position).Length());
	}

	if (game->debugPhysDraw) {
		if (game->debugPhysDrawIn2D)
			start2D();
			game->dynamicsWorld->debugDrawWorld();
		if (game->debugPhysDrawIn2D)
			end2D();
	}

	// Ligt reset
	for (unsigned int i = 0; i < lights.size(); i++ ) {
		lights.at(i)->cancel();
	}

#ifndef OPENGL_ES
	// for drawibg texts
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
#endif
	// save previous state of lighting
	bool lightingEnabled = glIsEnabled(GL_LIGHTING);
	glDisable(GL_LIGHTING);

	// Write 2D stuff
	start2D();

	// sort Z
	sort(_2d_objects.begin(), _2d_objects.end(), sort2D);

	game->_2dObjectsCount = _2d_objects.size();
	for (unsigned int i = 0; i < _2d_objects.size(); i++ ) {
		_2d_objects.at(i)->draw();
		//writeDebugText("Type: %i, id: %s, z:%f", opaque_objects.at(i)->type, opaque_objects.at(i)->id.c_str(), opaque_objects.at(i)->position.z);
	}

	// Draw debug texts
	for (unsigned int i = 0; i < debugStrings.size(); i++ ) {
		if (game->drawDebugText && !game->console->isOpened()) {
			std::pair <float, float>* size = new std::pair <float, float>();
			standart14Normal->GetStringSize(debugStrings.at(i).c_str(), size);

			static GOPlate* p = new GOPlate();
			p->setPositioningMode(GOPositioningModeTopLeft);
			p->color = MGColor(0.0f,0.0f,0.0f,0.4f);

			int h = (int)(size->second * 1.00);
			p->setSize(CVector(size->first, h));
			p->setPosition(CVector(5.0f, game->osAdapter->getScreenHeight() - (h * i)));
			p->draw();
			writeText(debugStrings.at(i), CVector(5.0f, game->osAdapter->getScreenHeight() - (h * i), 0.0f), STANDART_14_NORMAL, MGColor(0.0f, 1.0f, 0.0f, 1.0f));
		}
	}
	debugStrings.clear();

	game->luaVM->doString("_onDraw2D()");
	end2D();

	if (lightingEnabled) {
		glEnable(GL_LIGHTING);
	} else {
		glDisable(GL_LIGHTING);
	}

}


void GraphicEngine::endRender() {
	glFlush(); // Досрочное завершение рисования (ф-я необязательна)

	if (needSaveScreenShot) {
		if(!saveScreenshot())
			Log::warning("error save screenshot");
		needSaveScreenShot = false;
	}

	GLenum err = glGetError ();
	if (GL_NO_ERROR != err)
		Log::error("GraphicEngine::endRender(): OpenGL Error: %s", gluErrorString (err));
}


int GraphicEngine::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("writeDebugText")) {
		writeDebugText(lua_tostring(luaVM, -1));
		return 0;
	}
	if (isCurrentMethod("writeText")) {
		writeText(lua_tostring(luaVM, 1), CVector(lua_tointeger(luaVM, 2), lua_tointeger(luaVM, 3), 0), STANDART_14_NORMAL, MGColor::blueColor());
		return 0;
	}

	return LuaBridge::methodsBridge(luaVM);
}


