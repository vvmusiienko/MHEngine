#ifndef GOPLATE_H_
#define GOPLATE_H_

#include "GameObject.h"
#include <string>
#include "../Texture.h"
#include "../LuaVM.h"

using namespace std;

class GOPlate : public GameObject {
public:
	string textureName;

private:
	Texture* texture;
	CVector size;
	float textureZoomS;
	float textureZoomT;
	float density; // length of one segment. If zero then 1 segment
	bool autoSize; // if true the size of plate equal to the size of texture
	float tx,ty,tw,th; // rectangle area of texture to by used
	// area
	bool useArea;
	CVector areaOrigin;
	CVector areaSize;
	//fliping
	bool flipHorizontal;
	bool flipVertical;

	
	int oldVerticesCount;

private:
	void init();
	void refreshBuffers();
	void bindLua();
	int methodsBridge(lua_State* luaVM);

public:
	void setFlipHorizontal(bool v);
	bool getFlipHorizontal();
	void setFlipVertical(bool v);
	bool getFlipVertical();
	CVector getSize() {return size;}
	void setSize(CVector size);
	void setArea(CVector origin, CVector size);
	float getTextureZoomS() {return textureZoomS;}
	void setTextureZoomS(float z);
	float getTextureZoomT() {return textureZoomT;}
	void setTextureZoomT(float z);
	float getDensity() {return density;}
	void setDensity(float d);
	void setTexture(string name);
	void setTexture(Texture* tex);
	Texture* getTexture();
	void setAutoSize(bool as);
    bool getAutoSize();
    

	virtual void draw();
	virtual void update(float dt);
	virtual void save(FILE* f);

	GOPlate();
	GOPlate(string id);
	GOPlate(FILE* f);
	virtual ~GOPlate();
};

#endif /*GOPLATE_H_*/
