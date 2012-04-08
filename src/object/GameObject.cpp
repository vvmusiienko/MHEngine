#include "GameObject.h"
#include <stdexcept>
#include "./animation/MHAnimation.h"
#include "../GraphicEngine.h"
#include "../MapManager.h"
#include "../Camera.h"

GameObject::GameObject() {
	id = "";
	luaBinded = false;
	init();
}


GameObject::GameObject(string id) {
	this->id = id;
	luaBinded = true;
	init();
	bindLua();
	setObjectID(id);
	PhysicObject::bindLua();
}


GameObject::GameObject(FILE* f) {
	try {
		luaBinded = true;
		init();
		id = readString(f);
		if (Game::instance->firstObjectID != "") { // need override ID
			Game::instance->oldFirstObjectID = id;
			id = Game::instance->firstObjectID;
			Game::instance->firstObjectID = "";
			Game::instance->luaVM->doString(FormatString("%s = {} %s = %s", id.c_str(), Game::instance->oldFirstObjectID.c_str(), id.c_str()));
		}
		if (fParentID) {
			parentID = fParentID->getFullID();
			fParentID->addChild(this);
		}
		setEnabled(readChar(f));
		setHidden(readChar(f));
		opaque = readChar(f);
		color = readColor(f);
		setPosition(readVector(f));
		setOrientation(readVector(f));
		setScale(readVector(f));
		lightingEnabled = readChar(f);
		setAlpha(readFloat(f));
		loadPhysProperties(f);
		// Map Mask

		bindLua();
		setObjectID(getFullID());
		PhysicObject::bindLua();

		// Read count of objects
		int count = readInt(f);
		for (int i = 0; i < count; i++) {
			// read type
			unsigned char type = readChar(f);
			Game::instance->mapManager->createObject(f, type, this);
		}

	} catch (std::runtime_error e) {
		throw std::runtime_error(FormatString("GameObject::GameObject(FILE* f): Can't create object from stream! Reason:\n%s", e.what()));
	}
}


void GameObject::init() {
	type = GO_UNDEFINED;
	dead = false;
	positioningMode = GOPositioningModeCenter;
	needSaveToMap = game->needSaveToMap;
	opaque = true;
	orientation = CVector(0,0,0);
	enabled = true;
	hidden = false;
	needRefreshBuffers = true;
	is2D = false;
	loadedFromMap = false;
	needDeath = false;
	glName = 0;
	needDraw = true;
	parent = NULL;
	needUpdate = true;
	alpha = 1.0f;
	hasCustomBlendFunction = false;
	needRelativeOrientation = false;
	VBOBuffer = 0;
	scale = CVector(1.0f, 1.0f, 1.0f);
	lightingEnabled = true;
	useGradOrientation = true;
}


void GameObject::initStatic(Game* g) {
	game = g; // set game instance for all inherit classes
	glNameCounter = 0;
	fParentID = NULL;
}


void GameObject::save(FILE* f) {
	writeChar(type, f);
	writeString(id, f);
	writeChar(enabled, f);
	writeChar(hidden, f);
	writeChar(opaque, f);
	writeColor(color, f);
	writeVector(getPosition(), f);
	writeVector(getOrientation(), f);
	writeVector(getScale(), f);
	writeChar(lightingEnabled, f);
	writeFloat(getAlpha(), f);
	savePhysProperties(f);

	// save childs
	int count = 0;
	for (unsigned int i = 0; i < childs.size(); i++) {
		GameObject* go = childs.at(i);
		if (go->needSaveToMap && go->id != "")
			count ++;
	}

	writeInt(count, f);
	for (unsigned int i = 0; i < childs.size(); i++) {
		GameObject* go = childs.at(i);
		if (go->needSaveToMap && go->id != "")
			go->save(f);
	}
}


GameObject* GameObject::objectCreated() {
	if (luaBinded)
		game->luaVM->doString(FormatString("se(%s.onCreate, %s)", getFullID().c_str(), getFullID().c_str()));
	return this;
}

void GameObject::setPositioningMode(GOPositioningMode v) {
	positioningMode = v;
}


GOPositioningMode GameObject::getPositioningMode() {
	return positioningMode;
}


