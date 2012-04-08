/*
 * MD2Model.cpp
 *
 *  Created on: 23.12.2010
 *      Author: Администратор
 */

#include "MD2Model.h"
#include "../platform/opengl.h"
#include <iostream>
#include <fstream>
#include "../TextureCache.h"
#include "ModelCache.h"


using std::cout;
using std::cerr;
using std::endl;

/*void
Md2Model::renderFrameImmediate (int frame)
{
	// Bind to model's texture
	if (_tex)
		_tex->bind ();

	glBegin (GL_TRIANGLES);
		// Draw each triangle
		for (int i = 0; i < _header.num_tris; ++i) {
			// Draw each vertex of this triangle
			for (int j = 0; j < 3; ++j) {
				Md2Frame_t *pFrame = &_smoothFrame;
				Md2Vertex_t *pVert = &pFrame->verts[_triangles[i].vertex[j]];
				Md2TexCoord_t *pTexCoords = &_texCoords[_triangles[i].st[j]];

				// Compute final texture coords.
				GLfloat s = static_cast<GLfloat>(pTexCoords->s) / _header.skinwidth;
				GLfloat t = static_cast<GLfloat>(pTexCoords->t) / _header.skinheight;

				glTexCoord2f (s, 1.0f - t);

				// Send normal vector to OpenGL
				glNormal3fv (_kAnorms[pVert->normalIndex]);

				// Uncompress vertex position and scale it
				vec3_t v;

				v[0] = pFrame->uncompressedVerts[_triangles[i].vertex[j]][0];
				v[1] = pFrame->uncompressedVerts[_triangles[i].vertex[j]][1];
				v[2] = pFrame->uncompressedVerts[_triangles[i].vertex[j]][2];

				glVertex3fv (v);
			}
		}
	glEnd();
}*/

/////////////////////////////////////////////////////////////////////////////
//
// class Md2Model implementation.
//
/////////////////////////////////////////////////////////////////////////////

// Precomputed normal vector array (162 vectors)
vec3_t MD2Model::_kAnorms[] = {
#include "Anorms.h"
};



MD2Model::MD2Model(const string& modelName) throw (std::runtime_error) {
	init();
	this->modelName = modelName;

	// Load DATA
	modelData = (MD2ModelData*)ModelCache::getInstance()->load(modelName);
	if (modelData == NULL) {
		throw std::runtime_error("MD2Model::MD2Model: Can't load model data");
	}

	// Init smooth frame
	_smoothFrame = new Md2Frame_t;
	_smoothFrame->verts = new Md2Vertex_t[modelData->_header.num_vertices];
	_smoothFrame->uncompressedVerts = new vec3_t[modelData->_header.num_vertices];
	_smoothFrame->finalVertices = new FVertex[modelData->finalVerticesCount];

	_needSmooth = false;
	needRefreshBuffers = true;
	verticesBuff = NULL;
	normalsBuff = NULL;
	texCoordsBuff = NULL;

	// Set first animation as default animation
	_animInfo = &modelData->_anims.begin ()->second;
	next_animInfo = _animInfo;
	nextCycleAnim = true;
	_currentAnim = modelData->_anims.begin ()->first;
}

void MD2Model::init() {
	_scale = CVector(1.0, 1.0, 1.0);
	_texScale = CVector(1.0, 1.0, 1.0);
	_tex = NULL;

	_currFrame = 0;
	_nextFrame = 0;
	_interp = 0.0f;
	_percent = 0.0f;
	cycleAnim = true;
	oldVerticesCount = 0;
	glGenBuffers(1, &VBOBuffer);
	glGenBuffers(1, &indexBuffer);
	needRepeat = false;
	repX = 1;
	repY = 1;
	repZ = 1;
}


