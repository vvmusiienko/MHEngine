/*
 * GOModel.h
 *
 *  Created on: 16.11.2010
 *      Author: Администратор
 */

#ifndef GOMODEL_H_
#define GOMODEL_H_

#include "GameObject.h"
#include "../LuaVM.h"
#include <string>
#include <vector>
#include "../Texture.h"
#include "../model/AbstractModel.h"

class CVector;

using namespace std;

class GOModel : public GameObject {

private:
	CVector texScale;
	float frameRate;
	AbstractModel* modelData;
	Texture* texture;
	string currentModel;
	string currentTexture;
	string currentAnim;
	CVector modelSize;
	CVector repeating; // repeat draw more than 1 times;

	void init();
	void bindLua();

protected:
	virtual int methodsBridge(lua_State* luaVM);

public:
	void setModel (const string& modelName);
	string getModelName ();
	void setTexture (const string& textureName); // set texture
	void setTexture (Texture* texture); // set already prepared texture
	Texture* getTexture ();
	string getTextureName ();
	void setAnimation (const string& animName, bool cycle); // animate mesh with specified animation; animation will be applied after current animation will be played
	void setAnimationImmediate (const string &animName, bool cycle); // instant change animation to specified animation;
	string getCurrentAnimation (); // return currently played animation
	string getNextAnimation (); //return animation which will be played after current
	vector<string> getAllAnimations (); // return all available animations
	void setFrameRate (const float frameRate); // animate mesh with specified frame rate; default value is 1.0f
	float getFrameRate (); // return current framerate
	CVector getModelSize(); // return BBox for current model
	void setRepeat(CVector v);
	CVector getRepeat();


	// Override
	void draw();
	void update(float dt);
	void save(FILE* f);
	void makeCollisionShape();
	BBox getBBox();

	GOModel();
	GOModel(string id);
	GOModel(FILE* f);
	virtual ~GOModel();
};

#endif /* GOMODEL_H_ */