CVector GameObject::getPositioningOffset(GOPositioningMode v, float width, float height) {
	CVector r;
	switch (v) {
	case GOPositioningModeTopLeft:
		r = CVector(0, -height);
		break;
	case GOPositioningModeTopCenter:
		r = CVector(-width / 2.0f, -height);
		break;
	case GOPositioningModeTopRight:
		r = CVector(-width, -height);
		break;
	case GOPositioningModeCenterLeft:
		r = CVector(0, -height / 2.0f);
		break;
	case GOPositioningModeCenter:
		r = CVector(-width / 2.0f, -height / 2.0f);
		break;
	case GOPositioningModeCenterRight:
		r = CVector(-width, -height / 2.0f);
		break;
	case GOPositioningModeBottomLeft:
		break;
	case GOPositioningModeBottomCenter:
		r = CVector(-width / 2.0f, 0.0f);
		break;
	case GOPositioningModeBottomRight:
		r = CVector(-width, 0.0f);
		break;
	}
	return r;
}


void GameObject::kill() {
	dead = true;
	for (unsigned int i = 0; i < getChilds().size(); i ++) {
		getChilds().at(i)->kill();
	}
}


void GameObject::addToWorld() {
	game->mapManager->addObject(this);
}


void GameObject::removeFromWorld() {
	// TODO: add functional
}


void GameObject::updateFullIDsForChilds() {
	for (unsigned int i = 0; i < childs.size(); i++) {
		GameObject* o = childs.at(i);
		o->parentID = getFullID();
		Game::instance->luaVM->doString(FormatString("%s.%s = %s", getFullID().c_str(), o->id.c_str(), o->getFullID().c_str()));
		o->updateFullIDsForChilds();
	}
}


string GameObject::getFullID() {
	if (getParent() != NULL) {
		return getParent()->getFullID() + "." + id;
	}
	return id;
}


void GameObject::addChild(GameObject* o) {
	string oldID = o->getFullID();
	o->parent = this;
	o->parentID = getFullID();
	if (luaBinded && o->id != "") {
		// lua do
		Game::instance->luaVM->doString(FormatString("%s.%s = %s", getFullID().c_str(), o->id.c_str(), oldID.c_str()));
		o->updateFullIDsForChilds();
	}
	childs.push_back(o);
}


vector<GameObject*> GameObject::getChilds() {
	return childs;
}


void GameObject::removeChild(GameObject* o) {
	int pos = -1;
	for (unsigned int i = 0; i < childs.size(); i++) {
		if (o == childs.at(i)) {
			pos = i;
			break;
		}
	}

	if (pos >= 0) {
		if (luaBinded && o->id != "") {
			// lua do
			Game::instance->luaVM->doString(FormatString("%s = %s", o->id.c_str(), o->getFullID().c_str()));
			o->updateFullIDsForChilds();
		}
		o->parent = NULL;
		o->parentID = "";
		childs.erase(childs.begin() + pos);
	}
}


void GameObject::removeFromParent() {
	if (getParent()) {
		getParent()->removeChild(this);
	}
}


bool GameObject::hasParent(GameObject* candidat) { // return true if candidat present on any level of hierarchy
	if (getParent() == NULL)
		return false;

	if (getParent() == candidat)
		return true;

	return getParent()->hasParent(candidat);
}


bool GameObject::isHaveContactWith(GameObject* o) {
	// iterate animations
	for (vector<GameObject*>::iterator i = currentContacts.begin(); i != currentContacts.end(); i++) {
		if ((*i) == o)
			return true;
	}
	return false;
}


void GameObject::removeContactWith(GameObject* o) {
	// removing finished animations
	for (vector<GameObject*>::iterator i = currentContacts.begin(); i != currentContacts.end(); i++) {
		if ((*i) == o) {
			currentContacts.erase(i);
			return;
		}
	}
}


void GameObject::setHidden(bool v) {
	hidden = v;
	setNeedDraw(! v);
}


bool GameObject::getHidden() {
	return hidden;
}


void GameObject::setNeedDraw(bool v) {
	needDraw = v;
	for (unsigned int i = 0; i < childs.size(); i++) {
		GameObject* o = childs.at(i);
		if (! o->getHidden()) {
			o->setNeedDraw(v);
		}
	}
}


