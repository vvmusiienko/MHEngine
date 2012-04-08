/*
 * MD2Model.h
 *
 *  Created on: 23.12.2010
 *      Author: Администратор
 */

#ifndef MD2Model_H_
#define MD2Model_H_

#include "AbstractModel.h"
#include <vector>
#include <string>
#include <map>
#include "../Texture.h"
#include "MD2ModelData.h"
#include "../Common.h"


using std::map;
using std::vector;
using std::string;


class MD2Model: public AbstractModel {

private:

	void init();
	void renderFrameImmediate (int frame);
	void setSmoothFrame(int frameA, int frameB, float interp);
	void notUseSmooth();
	void drawModelItpImmediate (int frameA, int frameB, float interp);

	void refreshBuffers(const int frameA, const int frameB, const float interp);
	void refreshBuffersStock(const int frameA, int const frameB, const float interp);
	void refreshBuffersWithRepeat(const int frameA, const int frameB, const float interp);

	MD2ModelData* modelData;
	string modelName;

	bool needRepeat;
	int repX;
	int repY;
	int repZ;


	// Member variables
	// Graphic

	GLuint VBOBuffer;
	GLuint indexBuffer;
	MHVertex* vertexBuffer;
	int verticesCount;
	int oldVerticesCount;
	vec3f* verticesBuff;
	vec3f* normalsBuff;
	vec2f* texCoordsBuff;
	bool needRefreshBuffers;

	bool _needSmooth;
	// Constants
	static vec3_t _kAnorms[162];

	Md2Frame_t* _smoothFrame; // Need by one special frame more for smooth animation changing

	CVector _scale;
	CVector _texScale; // texture scale;
	Texture *_tex;

	// ++++ MD2Object import
	int _currFrame;
	int _nextFrame;
	float _interp;

	float _percent;

	// Animation data
	const Md2Anim_t *_animInfo;
	string _currentAnim;
	string _nextAnim;
	bool cycleAnim;
	const Md2Anim_t *next_animInfo;
	bool nextCycleAnim;

	void animate (int startFrame, int endFrame, float percent);

public:

	// VIRTUAL METHODS
	bool isStatic(); // return true if this model have not animations

	void setAnimation (const string& animName, bool cycle);
	void setAnimationImmediate (const string& animName, bool cycle);
	string getCurrentAnim();
	string getNextAnim();
	vector<string> getAllAnimations();
	CVector getModelSize();
	void setRepeat(int x, int y, int z);

	void drawWithTexture(Texture* texture);
	void animate (float dt);

	AbstractModel* getCopy(); // return new Instance of AbstractModel which is copy of current object


	MD2Model(const string& modelName) throw (std::runtime_error);
	virtual ~MD2Model();
};

#endif /* MD2MODELDATA_H_ */