// For smooth animation changing. Set last frame as current animation position
void MD2Model::setSmoothFrame(int frameA, int frameB, float interp) {
	int verticesCount = modelData->finalVerticesCount;
	Md2Frame_t *pFrameA = &modelData->_frames[frameA];
	Md2Frame_t *pFrameB = &modelData->_frames[frameB];

	for (int i = 0; i < verticesCount; ++i) {
		// This might by buggy place!!!!!!!!!!!!!!!!!!!!!!!!!1
		// Need find correct normal? hm..
		_smoothFrame->finalVertices[i].normalIndex = pFrameA->finalVertices[i].normalIndex;

		// Compute final vertex position
		vec3_t vecA, vecB;

		vecA[0] = pFrameA->finalVertices[i].x;
		vecA[1] = pFrameA->finalVertices[i].y;
		vecA[2] = pFrameA->finalVertices[i].z;

		vecB[0] = pFrameB->finalVertices[i].x;
		vecB[1] = pFrameB->finalVertices[i].y;
		vecB[2] = pFrameB->finalVertices[i].z;

		// Linear interpolation and scaling
		_smoothFrame->finalVertices[i].x = (vecA[0] + interp * (vecB[0] - vecA[0]));  // x
		_smoothFrame->finalVertices[i].y = (vecA[1] + interp * (vecB[1] - vecA[1]));  // z
		_smoothFrame->finalVertices[i].z = (vecA[2] + interp * (vecB[2] - vecA[2]));  // y
	}
	_needSmooth = true;
}


void MD2Model::notUseSmooth() {
	_needSmooth = false;
}


CVector MD2Model::getModelSize() {
	int frameNumber = 0;

	// for get bounding box size
	float size_max_x = 0;
	float size_max_y = 0;
	float size_max_z = 0;
	float size_min_x = 0;
	float size_min_y = 0;
	float size_min_z = 0;
	CVector size;

	// Draw each triangle
	for (int i = 0; i < modelData->_header.num_tris; ++i) {
	// Draw each vertex of this triangle
		for (int j = 0; j < 3; ++j) {
			Md2Frame_t *pFrame = &modelData->_frames[frameNumber];
			Md2Vertex_t *pVert = &pFrame->verts[modelData->_triangles[i].vertex[j]];

			// Uncompress vertex position and scale it
			vec3_t v;

			v[0] = (pFrame->scale[0] * pVert->v[0] + pFrame->translate[0]) * _scale.x;
			v[1] = (pFrame->scale[1] * pVert->v[1] + pFrame->translate[1]) * _scale.z;
			v[2] = (pFrame->scale[2] * pVert->v[2] + pFrame->translate[2]) * _scale.y;

			if (v[0] > size_max_x)
				size_max_x = v[0];
			if (v[1] > size_max_z)
				size_max_z = v[1];
			if (v[2] > size_max_y)
				size_max_y = v[2];
			if (v[0] < size_min_x)
				size_min_x = v[0];
			if (v[1] < size_min_z)
				size_min_z = v[1];
			if (v[2] < size_min_y)
				size_min_y = v[2];
		}
	}

	size.x = size_max_x + -size_min_x;
	size.y = size_max_y + -size_min_y;
	size.z = size_max_z + -size_min_z;
	return size;
}


void MD2Model::setRepeat(int x, int z, int y) {
	repX = 1;
	repY = 1;
	repZ = 1;
	if (abs(x) > 1 || abs(y) > 1 || abs(z) > 1) {
		if (x != 0)
			repX = x;
		if (y != 0)
			repY = y;
		if (z != 0)
			repZ = z;
		needRepeat = true;
		needRefreshBuffers = true;
	} else {
		needRepeat = false;
	}
}


