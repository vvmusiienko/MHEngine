#include <math.h>
#include "Game.h"
#include "object/GOCube.h"
#include "object/GOPlate.h"
#include "object/GOModel.h"
#include "object/GOLight.h"
#include "object/GOSound.h"
#include "object/GOPlayer.h"
#include "object/GOSkyBox.h"
#include "Tests.h"
#include "object/ui/UIButton.h"
#include "object/ui/UILabel.h"
#include "FontCache.h"
#include "SoundCache.h"
#include "AnimatedTexture.h"
#include "Atlas.h"
#include "AtlasTexture.h"
#include <btBulletDynamicsCommon.h>
#include "phys/PhysicObject.h"
#include "object/animation/MHPositionAnimation.h"
#include "object/animation/MHAlphaAnimation.h"
#include "object/animation/MHAnimationQueue.h"
#include "phys/constraint/GOConstraint.h"
#include "Camera.h"
#include "GraphicEngine.h"
#include "Console.h"
#include "MGConfig.h"
#include "MapManager.h"
#include "MapProperties.h"
#include "Log.h"


Game* GameObject::game;
unsigned int GameObject::glNameCounter;
Game* Game::instance;
GameObject* GameObject::fParentID;


class PhysDebugDrawer: public btIDebugDraw
{
	int debugMode;

	void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location,const char* textString);
	void setDebugMode(int debugMode);
	int	getDebugMode() const;
};


void PhysDebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& color) {
#ifndef OPENGL_ES
	static bool lightingEnabled;
	lightingEnabled = Game::instance->graphicEngine->isLightingEnabled();
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glLineWidth(2);
	glColor3f(color.x(), color.y(), color.z());
	glBegin(GL_LINES);
		glVertex3d(from.getX() * 100, from.getY() * 100, from.getZ() * 100);
		glVertex3d(to.getX() * 100, to.getY() * 100, to.getZ() * 100);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	Game::instance->graphicEngine->enableLighting(lightingEnabled);
#else //friken shit
	static bool lightingEnabled;
	lightingEnabled = Game::instance->graphicEngine->isLightingEnabled();
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glColor4f(color.x(), color.y(), color.z(), 1.0f);
    float vertices[9];
    vertices[0] = from.getX() * 100;
    vertices[1] = from.getY() * 100;
    vertices[2] = from.getZ() * 100;
    vertices[3] = from.getX() * 100;
    vertices[4] = from.getY() * 100 * 1.01;
    vertices[5] = from.getZ() * 100 * 0.999;;
    vertices[6] = to.getX() * 100;
    vertices[7] = to.getY() * 100;
    vertices[8] = to.getZ() * 100;
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_TEXTURE_2D);
	Game::instance->graphicEngine->enableLighting(lightingEnabled);
#endif	//#ifndef OPENGL_ES
}


void PhysDebugDrawer::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) {

}
void PhysDebugDrawer::reportErrorWarning(const char* warningString) {

}
void PhysDebugDrawer::draw3dText(const btVector3& location,const char* textString) {

}
void PhysDebugDrawer::setDebugMode(int debugMode) {
	this->debugMode = debugMode;
}
int	PhysDebugDrawer::getDebugMode() const {
	return this->debugMode;
}


struct TwoCollidedObjects {
	GameObject* go0;
	GameObject* go1;
};

