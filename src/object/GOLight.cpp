#include "../platform/opengl.h"
#include "GOLight.h"
#include "GOModel.h"
#include <stdexcept>


int GOLight::currentLight = 0;
unsigned int GOLight::lightNames[] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};
float GOLight::defSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};
float GOLight::defDiffuse[] = {0.0f, 0.0f, 0.0f, 1.0f};
float GOLight::defPos[] = {0.0f, 0.0f, 0.0f, 1.0f};
float GOLight::defDir[] = {0.0f, 0.0f, 1.0f};
float GOLight::defSpotExponent = 0.0f; // Насиченість світла
float GOLight::defSpotCutoff = 180.0f;
float GOLight::defConstAttenuation = 1.0f;
float GOLight::defLinearAttenuation = 0.0f;
float GOLight::defQuadAttenuation = 0.0f;



GOLight::GOLight() : GameObject() {
	init();
}


GOLight::GOLight(string id) : GameObject(id) {
	init();
	bindLua();
}


GOLight::GOLight(FILE* f) : GameObject(f) {
	try {
		init();
		lightType = readChar(f);
		lightSpecular = readColor(f);
		lightDiffuse = readColor(f);
		spotExponent = readFloat(f);
		spotCutoff = readFloat(f);
		attenuation = readVector(f);
		bindLua();
	} catch (std::runtime_error e) {
		throw std::runtime_error(FormatString("GOLight::GOLight(FILE* f): Can't create object from stream! Reason:\n%s", e.what()));
	}
}


void GOLight::save(FILE* f) {
	GameObject::save(f);
	writeChar(lightType, f);
	writeColor(lightSpecular, f);
	writeColor(lightDiffuse, f);
	writeFloat(spotExponent, f);
	writeFloat(spotCutoff, f);
	writeVector(attenuation, f);
}


void GOLight::setType(int type) {
	this->lightType = type;
}


void GOLight::init() {
	type = GO_LIGHT;
	this->lightType = POSITIONAL;
	drawed = false;
	lightSpecular = MGColor(defSpecular[0], defSpecular[1], defSpecular[2], defSpecular[3]);
	lightDiffuse = MGColor(defDiffuse[0], defDiffuse[1], defDiffuse[2], defDiffuse[3]);
	spotExponent = 0.0f; // Насиченість світла
	spotCutoff = 180.0f;
	attenuation = CVector(defConstAttenuation, defLinearAttenuation, defQuadAttenuation);
	lightModel = NULL;

	if (game->editorMode) {
		lightModel = new GOModel();
		lightModel->setModel("light_spot.md2");
		lightModel->setTexture("light_spot.tga");
	}
}


void GOLight::bindLua() {
	game->luaVM->registerProperty(id + ".%slightType", MG_INT, &this->lightType);
	game->luaVM->registerProperty(id + ".%sdiffuse", MG_COLOR, &this->lightDiffuse);
	game->luaVM->registerProperty(id + ".%sspecular", MG_COLOR, &this->lightSpecular);
	game->luaVM->registerProperty(id + ".%sspotExponent", MG_FLOAT, &this->spotExponent);
	game->luaVM->registerProperty(id + ".%sspotCutoff", MG_FLOAT, &this->spotCutoff);
	game->luaVM->registerProperty(id + ".%sattenuation", MG_VECTOR, &this->attenuation);
}