void MD2Model::refreshBuffersStock(const int frameA, const int frameB, const float interp) {
	verticesCount = modelData->finalVerticesCount;

	if (oldVerticesCount != verticesCount) {
		// generate indices
		int indicesCount = modelData->_header.num_tris * 3;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		if (modelData->isStatic()) {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(unsigned short), modelData->finalIndices, GL_STATIC_DRAW);
		} else {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(unsigned short), modelData->finalIndices, GL_DYNAMIC_DRAW);
		}

		// VERTEX
		glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);
		if (modelData->isStatic()) {
			glBufferData(GL_ARRAY_BUFFER, sizeof(MHVertex) * verticesCount, NULL, GL_STATIC_DRAW);
		} else {
			glBufferData(GL_ARRAY_BUFFER, sizeof(MHVertex) * verticesCount, NULL, GL_DYNAMIC_DRAW);
		}
		oldVerticesCount = verticesCount;
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);
	}
	vertexBuffer = (MHVertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (!vertexBuffer) {
		Log::warning("Can't map buffer. MD2Model(\"%s\")", modelName.c_str());
		return;
	}

	Md2Frame_t *pFrameA;
	if (_needSmooth) {
		pFrameA = _smoothFrame;
	} else {
		pFrameA = &modelData->_frames[frameA];
	}
	Md2Frame_t *pFrameB = &modelData->_frames[frameB];
	vec3_t vecA;
	float tx = 0.0f;
	float ty = 0.0f;
	float txh = 1.0f;
	float tyh = 1.0f;
	if (_tex) {
		tx = _tex->getU();
		ty = (1.0f - _tex->getV()) - _tex->getVL();
		txh = _tex->getUL();
		tyh = _tex->getVL();
	}
	int index;
	GLfloat *normA;
	GLfloat *normB;

	for (int i = 0; i < verticesCount; ++i) {
		// Draw each vertex of this triangle
		index = i;

		// Compute final texture coords.
		vertexBuffer[index].s = tx + (modelData->finalTexCoords[index][0] * _texScale.x * txh);
		vertexBuffer[index].t = 1.0f - (ty +  (modelData->finalTexCoords[index][1] * _texScale.y * tyh));

		// Compute interpolated normal vector
		normA = _kAnorms[pFrameA->finalVertices[index].normalIndex];
		normB = _kAnorms[pFrameB->finalVertices[index].normalIndex];

		vertexBuffer[index].nx = normA[0] + interp * (normB[0] - normA[0]);
		vertexBuffer[index].ny = normA[1] + interp * (normB[1] - normA[1]);
		vertexBuffer[index].nz = normA[2] + interp * (normB[2] - normA[2]);

		// Compute final vertex position
		vecA[0] = pFrameA->finalVertices[index].x;
		vecA[1] = pFrameA->finalVertices[index].y;
		vecA[2] = pFrameA->finalVertices[index].z;

		// Linear interpolation and scaling
		vertexBuffer[index].x = (vecA[0] + interp * (pFrameB->finalVertices[index].x - vecA[0])) * _scale.x;  // x
		vertexBuffer[index].y = (vecA[1] + interp * (pFrameB->finalVertices[index].y - vecA[1])) * _scale.z;  // z
		vertexBuffer[index].z = (vecA[2] + interp * (pFrameB->finalVertices[index].z - vecA[2])) * _scale.y;  // y
	}
	if (!glUnmapBuffer(GL_ARRAY_BUFFER) ) {
#ifdef MH_DEBUG
		Log::warning("Unmap buffer failed. MD2Model(\"%s\")", modelName.c_str());
#endif
	}
}