static bool customContactAddedCallback(btManifoldPoint& cp,	const btCollisionObject* colObj0,int partId0,int index0,const btCollisionObject* colObj1,int partId1,int index1)
{
	GameObject* go0 = (GameObject*)colObj0->getUserPointer();
	GameObject* go1 = (GameObject*)colObj1->getUserPointer();

	if (! go0->isHaveContactWith(go1)) {
		go0->currentContacts.push_back(go1);
		ContactInfo* ci = new ContactInfo();
		ci->contactPointA = CVector(cp.m_localPointA.x() * 100.0f, cp.m_localPointA.y() * 100.0f, cp.m_localPointA.z() * 100.0f);
		ci->contactPointB = CVector(cp.m_localPointB.x() * 100.0f, cp.m_localPointB.y() * 100.0f, cp.m_localPointB.z() * 100.0f);
		go0->onContactOccured(go1, ci);
		delete ci;
	}
	if (!go1->isHaveContactWith(go0)) {
		go1->currentContacts.push_back(go0);
		ContactInfo* ci = new ContactInfo();
		ci->contactPointB = CVector(cp.m_localPointA.x() * 100.0f, cp.m_localPointA.y() * 100.0f, cp.m_localPointA.z() * 100.0f);
		ci->contactPointA = CVector(cp.m_localPointB.x() * 100.0f, cp.m_localPointB.y() * 100.0f, cp.m_localPointB.z() * 100.0f);
		go1->onContactOccured(go0, ci);
		delete ci;
	}

	TwoCollidedObjects* co = new TwoCollidedObjects;
	co->go0 = go0;
	co->go1 = go1;
	cp.m_userPersistentData = co;

	return false;
}


static bool customContactDestroyedCallback (void* userPersistentData) {
	GameObject* go0 = ((TwoCollidedObjects*)userPersistentData)->go0;
	GameObject* go1 = ((TwoCollidedObjects*)userPersistentData)->go1;

	if (go0 && go0->isHaveContactWith(go1)) {
		go0->removeContactWith(go1);
		go0->onContactLost(go1, NULL);
	}
	if (go1 && go1->isHaveContactWith(go0)) {
		go1->removeContactWith(go0);
		go1->onContactLost(go0, NULL);
	}

	delete ((TwoCollidedObjects*)userPersistentData);
	return false;
}


static void tickCallback(btDynamicsWorld *world, btScalar timeStep) {
	vector<GameObject*> objects = Game::instance->objects;
	for (unsigned int i = 0; i < objects.size(); i++ ) {
    	static GameObject* go;
    	go = objects.at(i);
    	if(go->getNeedUpdate() && !go->isDead() && go->type != GO_SKY_BOX) {
    		if (go->isEnabledPhysics()) {
        		go->onPhysicTick(1.0f / timeStep);
    		}
    	}
	}
}



Game::Game(IOSAdapter* osAdapter) {
	startTime = osAdapter->getTickCount();
	instance = this;
	console = NULL;
	// Init log system
	this->osAdapter = osAdapter;
	new Log(this); // Hm...
	luaVM = new LuaVM(this);
	LuaBridge::initStatic(luaVM);
	config = new MGConfig(this);
	console = new Console(this);
	graphicEngine = new GraphicEngine(this);
	mapManager = new MapManager(this);
	
	GameObject::initStatic(this);

	/*
	 * INIT PHYS ENGINE
	 */

    // Build the broadphase
    broadphase = new btDbvtBroadphase();

    // Set up the collision configuration and dispatcher
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    // The actual physics solver
    solver = new btSequentialImpulseConstraintSolver;

    // The world.
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
    dynamicsWorld->setDebugDrawer(new PhysDebugDrawer());
    dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawConstraints+btIDebugDraw::DBG_DrawConstraintLimits);
    // ps: setting gravity in init part
    dynamicsWorld->setForceUpdateAllAabbs(false);

    gContactAddedCallback = customContactAddedCallback;
    gContactDestroyedCallback = customContactDestroyedCallback;
    dynamicsWorld->setInternalTickCallback(tickCallback);

	needSaveToMap = false;
	gameStoped = false;
	needRenderWorld = true;
	needRenderMenu = true;
	needRenderConsole = false;
	memset(&keys, 0, sizeof keys);
	memset(&mouseState, 0, sizeof mouseState);
	framesPassed = 0;
	fps = 0;
	msFromLast1000ms = osAdapter->getTickCount(); 
	msFromLast250ms = osAdapter->getTickCount();
	throtle = 20000000;
	throtle = 20000000;
	srand(osAdapter->getTickCount());
	totalR = 0;
	totalU = 0;
	totalU = 0;
	totalR = 0;
	player = NULL;
	map = new MapProperties(this);
	passUpdate = false;
	editorMode = false;
	skyBox = NULL;
	maxDT = 5.0f;
	iterations = 3;
	debugLabels = false;
	debugPhysDraw = false;
	debugPhysDrawIn2D = false;
	needCheckCursorOver = false;
	for (int i = 0; i < ACTIVE_CURSORS_COUNT; i ++)
		activeCursors[i] = NULL;
	needTmpIDWhileSaving = false;
    physIsDisabled = false;
    textureQuality = 1;
    mipmapEnabled = true;
}


