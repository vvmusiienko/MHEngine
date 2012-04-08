/*
 * GOGround.h
 *
 *  Created on: 14.02.2010
 *      Author: Администратор
 */

#ifndef GOGROUND_H_
#define GOGROUND_H_

#include "GameObject.h"
#include "GOPlate.h"
#include "GOModel.h"


class GOGround: public GameObject {
private:
	GOPlate* topAndBottom;
	GOPlate* base1; // along
	GOPlate* base2; // perpendicular
	GOModel* edgeModel;
	GOModel* centerModel1;
	GOModel* centerModel2;
	string modelPrefix;
	string texturePrefix;
	CVector size;

private:
	void init();
	void bindLua();
protected:
	virtual int methodsBridge(lua_State* luaVM);
	virtual void makeCollisionShape();
public:
	void setSize(CVector v);
	CVector getSize();
	void setTexturePrefix(string name); // texture name == ground_ + texture_prefix + _top
	void setModelPrefix(string name); // model name == ground_ + model_prefix + _edge or _center
	void draw();
	void update(float dt);
	void save(FILE* f);
	BBox getBBox();

	GOGround();
	GOGround(string id);
	GOGround(FILE* f);
	virtual ~GOGround();
};

#endif /* GOGROUND_H_ */