void MD2Model::refreshBuffersWithRepeat(const int frameA, const int frameB, const float interp) {
	CVector modelSize = getModelSize();

	verticesCount = modelData->_header.num_tris * 3 * abs(repX) * abs(repY) * abs(repZ);
	int verticesCountPerObject = modelData->_header.num_tris * 3;

	if (oldVerticesCount != verticesCount) {
		glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);
		if (modelData->isStatic()) {
			glBufferData(GL_ARRAY_BUFFER, sizeof(MHVertex) * verticesCount, 0, GL_STATIC_DRAW);
		} else {
			glBufferData(GL_ARRAY_BUFFER, sizeof(MHVertex) * verticesCount, 0, GL_DYNAMIC_DRAW);
		}
		oldVerticesCount = verticesCount;
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);
	}
	vertexBuffer = (MHVertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (!vertexBuffer) {
		Log::warning("Can't map buffer. MD2Model(\"%s\")", modelName.c_str());
		return;
	}

	for (int i = 0; i < modelData->_header.num_tris; ++i) {
		// Draw each vertex of this triangle
		for (int j = 0; j < 3; ++j) {
			Md2Frame_t *pFrameA;
			if (_needSmooth) {
				pFrameA = _smoothFrame;
			} else {
				pFrameA = &modelData->_frames[frameA];
			}

			Md2Frame_t *pFrameB = &modelData->_frames[frameB];

			Md2Vertex_t *pVertA = &pFrameA->verts[modelData->_triangles[i].vertex[j]];
			Md2Vertex_t *pVertB = &pFrameB->verts[modelData->_triangles[i].vertex[j]];

			Md2TexCoord_t *pTexCoords = &modelData->_texCoords[modelData->_triangles[i].st[j]];

			// Compute final texture coords.
			GLfloat s = static_cast<GLfloat>(pTexCoords->s) / modelData->_header.skinwidth;
			GLfloat t = static_cast<GLfloat>(pTexCoords->t) / modelData->_header.skinheight;

			float tx = 0.0f;
			float ty = 0.0f;
			float txh = 1.0f;
			float tyh = 1.0f;

			if (_tex) {
				tx = _tex->getU();
				ty = (1.0f - _tex->getV()) - _tex->getVL();
				txh = _tex->getUL();
				tyh = _tex->getVL();
			}


			// Compute interpolated normal vector
			const GLfloat *normA = _kAnorms[pVertA->normalIndex];
			const GLfloat *normB = _kAnorms[pVertB->normalIndex];

			// Compute final vertex position
			vec3_t vecA, vecB;

			vecA[0] = pFrameA->uncompressedVerts[modelData->_triangles[i].vertex[j]][0];
			vecA[1] = pFrameA->uncompressedVerts[modelData->_triangles[i].vertex[j]][1];
			vecA[2] = pFrameA->uncompressedVerts[modelData->_triangles[i].vertex[j]][2];

			vecB[0] = pFrameB->uncompressedVerts[modelData->_triangles[i].vertex[j]][0];
			vecB[1] = pFrameB->uncompressedVerts[modelData->_triangles[i].vertex[j]][1];
			vecB[2] = pFrameB->uncompressedVerts[modelData->_triangles[i].vertex[j]][2];

			int r = 0;
			for (int x = 0; x < abs(repX); x ++) {
				for (int y = 0; y < abs(repY); y ++) {
					for (int z = 0; z < abs(repZ); z ++) {
						vertexBuffer[r * verticesCountPerObject + (i * 3 + j)].nx = normA[0] + interp * (normB[0] - normA[0]);
						vertexBuffer[r * verticesCountPerObject + (i * 3 + j)].ny = normA[1] + interp * (normB[1] - normA[1]);
						vertexBuffer[r * verticesCountPerObject + (i * 3 + j)].nz = normA[2] + interp * (normB[2] - normA[2]);

						vertexBuffer[r * verticesCountPerObject + (i * 3 + j)].s = tx + (s * _texScale.x * txh);
						vertexBuffer[r * verticesCountPerObject + (i * 3 + j)].t = 1.0f - (ty +  (t * _texScale.y * tyh));
						// Linear interpolation and scaling
						vertexBuffer[r * verticesCountPerObject + (i * 3 + j)].x = x * modelSize.x + ((vecA[0] + interp * (vecB[0] - vecA[0])) * _scale.x);  // x
						vertexBuffer[r * verticesCountPerObject + (i * 3 + j)].y = y * modelSize.y + ((vecA[1] + interp * (vecB[1] - vecA[1])) * _scale.z);  // z
						vertexBuffer[r * verticesCountPerObject + (i * 3 + j)].z = z * modelSize.z + ((vecA[2] + interp * (vecB[2] - vecA[2])) * _scale.y);  // y
						r++;
					}
				}
			}

		}
	}
	if (!glUnmapBuffer(GL_ARRAY_BUFFER) ) {
#ifdef MH_DEBUG
		Log::warning("Unmap buffer failed. MD2Model(\"%s\")", modelName.c_str());
#endif
	}
}



// Refresh and prepare buffers for rendering.
void MD2Model::refreshBuffers(const int frameA, const int frameB, const float interp) {
	//Log::info("%i   %i   %f  ", frameA, frameB, interp);
	Game::instance->totalBufferRefreshCount ++;
	if (needRepeat) {
		refreshBuffersWithRepeat(frameA, frameB, interp);
	} else {
		refreshBuffersStock(frameA, frameB, interp);
	}
}


// --------------------------------------------------------------------------
// Render the model with frame interpolation, using immediate mode.
// --------------------------------------------------------------------------
void MD2Model::drawModelItpImmediate (int frameA, int frameB, float interp) {
	// Compute max frame index
	int maxFrame = modelData->_header.num_frames - 1;

	// Check if frames are valid
	if ((frameA < 0) || (frameB < 0))
		return;

	if ((frameA > maxFrame) || (frameB > maxFrame))
		return;

	if (frameA != frameB || _needSmooth)
		needRefreshBuffers = true;


	// Bind to model's texture
	if (_tex)
		_tex->bind ();

	if (needRefreshBuffers) {
		refreshBuffers(frameA, frameB, interp);
		needRefreshBuffers = false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);         // for vertex coordinates
	if (!needRepeat) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	}
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3/*size of one vertex*/,
					GL_FLOAT/*type of each component*/,
					sizeof(MHVertex)/*is the byte offset between vertices*/,
					BUFFER_OFFSET(0)/*no comments:P*/);
	glNormalPointer(GL_FLOAT/*type of each component*/,
					sizeof(MHVertex)/*is the byte offset between vertices*/,
					BUFFER_OFFSET(12)/*no comments:P*/);
	glTexCoordPointer(2/*size of one vertex*/,
					GL_FLOAT/*type of each component*/,
					sizeof(MHVertex)/*is the byte offset between vertices*/,
					BUFFER_OFFSET(24)/*no comments:P*/);

	if (!needRepeat) {
		glDrawElements(GL_TRIANGLES, modelData->_header.num_tris * 3, GL_UNSIGNED_SHORT, 0);
	} else {
		glDrawArrays(GL_TRIANGLES/*type of vertices*/, 0/*offset*/, verticesCount/*count*/);
	}



	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	// bind with 0, so, switch back to normal pointer operation
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (!needRepeat) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}