bool Game::needStop() {
	return gameStoped;
}


void Game::stopGame() {
	gameStoped = true;
}


// SOUND
void Game::setListenerPosition(CVector v) {
	ALfloat listenerPos[] = { v.x, v.y, v.z };
    alListenerfv(AL_POSITION, listenerPos);
}


void Game::setListenerOrientation(CVector at, CVector up) {
    // Orientation of the listener. (first 3 elements are "at", second 3 are "up")
	ALfloat listenerOri[] = { at.x, at.y, at.z,  up.x, up.y, up.z};
    alListenerfv(AL_ORIENTATION, listenerOri);
}


void Game::initialize() {
	Log::info("init game engine...");

	registrateMethods("game");
	luaVM->init();
	console->init();
	graphicEngine->initGL();
	camera = new Camera(this);
	// ++++ init sound
	if (alutInit(NULL, 0) != AL_TRUE)
		Log::warning("Failed initialize sound system");
	alGetError();
	ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
    alListenerfv(AL_VELOCITY,    listenerVel);
	setListenerPosition(CVector(0.0f, 0.0f, 0.0f));
	setListenerOrientation(CVector(0.0, 0.0, -1.0), CVector(0.0, 1.0, 0.0));

	physIterationsPerSecond = config->getFloat("physIterationsPerSecond");
	maxPhysSubsteps = config->getInt("maxPhysSubsteps");

	iterations = config->getInt("iterations");
	luaVM->registerProperty("%siterations", MG_INT, &iterations);

	maxDT = config->getFloat("max_dt");
	luaVM->registerProperty("%smax_dt", MG_FLOAT, &maxDT);

	drawBBoxes = config->getInt("drawBBoxes");
	luaVM->registerProperty("%sdrawBBoxes", MG_INT, &drawBBoxes);

	debugLabels = config->getInt("debug_labels");
	luaVM->registerProperty("%sdebug_labels", MG_INT, &debugLabels);

	gravity = config->getVector("gravity");
    dynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
	luaVM->registerProperty("%sgravity", MG_VECTOR, &gravity);

	airResist = config->getVector("air_resist");
	luaVM->registerProperty("%sair_resist", MG_VECTOR, &airResist);

	debugPhysDrawIn2D = config->getInt("debugPhysDrawIn2D");
	debugPhysDraw = config->getInt("debugPhysDraw");
	luaVM->registerProperty("%sdebugPhysDraw", MG_BOOL, &debugPhysDraw);
    
    
	luaVM->registerProperty("%sthrotle", MG_INT, &throtle);
	luaVM->registerProperty("%spassUpdate", MG_BOOL, &passUpdate);
	luaVM->registerProperty("%seditorMode", MG_BOOL, &editorMode);


	camera->cameraFollowsPlayer = config->getInt("camera_follows_player");

	drawDebugText = config->getInt("draw_debug_text");

	mouseState.hAngle = -acos(camera->lookAt.x);
	mouseState.vAngle = -atan(camera->lookAt.y);
	luaVM->doString("_onGameInitialized()");
	Log::info("done init game engine");



	/*UILabel* l1 = new UILabel();
	l1->setPosition(CVector(10, 200));
	l1->setGradientColor(MGColor(0.0f, 1.0f, 0.0f, 1.0f), MGColor(0.0f, 0.0f, 1.0f, 1.0f));
	l1->setFontSize(50.0f);
	l1->setText("MHEngine");
	l1->addToWorld();
	l1->setAlpha(0.0f);

	MHPositionAnimation* a = new MHPositionAnimation(CVector(10, 200), 1.0f, true);
	a->startAfter(2);
	l1->addAnimation(a);
	MHAlphaAnimation* a2 = new MHAlphaAnimation(0.0f, 1.0f);
	a2->startAfter(2);
	l1->addAnimation(a2);
	l1->addAnimation(new MHPositionAnimation(CVector(100, 200), 1.0f));
	l1->addAnimation(new MHAlphaAnimation(1.0f, 1.0f));



	l1 = new UILabel();
	l1->setPosition(CVector(250, 150));
	l1->setGradientColor(MGColor(0.0f, 1.0f, 0.0f, 1.0f), MGColor(0.0f, 0.0f, 1.0f, 1.0f));
	l1->setFontSize(50.0f);
	l1->setText("by MuHAOS.");
	l1->addToWorld();
	l1->setAlpha(0.0f);

	a = new MHPositionAnimation(CVector(250, 150), 1.0f, true);
	a->startAfter(2);
	l1->addAnimation(a);
	a2 = new MHAlphaAnimation(0.0f, 1.0f);
	a2->startAfter(2);
	l1->addAnimation(a2);
	l1->addAnimation(new MHPositionAnimation(CVector(100, 150), 1.0f));
	l1->addAnimation(new MHAlphaAnimation(1.0f, 1.0f));

	GOModel* m = new GOModel();
	m->setMass(0);
	m->setModel("box.md2");
	m->setTexture("box.tga");
	m->setPosition(CVector(0,100,0));
	m->setRepeat(CVector(1,-3,2));
	m->addToWorld();*/
}