bool GameObject::getNeedDraw() {
	if (getParent() == NULL)
		return needDraw;

	return (!getParent()->getNeedDraw()) ? false : needDraw;
}


GameObject* GameObject::getParent() {
	return parent;
}

void GameObject::setEnabled(bool v) {
	enabled = v;
	setNeedUpdate(v);
}


bool GameObject::getEnabled() {
	return enabled;
}


void GameObject::setNeedUpdate(bool v) {
	needUpdate = v;
	for (unsigned int i = 0; i < childs.size(); i++) {
		GameObject* o = childs.at(i);
		if (o->getEnabled()) {
			o->setNeedUpdate(v);
		}
	}
}


bool GameObject::getNeedUpdate() {
	return needUpdate;
}

// Alpha
void GameObject::setAlpha(float v) {
	alpha = v;
}


float GameObject::getAlpha() {
	return alpha;
}


float GameObject::getAbsoluteAlpha() {
	float r;
	if (getParent() == NULL) {
		r = alpha;
	} else {
		r = alpha - (1 - getParent()->getAbsoluteAlpha());
	}

	if (r < 0.0f)
		r = 0.0f;
	if (r > 1.0f)
		r = 1.0f;

	return r;
}


void GameObject::setPosition(CVector v) {
	if (isEnabledPhysics()) {
		setPhysPosition(v);
	}
	position = v;
}


CVector GameObject::getPosition() {
	if (isEnabledPhysics() && isPhysicsInitialized()) {
		return getPhysPosition();
	}

	return position;
}


void GameObject::setOrientation(CVector v) {
	useGradOrientation = true;
	if (isEnabledPhysics()) {
		setPhysOrientation(v);
	}
	orientation = v;
}


CVector GameObject::getOrientation() {
	if (isEnabledPhysics() && isPhysicsInitialized()) {
		return getPhysOrientation();
	}
	return orientation;
}


void GameObject::setOrientationM(CVector v) {
	useGradOrientation = false;
	orientationM = v;
}


CVector GameObject::getOrientationM() {
	return orientationM;
}


BBox GameObject::getBBox() {
	return BBox(Game::instance->camera->position,Game::instance->camera->position);
}


void GameObject::refreshBuffers() {
	Game::instance->totalBufferRefreshCount ++;
	///game->graphicEngine->writeDebugText(" %s", id.c_str());
}


CVector rotate(CVector p, float fx, float fy, float fz) {
	CVector x,y,z;
	z.x = p.x*cos(fz / 57.29578) - p.y*sin(fz / 57.29578);
	z.y = p.x*sin(fz / 57.29578) + p.y*cos(fz / 57.29578);
	z.z = p.z;
	// X rotation
	x.y = z.y*cos(fx / 57.29578) - z.z*sin(fx / 57.29578);
	x.z = z.y*sin(fx / 57.29578) + z.z*cos(fx / 57.29578);
	x.x = z.x;
	// Y rotation
	y.z += x.z*cos(fy / 57.29578) - x.x*sin(fy / 57.29578);
	y.x += x.z*sin(fy / 57.29578) + x.x*cos(fy / 57.29578);
	y.y += x.y;
	return y;
}


CVector GameObject::getAbsolutePosition() {
	if (getParent() == NULL)
		return getPosition();
	if (!needRelativeOrientation)
		return getParent()->getAbsolutePosition() + getPosition();
	return getParent()->getAbsolutePosition() + rotate(getPosition(), getParent()->getAbsoluteOrientation().x, getParent()->getAbsoluteOrientation().y, getParent()->getAbsoluteOrientation().z);
}


CVector GameObject::getAbsoluteOrientation() {
	if (getParent() == NULL)
		return orientation;
	return getParent()->getAbsoluteOrientation() + orientation;
}


float GameObject::computeAlpha() {
	float a = color.getA() -  (1 - getAbsoluteAlpha());
	if (a > 1.0f) a = 1.0f;
	if (a < 0.0f) a = 0.0f;
	return a;
}


void GameObject::setBlendFunc(GLenum s, GLenum d) {
	hasCustomBlendFunction = true;
	customBlendFuncS = s;
	customBlendFuncD = d;
}


void GameObject::setScale(CVector v) {
	scale = v;
	if (isEnabledPhysics()) {
		setCollisionShapeScale(scale);
	}
}