bool MD2Model::isStatic() {
	return false;
}


void MD2Model::setAnimation (const string& animName, bool cycle) {
	// Try to find the desired animation
	AnimMap::const_iterator itor;
	itor = modelData->_anims.find (animName);

	if (itor != modelData->_anims.end ()) {
		next_animInfo = &itor->second;
		nextCycleAnim = cycle;
		_nextAnim = animName;
	}
}



void MD2Model::setAnimationImmediate (const string& animName, bool cycle) {
	// Try to find the desired animation
	AnimMap::const_iterator itor;
	itor = modelData->_anims.find (animName);

	if (itor != modelData->_anims.end ()) {
		next_animInfo = &itor->second;
		nextCycleAnim = cycle;
		_nextAnim = animName;
		cycleAnim = nextCycleAnim;
		//---
		if (_animInfo != next_animInfo) {
		    setSmoothFrame(_currFrame, _nextFrame, _interp);
			_animInfo = next_animInfo;
			_currFrame = _animInfo->start;
			_nextFrame = _animInfo->start;
			_currentAnim = _nextAnim;
			_interp = 0.0f;
		}
	}
}


string MD2Model::getCurrentAnim() {
	return _currentAnim;
}


string MD2Model::getNextAnim() {
	return _nextAnim;
}


vector<string> MD2Model::getAllAnimations() {
	vector<string> anims;
	AnimMap::const_iterator itor;
	for (itor = modelData->_anims.begin (); itor != modelData->_anims.end (); ++itor)
		anims.push_back (itor->first);
	return anims;
}


void MD2Model::drawWithTexture(Texture* texture) {
	_tex = texture;
	glPushMatrix ();
		// Axis rotation
		glRotatef (-90.0f, 1.0f, 0.0f, 0.0f);
		//glScalef(2.0f, 2.0f, 2.0f);
		//glRotatef (-90.0f, 0.0f, 0.0f, 1.0f);

		// Set model scale factor
		//_model->setScale (_scale);

		glFrontFace (GL_CW);
		drawModelItpImmediate (_currFrame, _nextFrame, _interp);
		glFrontFace (GL_CCW);
	glPopMatrix ();
}


void MD2Model::animate (float dt) {
	animate (_animInfo->start, _animInfo->end, dt);
}


// --------------------------------------------------------------------------
// Animate the object.  Compute current and next frames, and the
// interpolation percent.
// --------------------------------------------------------------------------
void MD2Model::animate (int startFrame, int endFrame, float percent)
{
	// _currFrame must range between startFrame and endFrame
	if (_currFrame < startFrame)
		_currFrame = startFrame;

	if (_currFrame > endFrame)
		_currFrame = startFrame;

	_interp += percent;

	// Compute current and next frames.
	if (_interp >= 1.0) {
		if (!_needSmooth)
			_currFrame++;
		notUseSmooth();


		_interp = 0.0f;

		if (_currFrame >= endFrame) {
			if (cycleAnim) {
				_currFrame = startFrame;
			} else {
				_currFrame = endFrame;
			}

			if (_animInfo != next_animInfo) {
			    setSmoothFrame(endFrame,endFrame,0.0);
				_animInfo = next_animInfo;
				_currFrame = _animInfo->start;
				_nextFrame = _animInfo->start;
				cycleAnim = nextCycleAnim;
				_currentAnim = _nextAnim;
				_interp = 0.0f;
				return;
			}
			cycleAnim = nextCycleAnim;
		}

		_nextFrame = _currFrame + 1;

		if (_nextFrame >= endFrame) {
			if (cycleAnim) {
				_nextFrame = startFrame;
			} else {
				_nextFrame = endFrame;
			}
		}
	}
}