void Game::call1000ms() {
	fps = framesPassed * 1;
	avgU = (unsigned long int)(totalU / (float)framesPassed);
	avgR = (unsigned long int)(totalR / (float)framesPassed);
	totalU = 0;
	totalR = 0;
	framesPassed = 0;
}


void Game::call250ms() {
}


void Game::deleteDeadObjects() {
    for (unsigned int i = 0; i < objects.size(); i++ ) {
    	static GameObject* go;
    	go = objects.at(i);
    	if (go->isDead()) {
    		//delete go;
    		go->onWillRemoveFromWorld();
    		go->removePhysObjectFromSimulation();
    		objects.erase(objects.begin() + i);
    		// remove from childs
    		vector<GameObject*> c;
    		for (unsigned int j = 0; j < go->getChilds().size(); j++) {
    			c.push_back(go->getChilds().at(j));
    		}
    		for (unsigned int j = 0; j < c.size(); j++) {
    			go->removeChild(c.at(j));
    		}
    		go->removeFromParent();
    		delete go;
    		i = i-1;
    	}
	}
}


void Game::iterate(float dt, int maxSubSteps, float fixedTimeStep) {
	if (gameStoped) return;
	if (dt > maxDT)
		dt = maxDT;
	
	this->dt = dt;
	this->fixedTimeStep = fixedTimeStep;
	this->maxSubSteps = maxSubSteps;
	
	framesPassed ++;

	/*for (int f = 0; f < 9500000; f++) {
		sqrt(1000000);
	}*/

	// for game timers
	static int currentTicCount;
	currentTicCount = osAdapter->getTickCount();

	if (currentTicCount - msFromLast1000ms >= 1000) {
		call1000ms();
		msFromLast1000ms = currentTicCount;
	}
	if (currentTicCount - msFromLast250ms >= 250) {
		call250ms();	
		msFromLast250ms = currentTicCount;
	}

	
	graphicEngine->writeDebugText("FPS: %i;  U: %i;  R: %i;  DT: %f(%f);", fps, avgU, avgR, dt, dt / (float)iterations);
	// Other
	graphicEngine->writeDebugText("BUFFERS REFS: %i;", totalBufferRefreshCount);
	// showing count of visual objects
	graphicEngine->writeDebugText("Objects: %i (2D: %i; TRANS: %i; ENABLED: %i; DRAWN: %i)", objects.size(), _2dObjectsCount, transparentObjectsCount, enabledObjectsCount, drawnObjectsCount);
	totalBufferRefreshCount = 0;



	// +++ ANIMATING AND PHYSICS +++
	if (keys[KEY_RIGHT]) {
		if (player) player->moveRight();
	}

	if (keys[KEY_LEFT]) {
		if (player) player->moveLeft();
	}

	if (keys[KEY_UP]) {
		if (player) player->jump();
	}
	
	static unsigned long int beforeBench = 0;
	
	beforeBench = osAdapter->getTickCount();

	graphicEngine->blockDebugTexts = true;
    for (int x = 0; x < iterations; x++) {
    	if (x == iterations-1)
    	    graphicEngine->blockDebugTexts = false;

    	enabledObjectsCount = 0;
        for (unsigned int i = 0; i < objects.size(); i++ ) {
        	static GameObject* go;
        	go = objects.at(i);
        	if(!passUpdate && go->getNeedUpdate() && !go->isDead() && go->type != GO_SKY_BOX) {
        		if (go->isEnabledPhysics() && go->isNeedRefreshCollisionShape()) {
        			go->makeCollisionShape();
        		}
        		go->update(dt / (float)iterations);
        		enabledObjectsCount ++;
        	}
    	}
        deleteDeadObjects();

    	luaVM->doString(FormatString("_onIterate(%f)", dt / (float)iterations));
    }

    // +++ PHYS +++
    if (!editorMode && !physIsDisabled)
    	dynamicsWorld->stepSimulation(dt / 30.f, maxPhysSubsteps, 1.f / physIterationsPerSecond);
    // First need update player then camera and then sky box. It's need for avoid fuckin camera shaking
	camera->update(dt);
    if(skyBox) skyBox->update(dt);

    // +++ CONSTRAINTS MANAGEMENT
	// remove from list
	vector<GOConstraint*>::iterator p = constraints.begin();
	for (; p < constraints.end(); p++) {
		if ((*p)->isNeedRecreateConstraint()) {
			(*p)->createConstraint();
		}
	}


    totalU += osAdapter->getTickCount() - beforeBench;
    beforeBench = osAdapter->getTickCount();

	// +++ RENDERING +++

	graphicEngine->beginRender();

	
	if (needRenderWorld)
	{
		graphicEngine->renderWorld();
		GameObject::glNameCounter = 0;
	}

	

	if (needRenderMenu)
	{
		
	}
	
	if (console->isOpened())
	{
		console->draw();
	}
	graphicEngine->endRender();

	totalR += osAdapter->getTickCount() - beforeBench;

	osAdapter->swapBuffers();

	//cursor over
	if (needCheckCursorOver) {
		for (unsigned int i = 0; i < responders.size(); i++) {
			UIResponder* r = responders.at(i);
			bool hasCursorOver = false;
			if (r->getResponsible()) {
				for (int j = 0; j < 10; j++) {
					if(r->cursorsOver[j]) {
						hasCursorOver = true;
						break;
					}
				}
				if (!hasCursorOver) {
					if (r->isSomeCursorOver) {
						r->cursorLeave(mouseState);
						r->isSomeCursorOver = false;
					}
				}
			}
		}

		needCheckCursorOver = false;
	}

}