CVector GameObject::getScale() {
	return scale;
}


void GameObject::enableLighting(bool v) {
	lightingEnabled = v;
}


bool GameObject::isLightingEnabled() {
	return lightingEnabled;
}


void GameObject::draw() {
#ifndef OPENGL_ES
	if (id != "")
		glName = ++glNameCounter;
	if (glName != 0)
		glLoadName(glName);
#endif

	if (needRefreshBuffers) {
		refreshBuffers();
		needRefreshBuffers = false;
	}

	// blending
	if (hasCustomBlendFunction) {
		if (game->graphicEngine->getCurrentBlendFuncS() != customBlendFuncS || game->graphicEngine->getCurrentBlendFuncD() != customBlendFuncD )
			game->graphicEngine->setBlendFunc(customBlendFuncS, customBlendFuncD);
	} else {
		if (game->graphicEngine->getCurrentBlendFuncS() != game->graphicEngine->getDefaultBlendFuncS()
				|| game->graphicEngine->getCurrentBlendFuncD() != game->graphicEngine->getDefaultBlendFuncD())
		game->graphicEngine->setBlendFunc(game->graphicEngine->getDefaultBlendFuncS(), game->graphicEngine->getDefaultBlendFuncD());
	}

	// lighting
	if (!lightingEnabled) {
		if (Game::instance->graphicEngine->isLightingEnabled()) {
			Game::instance->graphicEngine->enableLighting(false);
		}
	} else {
		if (!Game::instance->graphicEngine->isLightingEnabled()) {
			Game::instance->graphicEngine->enableLighting(true);
		}
	}

	if (!isEnabledPhysics() || !isPhysicsInitialized()) {
		CVector p = getAbsolutePosition();
		if (useGradOrientation) {
			glTranslatef(p.x, p.y, p.z);
			glRotatef(getAbsoluteOrientation().x, 1.0, 0.0, 0.0);
			glRotatef(getAbsoluteOrientation().y, 0.0, 1.0, 0.0);
			glRotatef(getAbsoluteOrientation().z, 0.0, 0.0, 1.0);
		} else {
			static float* m = new float[16];

			CVector orientationM;
			orientationM = this->orientationM;
			orientationM.Normalize();

			Log::info("as %f %f %f", orientationM.x, orientationM.y, orientationM.z);
			CVector v1 = orientationM.CrossProduct(CVector(0,1,0));
			Log::info("%f %f %f", v1.x, v1.y, v1.z);

/*			CVector orientationM;
			orientationM.x = this->orientationM.x;
			orientationM.y = this->orientationM.y;
			orientationM.z = this->orientationM.z;

			orientationM.Normalize();

			// at entry: dir is direction you want(normalized!) m is 4x4 matrix
			CVector x_dir(1.0,0.0,0.0),y_dir;
			float d=orientationM.z;

			if(d>-0.999999999 && d<0.999999999){
				// to avoid problems with normalize in special cases
				x_dir=x_dir-orientationM*d;
				x_dir.Normalize();
				y_dir=orientationM.CrossProduct(x_dir);
			}else{
				x_dir=CVector(orientationM.z,0,-orientationM.x);
				y_dir=CVector(0,1,0);
			};
				// x_dir and y_dir is orthogonal to dir and to eachother.
				// so, you can make matrix from x_dir, y_dir, and dir in whatever way you prefer.
				// What to do depends to what API you use and where arrow model is pointing.
				// this is matrix i use which may give starting point.
				// this is for arrow that points in z direction (for arrow that points in x direction you may try swapping dir and x_dir)
/*				m[0][0]=x_dir.x;
				m[0][1]=x_dir.y;
				m[0][2]=x_dir.z;
				m[0][3]=0.0;
				m[1][0]=y_dir.x;
				m[1][1]=y_dir.y;
				m[1][2]=y_dir.z;
				m[1][3]=0.0;
				m[2][0]=dir.x;
				m[2][1]=dir.y;
				m[2][2]=dir.z;
				m[2][3]=0.0;
				m[3][0]=0;
				m[3][1]=0;
				m[3][2]=0;
				m[3][3]=1.0;
*/
/*
				m[0]=x_dir.x;
				m[1]=x_dir.y;
				m[2]=x_dir.z;
				m[3]=0.0;
				m[4]=y_dir.x;
				m[5]=y_dir.y;
				m[6]=y_dir.z;
				m[7]=0.0;
				m[8]=orientationM.x;
				m[9]=orientationM.y;
				m[10]=orientationM.z;
				m[11]=0.0;
				m[12]=0;
				m[13]=0;
				m[14]=0;
				m[15]=1.0;
*/

			m[0] = orientationM.x; m[4] = orientationM.y; m[8] = orientationM.z; m[12] = p.x;
			m[1] = 0; m[5] = 1;  m[9] = 0.0; m[13] = p.y;
			m[2] = v1.x; m[6] = v1.y; m[10] = v1.z; m[14] = p.z;
			m[3] = 0.0; m[7] = 0.0; m[11] = 0.0; m[15] = 1.0;

			glMultMatrixf(m);
		}
	} else {
		float* matrix = getTransformMatrix();
		// add parent offset
        if (getParent() != NULL) {
            CVector ap = getParent()->getAbsolutePosition(); 
            matrix[12] = matrix[12] + ap.x;
            matrix[13] = matrix[13] + ap.y;
            matrix[14] = matrix[14] + ap.z;
        }
        
        glMultMatrixf(matrix);
	}
	glScalef(scale.x, scale.y, scale.z);

	glColor4f(color.getR(), color.getG(), color.getB(),  computeAlpha());
}


