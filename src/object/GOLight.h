#ifndef GOLIGHT_H_
#define GOLIGHT_H_

#include "../CVector.h"
#include "../Common.h"
#include "GameObject.h"

class GOModel;

// Light types
#define	DIRECTIONAL		0
#define	POSITIONAL		1
#define	SPOT			2


class GOLight : public GameObject
{
private:
	static int currentLight;
	static unsigned int lightNames[];
	static float defSpecular[];
	static float defDiffuse[];
	static float defPos[];
	static float defDir[];
	static float defSpotExponent; // Насиченість світла
	static float defSpotCutoff;
	static float defConstAttenuation;
	static float defLinearAttenuation;
	static float defQuadAttenuation;
	int lightNum;
	bool drawed;
	
	GOModel* lightModel;

public:

	int lightType;
	MGColor lightDiffuse;
	MGColor lightSpecular;
	CVector attenuation; // x - GL_CONSTANT_ATTENUATION; y - GL_LINEAR_ATTENUATION; z - GL_QUADRATIC_ATTENUATION;
	float spotExponent; // Насиченість світла
	float spotCutoff;
private:
	void init();
	void bindLua();
	
public:
	void apply();
	void cancel();
	void setType(int type);
	
	// interface implementation
	void draw();
	void update(float dt);
	void save(FILE* f);
	
	GOLight();
	GOLight(string id);
	GOLight(FILE* f);
	virtual ~GOLight();
	
};

#endif /*GOLIGHT_H_*/