void Game::setTextureQuality(int q) {
    textureQuality = q;
}


int Game::getTextureQuality() {
    return textureQuality;
}


void Game::setMipmapEnabled(bool v) {
    mipmapEnabled = v;
}


bool Game::getMipmapEnabled() {
    return mipmapEnabled;
}


void Game::disablePhys(bool v) {
    physIsDisabled = v;
}


bool Game::isPhysDisabled() {
    return physIsDisabled;
}


void Game::startProfilerZone() {
	lastCallTime = osAdapter->getTickCount();
}
// QWE
int Game::endProfilerZone() {
	int timePassed = osAdapter->getTickCount() - lastCallTime;
	lastCallTime = osAdapter->getTickCount();
	return timePassed;
}


void Game::updateScreenSize() {
	graphicEngine->updateScreenSize();
}


bool Game::sortResponders(UIResponder* a, UIResponder* b) {
	return a->responseOrder() < b->responseOrder();
}


void Game::calculateCursorsOverResponders(CursorState c, int cursorNum) {
	// cursor over handling
	for (unsigned int i = 0; i < responders.size(); i++) {
		UIResponder* r = responders.at(i);
		if (r->getResponsible()) {
			if (r->hitTest(c)) {
				if (!r->isSomeCursorOver) {
					r->cursorOver(c);
					r->isSomeCursorOver = true;
				}
				r->cursorsOver[cursorNum] = true;
			} else {
				r->cursorsOver[cursorNum] = false;
			}
		}
	}
	needCheckCursorOver = true;
}