void GameObject::update(float dt) {
	// removing finished animations
	for (vector<MHAnimation*>::iterator i = animations.begin(); i != animations.end();) {
		if ((*i)->isFinished()) {
			delete (*i);
			i = animations.erase(i);
		} else {
			++i;
		}
	}
    
	// iterate animations (iterating without iterattors for avoid bug if animations modified under iterations cycle.
    int count = animations.size();
	for (int i = 0; i < count; i++) {
		animations.at(i)->update(dt);
	}
}


void GameObject::addAnimation(MHAnimation* a) {
	// removing duplicated anims
	for (vector<MHAnimation*>::iterator i = animations.begin(); i != animations.end(); i++) {
		if (a->getAnimationType() == (*i)->getAnimationType()) {
			delete (*i);
			animations.erase(i);
			break;
		}
	}

	a->setOwner(this);
	animations.push_back(a);
}


void GameObject::onAddedToWorld() {
	if (luaBinded)
		game->luaVM->doString(FormatString("se(%s.onAddedToWorld, %s)", getFullID().c_str(), getFullID().c_str()));
}


void GameObject::onWillRemoveFromWorld() {
	if (luaBinded)
		game->luaVM->doString(FormatString("se(%s.onWillRemoveFromWorld, %s)", getFullID().c_str(), getFullID().c_str()));
}


void GameObject::onContactOccured(GameObject* target, ContactInfo* ci) {
	if (luaBinded)
		game->luaVM->doString(FormatString("se(%s.onContactOccured, %s, %s)", getFullID().c_str(), getFullID().c_str(), target->getFullID().c_str()));
}


void GameObject::onContactLost(GameObject* target, ContactInfo* ci) {
	if (luaBinded)
		game->luaVM->doString(FormatString("se(%s.onContactLost, %s, %s)", getFullID().c_str(), getFullID().c_str(), target->getFullID().c_str()));
}


bool GameObject::isDead() {
	return dead;
}


void GameObject::bindLua() {
	string id = getFullID();
	game->luaVM->doString("if "+id+" == nil then "+id+" = {} end");
	game->luaVM->doString(FormatString(id+".cpointer = %i", this));
	game->luaVM->registerProperty(id + ".%sid", MG_STRING, &this->id);
	game->luaVM->registerProperty(id + ".%stype", MG_INT, &this->type);
	game->luaVM->registerProperty(id + ".%sdead", MG_BOOL, &this->dead);
	game->luaVM->registerProperty(id + ".%sopaque", MG_BOOL, &this->opaque);
	game->luaVM->registerProperty(id + ".%scolor", MG_COLOR, &this->color);
	game->luaVM->registerProperty(id + ".%senabled", MG_BOOL, &this->enabled);
	game->luaVM->registerProperty(id + ".%sneedSaveToMap", MG_BOOL, &this->needSaveToMap);
	game->luaVM->registerProperty(id + ".%shidden", MG_BOOL, &this->hidden);
	game->luaVM->registerProperty(id + ".%sneedDeath", MG_BOOL, &this->needDeath);

	//registrateMethods(id);
}


