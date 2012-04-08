#ifndef GOCUBE_H_
#define GOCUBE_H_

#include "GameObject.h"


class GOCube : public GameObject
{
private:
	void init();
	void bindLua();

	CVector size;

	// VBO
	GLuint VBOBuffer;
	vec3f* verticesBuff;
	void addVertex(int trinum, float x, float y, float z, float nx, float ny, float nz);

protected:

public:

	virtual int methodsBridge(lua_State* luaVM);
	void draw();
	void update(float dt);
	void save(FILE* f);
	BBox getBBox();

	
	void refreshBuffers();

	virtual void onWillRemoveFromWorld();
	void makeCollisionShape();

	void setSize(CVector size);
	CVector getSize() {return size;};

	GOCube();
	GOCube(string id);
	GOCube(FILE* f);
	virtual ~GOCube();
};

#endif /*GOCUBE_H_*/