void GOLight::apply() {
	static float specular[4];
	static float diffuse[4];
	static float pos[4];
	static float dir[3];
	
	switch (lightType) {
		case DIRECTIONAL:
			specular[0] = lightSpecular.getR();
			specular[1] = lightSpecular.getG();
			specular[2] = lightSpecular.getB();
			specular[3] = lightSpecular.getA();
			
			diffuse[0] = lightDiffuse.getR();
			diffuse[1] = lightDiffuse.getG();
			diffuse[2] = lightDiffuse.getB();
			diffuse[3] = lightDiffuse.getA();
			
			pos[0] = getOrientation().x;
			pos[1] = getOrientation().y;
			pos[2] = getOrientation().z;
			pos[3] = 0.0f;
			
		    glLightfv(lightNames[currentLight], GL_SPECULAR, specular);
		    glLightfv(lightNames[currentLight], GL_DIFFUSE, diffuse);
		    glLightfv(lightNames[currentLight], GL_POSITION, pos);
		    glLightfv(lightNames[currentLight], GL_SPOT_DIRECTION, defDir);
		    glLightf(lightNames[currentLight], GL_SPOT_EXPONENT, defSpotExponent);
		    glLightf(lightNames[currentLight], GL_SPOT_CUTOFF, defSpotCutoff);
		    glLightf(lightNames[currentLight], GL_CONSTANT_ATTENUATION, defConstAttenuation);
		    glLightf(lightNames[currentLight], GL_LINEAR_ATTENUATION, defLinearAttenuation);
		    glLightf(lightNames[currentLight], GL_QUADRATIC_ATTENUATION, defQuadAttenuation);
		    glEnable (lightNames[currentLight]);			
			break;
		
		case POSITIONAL:
			specular[0] = lightSpecular.getR();
			specular[1] = lightSpecular.getG();
			specular[2] = lightSpecular.getB();
			specular[3] = lightSpecular.getA();
			
			diffuse[0] = lightDiffuse.getR();
			diffuse[1] = lightDiffuse.getG();
			diffuse[2] = lightDiffuse.getB();
			diffuse[3] = lightDiffuse.getA();
			
			pos[0] = getAbsolutePosition().x;
			pos[1] = getAbsolutePosition().y;
			pos[2] = getAbsolutePosition().z;
			pos[3] = 1.0f;

			glLightfv(lightNames[currentLight], GL_SPECULAR, specular);
		    glLightfv(lightNames[currentLight], GL_DIFFUSE, diffuse);
		    glLightfv(lightNames[currentLight], GL_POSITION, pos);
		    glLightfv(lightNames[currentLight], GL_SPOT_DIRECTION, defDir);
		    glLightf(lightNames[currentLight], GL_SPOT_EXPONENT, defSpotExponent);
		    glLightf(lightNames[currentLight], GL_SPOT_CUTOFF, defSpotCutoff);
		    glLightf(lightNames[currentLight], GL_CONSTANT_ATTENUATION, attenuation.x);
		    glLightf(lightNames[currentLight], GL_LINEAR_ATTENUATION,  attenuation.y);
		    glLightf(lightNames[currentLight], GL_QUADRATIC_ATTENUATION,  attenuation.z);
		    glEnable (lightNames[currentLight]);
			break;

		case SPOT:
			specular[0] = lightSpecular.getR();
			specular[1] = lightSpecular.getG();
			specular[2] = lightSpecular.getB();
			specular[3] = lightSpecular.getA();
			
			diffuse[0] = lightDiffuse.getR();
			diffuse[1] = lightDiffuse.getG();
			diffuse[2] = lightDiffuse.getB();
			diffuse[3] = lightDiffuse.getA();
			
			pos[0] = getAbsolutePosition().x;
			pos[1] = getAbsolutePosition().y;
			pos[2] = getAbsolutePosition().z;
			pos[3] = 1.0f;

			dir[0] = getOrientation().x;
			dir[1] = getOrientation().y;
			dir[2] = getOrientation().z;
			dir[3] = 0.0f;
			
			
			glLightfv(lightNames[currentLight], GL_SPECULAR, specular);
		    glLightfv(lightNames[currentLight], GL_DIFFUSE, diffuse);
		    glLightfv(lightNames[currentLight], GL_POSITION, pos);
		    glLightfv(lightNames[currentLight], GL_SPOT_DIRECTION, dir);
		    glLightf(lightNames[currentLight], GL_SPOT_EXPONENT, spotExponent);
		    glLightf(lightNames[currentLight], GL_SPOT_CUTOFF, spotCutoff);
		    glLightf(lightNames[currentLight], GL_CONSTANT_ATTENUATION, attenuation.x);
		    glLightf(lightNames[currentLight], GL_LINEAR_ATTENUATION,  attenuation.y);
		    glLightf(lightNames[currentLight], GL_QUADRATIC_ATTENUATION,  attenuation.z);
		    glEnable (lightNames[currentLight]);
			
			break;
	}
	lightNum = currentLight;
}


void GOLight::draw() {
	glPushMatrix();
	GameObject::draw();
	glPopMatrix();
	if (lightModel) {
		// save previous state of texturing
		bool lightingEnabled = glIsEnabled(GL_LIGHTING);
		glDisable(GL_LIGHTING);
		lightModel->color = color;
		lightModel->draw();
		if (lightingEnabled) {
			glEnable(GL_LIGHTING);
		} else {
			glDisable(GL_LIGHTING);
		}
	}
	drawed = false;
	if (currentLight > 7) return;
	apply();
	currentLight++;
	drawed = true;
}



void GOLight::cancel() {
    if (!drawed) return;
	glDisable (lightNames[lightNum]);
    currentLight--;
}


void GOLight::update(float dt) {
	GameObject::update(dt);
	if (lightModel) {
		lightModel->setPosition(getAbsolutePosition());
		lightModel->setOrientation(getOrientation());
	}
}


GOLight::~GOLight()
{
}