int GameObject::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("getFullID")) {
		lua_pushstring(luaVM, getFullID().c_str());
		return 1;
	}
	if (isCurrentMethod("objectCreated")) {
		objectCreated();
		return 0;
	}
	if (isCurrentMethod("setPosition")) {
		setPosition(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getPosition")) {
		luaPushVector(luaVM, getPosition().x, getPosition().y, getPosition().z);
		return 1;
	}
	if (isCurrentMethod("setOrientation")) {
		setOrientation(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getOrientation")) {
		CVector o = getOrientation();
		luaPushVector(luaVM, o.x, o.y, o.z);
		return 1;
	}
	if (isCurrentMethod("setOrientationM")) {
		setOrientationM(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getOrientationM")) {
		CVector o = getOrientationM();
		luaPushVector(luaVM, o.x, o.y, o.z);
		return 1;
	}
	if (isCurrentMethod("setScale")) {
		setScale(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	if (isCurrentMethod("getScale")) {
		luaPushVector(luaVM, getScale().x, getScale().y, getScale().z);
		return 1;
	}
	if (isCurrentMethod("setHidden")) {
		setHidden(lua_toboolean(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("getHidden")) {
		lua_pushboolean(luaVM, getHidden());
		return 1;
	}
	if (isCurrentMethod("setAlpha")) {
		setAlpha(lua_tonumber(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("getAlpha")) {
		lua_pushnumber(luaVM, getAlpha());
		return 1;
	}
	if (isCurrentMethod("setEnabled")) {
		setEnabled(lua_toboolean(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("getEnabled")) {
		lua_pushboolean(luaVM, getEnabled());
		return 1;
	}
	if (isCurrentMethod("addChild")) {
		lua_pushstring(luaVM, "cpointer");
		lua_gettable(luaVM, -2);
		GameObject* o = (GameObject*)lua_tointeger(luaVM, -1);
		addChild(o);
		lua_pop(luaVM, -1);
		return 0;
	}
	if (isCurrentMethod("removeChild")) {
		lua_pushstring(luaVM, "cpointer");
		lua_gettable(luaVM, -2);
		GameObject* o = (GameObject*)lua_tointeger(luaVM, -1);
		removeChild(o);
		lua_pop(luaVM, -1);
		return 0;
	}
	if (isCurrentMethod("removeFromParent")) {
		removeFromParent();
		return 0;
	}
	if (isCurrentMethod("hasParent")) {
		lua_pushstring(luaVM, "cpointer");
		lua_gettable(luaVM, -2);
		GameObject* o = (GameObject*)lua_tointeger(luaVM, -1);
		lua_pushboolean(luaVM, hasParent(o));
		return 1;
	}
	if (isCurrentMethod("getChilds")) {
		lua_newtable(luaVM);
		int tableIndex = lua_gettop(luaVM);
		vector<GameObject*> objs;
		for (unsigned int i = 0; i < childs.size(); ++i) {
			if (childs.at(i)->id == "" || game->objects.at(i)->dead) continue;
			objs.push_back(childs.at(i));
		}
		for (unsigned int i = 0; i < objs.size(); ++i) {
			lua_pushinteger(luaVM, i+1);
			string tmpName = "tmp_" + generateRandomString(10);
			Game::instance->luaVM->doString(FormatString("%s = %s", tmpName.c_str(), objs.at(i)->getFullID().c_str()));
			lua_getglobal(luaVM, tmpName.c_str());
			lua_settable (luaVM, tableIndex);
		}
		return 1;
	}
	if (isCurrentMethod("kill")) {
		kill();
		return 0;
	}
	if (isCurrentMethod("enableLighting")) {
		enableLighting(lua_toboolean(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("isLightingEnabled")) {
		lua_pushboolean(luaVM, isLightingEnabled());
		return 1;
	}

	return PhysicObject::methodsBridge(luaVM);
}


GameObject::~GameObject() {

}