void Game::cursorDown(CursorButton button, int cursorNum)
{

	switch (button)
	{
		case LEFT:
			mouseState.lButton = true;
			luaVM->doString(FormatString("_onMouseDown(%i)", 0));
			break;
		case MIDDLE:
			mouseState.mButton = true;
			luaVM->doString(FormatString("_onMouseDown(%i)", 1));
			break;
		case RIGHT:
			mouseState.rButton = true;
			luaVM->doString(FormatString("_onMouseDown(%i)", 2));
			break;
	}

	static vector<UIResponder*> respondCandidates;
	respondCandidates.clear();
	CursorState c = mouseState;
	c.y = osAdapter->getScreenHeight() - c.y;
	for (unsigned int i = 0; i < responders.size(); i++) {
		UIResponder* r = responders.at(i);
		if (r->getResponsible() && r->hitTest(c) && r->isDownByCursor == -1) {
			respondCandidates.push_back(r);
		}
	}
	sort(respondCandidates.begin(), respondCandidates.end(), sortResponders);
	if (respondCandidates.size() >= 1) {
		UIResponder* firstResponder = respondCandidates.at(respondCandidates.size()-1);
		firstResponder->cursorDown(c);
		firstResponder->isDownByCursor = cursorNum;
		firstResponders.push_back(firstResponder);
	}

	calculateCursorsOverResponders(c, cursorNum);
}


void Game::cursorMoved(float x, float y, int cursorNum)
{
	mouseState.x = x;
	mouseState.y = y;

	mouseState.dx = x - mouseState.lastX;
	mouseState.dy = y - mouseState.lastY;

	mouseState.lastX = x;
	mouseState.lastY = y;


    if (mouseState.mButton)
    {
        mouseState.hAngle += (PI * 0.0015) * mouseState.dx;
        mouseState.vAngle += (PI * 0.0015) * mouseState.dy;

        camera->lookAt.x = cos(mouseState.hAngle);
        camera->lookAt.y = -tan(mouseState.vAngle);
        camera->lookAt.z = sin(mouseState.hAngle);
    }

	luaVM->doString(FormatString("_onMouseMoved(%f, %f)", x, y));

	CursorState c = mouseState;
	c.y = osAdapter->getScreenHeight() - c.y;
	for (unsigned int i = 0; i < firstResponders.size(); i++) {
		UIResponder* firstResponder = firstResponders.at(i);
		if (firstResponder->isDownByCursor == cursorNum && firstResponder->getResponsible()) {
			firstResponder->cursorMoved(c);
			break;
		}
	}
	calculateCursorsOverResponders(c, cursorNum);
}


