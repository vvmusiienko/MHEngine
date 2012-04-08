#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include "../CVector.h"
#include "../Common.h"
#include <vector>
#include "../Game.h"
#include "../platform/opengl.h"
#include "../LuaBridge.h"
#include "../phys/PhysicObject.h"


// Game Objects

#define GO_CUBE					0
#define GO_GROUND				1
#define GO_LIGHT				2
#define GO_MODEL				3
#define GO_PLATE				4
#define GO_PLAYER				5
#define GO_CREATURE				6

// invisible and not iteratable objects (as usual config objects)
#define GO_SCRIPT				32
#define GO_MAP_PROPS			33
#define GO_SKY_BOX				34

// UI elements
#define GO_BUTTON				64
#define GO_LABEL				65

// Collision objects (only for editor)
#define GO_COMPOUND_CS			96
#define GO_BOX_CS				97
#define GO_SPHERE_CS			98
#define GO_CYLINDER_CS			99
#define GO_UNDEFINED_CS			100
#define GO_CONE_CS				101
#define GO_CAPSULE_CS			102
#define GO_MESH_CS				103

// Constraints
#define GO_P2P_CONSTRAINT		128
#define GO_HINGE_CONSTRAINT		129
#define GO_SLIDER_CONSTRAINT	130


#define GO_UNDEFINED			255


class GameObject;


typedef float vec3f[3];
typedef float vec2f[2];


enum GOPositioningMode {
	GOPositioningModeTopLeft,
	GOPositioningModeTopCenter,
	GOPositioningModeTopRight,
	GOPositioningModeCenterLeft,
	GOPositioningModeCenter,
	GOPositioningModeCenterRight,
	GOPositioningModeBottomLeft,
	GOPositioningModeBottomCenter,
	GOPositioningModeBottomRight
};


enum GOContentMode {
	GOContentModeCenter

};


class ContactInfo {
public:
	CVector contactPointA;
	CVector contactPointB;
};


/*struct Vertex {
	float x;
	float y;
	float z;
};*/

class MHAnimation;


class GameObject : public PhysicObject
{
friend class MapManager;

private:
	// Hierarchy
	vector<GameObject*> childs;
	GameObject* parent;
	string parentID; // for saving hierarchy in map
	GOPositioningMode positioningMode;
	float alpha; // for hierarchy alpha
	// blending
	bool hasCustomBlendFunction;
	GLenum customBlendFuncS;
	GLenum customBlendFuncD;
	bool dead; // if true then delete this object and remove from object list
	bool lightingEnabled;
	bool useGradOrientation;

	CVector position;
	CVector orientation; // in gradus
	CVector orientationM; // in Look At
	CVector scale;

	// animation
	vector<MHAnimation*> animations;



public:
	
	static Game* game;
	static unsigned int glNameCounter;
	static GameObject* fParentID;

	string id;
	int type; // Is very necessary to set!!!!!!!!!!!!!!!!!!!!
	bool luaBinded; // if true then this object have actual ID and may call lua events
	bool needSaveToMap; // EDITOR; if true then save to map while saving map
	bool opaque; // if had any alpha blend - false
	unsigned int glName; // for object picking
	bool is2D; // if true then render as 2d object
	bool needDeath; // if true then this object will be killed during MapManager::clear function call
	bool loadedFromMap; // if true then this object was be created from map, and when another map will be loaded this object will be killed
	MGColor color;

	bool needRelativeOrientation;

	// contact management
	vector<GameObject*> currentContacts;
	bool isHaveContactWith(GameObject* o);
	void removeContactWith(GameObject* o);

	void kill();
	bool isDead();
	void addToWorld();
	void removeFromWorld();
	virtual BBox getBBox();

	virtual void draw();
	virtual void update(float dt);
	virtual void save(FILE* f);
	GameObject* objectCreated(); // subclasses calls this method after creating instance. Return himself
	
	// Hierarchy
	void addChild(GameObject* o);
	void removeChild(GameObject* o);
	string getFullID();
	void setHidden(bool v);
	bool getHidden();
	void setNeedDraw(bool v);
	bool getNeedDraw();
	void setEnabled(bool v);
	bool getEnabled();
	void setNeedUpdate(bool v);
	bool getNeedUpdate();
	GameObject* getParent();
	vector<GameObject*> getChilds();
	void removeFromParent();
	bool hasParent(GameObject* candidat); // return true if candidat present on any level of hierarchy
	CVector getAbsolutePosition();
	CVector getAbsoluteOrientation();
	// Alpha
	void setAlpha(float v);
	float getAlpha();
	float getAbsoluteAlpha(); // return summari of all alpha
	void enableLighting(bool v);
	bool isLightingEnabled();

	// Positioning
	void setPosition(CVector v);
	CVector getPosition();
	void setOrientation(CVector v);
	CVector getOrientation();
	void setOrientationM(CVector v);
	CVector getOrientationM();
	void setScale(CVector v);
	CVector getScale();

	virtual void setPositioningMode(GOPositioningMode v);
	virtual GOPositioningMode getPositioningMode();
	CVector getPositioningOffset(GOPositioningMode v, float width, float height); // return offset from top left corner
	// blending
	void setBlendFunc(GLenum s, GLenum d);

	// Animation
	void addAnimation(MHAnimation* a);

	// Events
	// phys
	virtual void onContactOccured(GameObject* target, ContactInfo* ci);
	virtual void onContactLost(GameObject* target, ContactInfo* ci);
	// system
	virtual void onAddedToWorld(); // called while this object processed by MapManager::addObject
	virtual void onWillRemoveFromWorld(); // called while this object removed from game queue

protected:
	bool needDraw; // if true then this object will be rendered
	bool hidden; // if true then skip draw()
	bool enabled; // if false then skip update() for this object and child
	bool needUpdate; // if false then skip update

	// Graphic
	int verticesCount;
	GLuint VBOBuffer;
	MHVertex* vertexBuffer;
	bool needRefreshBuffers;
	virtual void refreshBuffers();
	// Lua binding
	virtual int methodsBridge(lua_State* luaVM);
private:
	// render
	void init();
	void bindLua();
	float computeAlpha();
	void updateFullIDsForChilds();
public:
	static void initStatic(Game* g);

	GameObject();
	GameObject(string id);
	GameObject(FILE* f); // create and load object from stream;

	virtual ~GameObject();
};


#endif /*GAMEOBJECT_H_*/