AbstractModel* MD2Model::getCopy() {
	return NULL;
}


MD2Model::~MD2Model() {
}

/*

void MD2Model::refreshBuffersStock(const int frameA, const int frameB, const float interp) {
	verticesCount = modelData->_header.num_tris * 3;
	Log::info("%i  %i", verticesCount, modelData->_header.num_vertices);

	bool needRefreshTextureData = false;
	if (oldVerticesCount != verticesCount) {
		glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);
		if (modelData->isStatic()) {
			glBufferData(GL_ARRAY_BUFFER, sizeof(MHVertex) * verticesCount, 0, GL_STATIC_DRAW);
		} else {
			glBufferData(GL_ARRAY_BUFFER, sizeof(MHVertex) * verticesCount, 0, GL_DYNAMIC_DRAW);
		}
		oldVerticesCount = verticesCount;
		needRefreshTextureData = true;
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);
	}
	vertexBuffer = (MHVertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (!vertexBuffer) {
		Log::warning("Can't map buffer. MD2Model(\"%s\")", modelName.c_str());
		return;
	}

	Md2Frame_t *pFrameA;
	if (_needSmooth) {
		pFrameA = _smoothFrame;
	} else {
		pFrameA = &modelData->_frames[frameA];
	}
	Md2Frame_t *pFrameB = &modelData->_frames[frameB];
	vec3_t vecA;
	float tx = 0.0f;
	float ty = 0.0f;
	float txh = 1.0f;
	float tyh = 1.0f;
	if (needRefreshTextureData) {
		if (_tex) {
			tx = _tex->getU();
			ty = (1.0f - _tex->getV()) - _tex->getVL();
			txh = _tex->getUL();
			tyh = _tex->getVL();
		}
	}
	int index;
	GLfloat s;
	GLfloat t;
	const Md2TexCoord_t *pTexCoords;
	GLfloat *normA;
	GLfloat *normB;

	for (int i = 0; i < modelData->_header.num_tris; ++i) {
		// Draw each vertex of this triangle
		for (int j = 0; j < 3; ++j) {
			index = i * 3 + j;

			if (needRefreshTextureData) {
				pTexCoords = &modelData->_texCoords[modelData->_triangles[i].st[j]];
				// Compute final texture coords.
				s = static_cast<GLfloat>(pTexCoords->s) / modelData->_header.skinwidth;
				t = static_cast<GLfloat>(pTexCoords->t) / modelData->_header.skinheight;

				vertexBuffer[index].s = tx + (s * _texScale.x * txh);
				vertexBuffer[index].t = 1.0f - (ty +  (t * _texScale.y * tyh));
			}

			// Compute interpolated normal vector
			normA = _kAnorms[pFrameA->verts[modelData->_triangles[i].vertex[j]].normalIndex];
			normB = _kAnorms[pFrameB->verts[modelData->_triangles[i].vertex[j]].normalIndex];

			vertexBuffer[index].nx = normA[0] + interp * (normB[0] - normA[0]);
			vertexBuffer[index].ny = normA[1] + interp * (normB[1] - normA[1]);
			vertexBuffer[index].nz = normA[2] + interp * (normB[2] - normA[2]);

			// Compute final vertex position
			vecA[0] = pFrameA->uncompressedVerts[modelData->_triangles[i].vertex[j]][0];
			vecA[1] = pFrameA->uncompressedVerts[modelData->_triangles[i].vertex[j]][1];
			vecA[2] = pFrameA->uncompressedVerts[modelData->_triangles[i].vertex[j]][2];

			// Linear interpolation and scaling
			vertexBuffer[index].x = (vecA[0] + interp * (pFrameB->uncompressedVerts[modelData->_triangles[i].vertex[j]][0] - vecA[0])) * _scale.x;  // x
			vertexBuffer[index].y = (vecA[1] + interp * (pFrameB->uncompressedVerts[modelData->_triangles[i].vertex[j]][1] - vecA[1])) * _scale.z;  // z
			vertexBuffer[index].z = (vecA[2] + interp * (pFrameB->uncompressedVerts[modelData->_triangles[i].vertex[j]][2] - vecA[2])) * _scale.y;  // y

		}
	}
	if (!glUnmapBuffer(GL_ARRAY_BUFFER) ) {
#ifdef MH_DEBUG
		Log::warning("Unmap buffer failed. MD2Model(\"%s\")", modelName.c_str());
#endif
	}
}


 */