void Game::cursorUp(CursorButton button, int cursorNum)
{
	switch (button)
	{
		case LEFT:
			mouseState.lButton = false;
			luaVM->doString(FormatString("_onMouseUp(%i)", 0));
			break;
		case MIDDLE:
			mouseState.mButton = false;
			luaVM->doString(FormatString("_onMouseUp(%i)", 1));
			break;
		case RIGHT:
			mouseState.rButton = false;
			luaVM->doString(FormatString("_onMouseUp(%i)", 2));
			break;
	}

	for (unsigned int i = 0; i < firstResponders.size(); i++) {
		UIResponder* firstResponder = firstResponders.at(i);
		if (firstResponder->isDownByCursor == cursorNum && firstResponder->getResponsible()) {
			CursorState c = mouseState;
			c.y = osAdapter->getScreenHeight() - c.y;
			firstResponder->cursorUp(c);
			firstResponder->isDownByCursor = -1;
			firstResponders.erase(firstResponders.begin() + i);
			break;
		}
	}

	for (unsigned int i = 0; i < responders.size(); i++) {
		UIResponder* r = responders.at(i);
		if (r->getResponsible()) {
			r->cursorsOver[cursorNum] = false;
		}
	}
	needCheckCursorOver = true;
}


void Game::cursorWheelMoved(float dx, float dy, int cursorNum)
{
	
}



// KEY ADAPTER
void Game::keyDown(unsigned char key)
{
	if (console->isOpened()) {
		console->onKeyDown(key);
		return;
	}

	keys[key] = true;
	
	switch (key) {
		case 123:
			graphicEngine->getScreenshot();
			break;
		case '1':
			graphicEngine->toggleWhireFrame();
			break;
		case '2':
			drawDebugText = !drawDebugText;
			config->setInt("draw_debug_text", drawDebugText);
			break;
		case '3':
			drawBBoxes = !drawBBoxes;
			config->setInt("drawBBoxes", drawBBoxes);
			break;
		case '4':
			if (glIsEnabled(GL_LIGHTING)){
				glDisable (GL_LIGHTING);
			} else {
				glEnable (GL_LIGHTING);
			}
			break;
		case KEY_UP:
			break;
		case KEY_DOWN:
			break;
		case KEY_RIGHT:
			break;
		case KEY_LEFT:
			break;
		case 107:
			//plate_s->position.y = plate_s->position.y + 1;
			break;
		case 109:
			//plate_s->position.y = plate_s->position.y - 1;
			break;
		case KEY_TILD:
			break;

	}
}

void Game::keyUp(unsigned char key)
{

	if (console->isOpened()) {
		console->onKeyUp(key);
		return;
	}


	keys[key] = false;

	switch (key) {
		case KEY_RIGHT:
		case KEY_LEFT:
			if (player) player->stopRun();
			break;
		case 'F':
			//cnst->;
			break;

		case 'R':
			mapManager->restart();
			break;
		case 'C':
			camera->cameraFollowsPlayer = !camera->cameraFollowsPlayer;
			config->setInt("camera_follows_player", camera->cameraFollowsPlayer);
			break;
	}
}

void Game::charEntered(unsigned char c)
{
	if (console->isOpened()) {
		console->onCharEntered(c);
		return;
	}


	if (c == '`') {
		console->open();
	}
}

void Game::finalize()
{
	config->saveConfig();
	delete graphicEngine;

    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
}


int Game::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("getResourcePath")) {
		lua_pushstring(luaVM, osAdapter->getResourcePath().c_str());
		return 1;
	}
	if (isCurrentMethod("getScreenWidth")) {
		lua_pushinteger(luaVM, osAdapter->getScreenWidth());
		return 1;
	}
	if (isCurrentMethod("getScreenHeight")) {
		lua_pushinteger(luaVM, osAdapter->getScreenHeight());
		return 1;
	}
	if (isCurrentMethod("getObjectAtPoint")) {
		float x = lua_tonumber(luaVM, 1);
		float y = lua_tonumber(luaVM, 2);
		GameObject* go = graphicEngine->getObjectAtPoint((int)x, (int)y);
		if (go) {
			string tmpName = "tmp_" + generateRandomString(10);
			Game::instance->luaVM->doString(FormatString("%s = %s", tmpName.c_str(), go->getFullID().c_str()));
			lua_getglobal(luaVM, tmpName.c_str());
		} else {
			lua_pushnil(luaVM);
		}
		return 1;
	}

	return LuaBridge::methodsBridge(luaVM);
}


Game::~Game()
{
}
