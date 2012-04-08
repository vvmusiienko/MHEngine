/*
 * MD2ModelData.cpp
 *
 *  Created on: 11.01.2011
 *      Author: Администратор
 */

#include "MD2ModelData.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "../Log.h"

using std::cout;
using std::cerr;
using std::endl;


// Precomputed normal vector array (162 vectors)
vec3_t _kAnorms[] = {
#include "Anorms.h"
};


// Magic number (should be 844121161)
int MD2ModelData::_kMd2Ident = 'I' + ('D'<<8) + ('P'<<16) + ('2'<<24);

// MD2 format version
int MD2ModelData::_kMd2Version = 8;


MD2ModelData::MD2ModelData(string path) {
	init();
	// Open the file
	std::ifstream ifs (path.c_str (), std::ios::binary);

	if (ifs.fail ())
		throw std::runtime_error ("Couldn't open file");

	// Read header
	ifs.read (reinterpret_cast<char *>(&_header), sizeof (Md2Header_t));

	// Check if ident and version are valid
	if (_header.ident != _kMd2Ident)
		throw std::runtime_error ("Bad file ident");

	if (_header.version != _kMd2Version)
		throw std::runtime_error ("Bad file version");

	// Memory allocation for model data
	_skins = new Md2Skin_t[_header.num_skins];
	_texCoords = new Md2TexCoord_t[_header.num_st];
	_triangles = new Md2Triangle_t[_header.num_tris];
	_frames = new Md2Frame_t[_header.num_frames];
	_glcmds = new int[_header.num_glcmds];

	// Read skin names
	ifs.seekg (_header.offset_skins, std::ios::beg);
	ifs.read (reinterpret_cast<char *>(_skins),
		sizeof (Md2Skin_t) * _header.num_skins);

	// Read texture coords.
	ifs.seekg (_header.offset_st, std::ios::beg);
	ifs.read (reinterpret_cast<char *>(_texCoords),
		sizeof (Md2TexCoord_t) * _header.num_st);

	// Read triangle data
	ifs.seekg (_header.offset_tris, std::ios::beg);
	ifs.read (reinterpret_cast<char *>(_triangles),
		sizeof(Md2Triangle_t) * _header.num_tris);

	// Read frames
	ifs.seekg (_header.offset_frames, std::ios::beg);
	for (int i = 0; i < _header.num_frames; ++i) {
		// Memory allocation for the vertices of this frame
		_frames[i].verts = new Md2Vertex_t[_header.num_vertices];
		_frames[i].uncompressedVerts = new vec3_t[_header.num_vertices];

		// Read frame data
		ifs.read (reinterpret_cast<char *>(&_frames[i].scale),
		sizeof (vec3_t));
		ifs.read (reinterpret_cast<char *>(&_frames[i].translate),
		sizeof (vec3_t));
		ifs.read (reinterpret_cast<char *>(&_frames[i].name),
		sizeof (char) * 16);
		ifs.read (reinterpret_cast<char *>(_frames[i].verts),
		sizeof (Md2Vertex_t) * _header.num_vertices);

		// uncompress vertexes
		// each triangle
		for (int t = 0; t < _header.num_tris; ++t) {
		// each vertex of this triangle
			for (int j = 0; j < 3; ++j) {
				Md2Frame_t *pFrame = &_frames[i];
				Md2Vertex_t *pVert = &pFrame->verts[_triangles[t].vertex[j]];
				// Uncompress vertex position and scale it
				pFrame->uncompressedVerts[_triangles[t].vertex[j]][0] = pFrame->scale[0] * pVert->v[0] + pFrame->translate[0];
				pFrame->uncompressedVerts[_triangles[t].vertex[j]][1] = pFrame->scale[1] * pVert->v[1] + pFrame->translate[1];
				pFrame->uncompressedVerts[_triangles[t].vertex[j]][2] = pFrame->scale[2] * pVert->v[2] + pFrame->translate[2];
			}
		}
	}


	// Read OpenGL commands
	ifs.seekg (_header.offset_glcmds, std::ios::beg);
	ifs.read (reinterpret_cast<char *>(_glcmds),
		sizeof (int) * _header.num_glcmds);

	// Close file
	ifs.close();

	// Setup animation infos
	setupAnimations ();

	// OTIMIZATION;
	calculateFinalIndicesAndVT();
	for (int i = 0; i < _header.num_frames; ++i) {
		calculateFinalVerticesForFrame(&_frames[i]);
	}
	delete [] tmpVT;
#ifdef MH_DEBUG
	Log::info("Model %s loaded. OVC %i, AVC %i", path.c_str(), finalVerticesCount, _header.num_tris * 3);
#endif
}


int getIndexForVertex(VT v, int currentVertexNum, VT* finalVertices) {
	for (int i = 0; i < currentVertexNum; i++) {
		if (finalVertices[i].vi == v.vi &&
				finalVertices[i].ti == v.ti)
			return i;
	}
	return -1;
}


void MD2ModelData::calculateFinalIndicesAndVT() {
	int verticesCount = _header.num_tris * 3;
	VT* allVertices = new VT[verticesCount];
	tmpVT = new VT[verticesCount];

	for (int i = 0; i < _header.num_tris; ++i) {
		// Draw each vertex of this triangle
		for (int j = 0; j < 3; ++j) {
			int index = i * 3 + j;
			allVertices[index].ti = _triangles[i].st[j];
			allVertices[index].vi = _triangles[i].vertex[j];
		}
	}

	// create indices
	int currentVertexNum = 0;
	vector<unsigned short> tmpfinalIndices;

	for (int i = 0; i < verticesCount; i ++) {
		int index = getIndexForVertex(allVertices[i], currentVertexNum, tmpVT);
		if (index < 0) { // there no this vertex
			tmpVT[currentVertexNum] = allVertices[i];
			tmpfinalIndices.push_back(currentVertexNum);
			currentVertexNum++;
		} else {
			tmpfinalIndices.push_back(index);
		}
	}

	// generate new txt coords
	finalTexCoords = new vec2f[currentVertexNum];
	for (int i = 0; i < currentVertexNum; i++) {
		Md2TexCoord_t *pTexCoords = &_texCoords[tmpVT[i].ti];
		// Compute final texture coords.
		finalTexCoords[i][0] = static_cast<GLfloat>(pTexCoords->s) / _header.skinwidth;
		finalTexCoords[i][1] = static_cast<GLfloat>(pTexCoords->t) / _header.skinheight;
	}

	finalIndices = new unsigned short [tmpfinalIndices.size()];
	for (unsigned int i = 0; i < tmpfinalIndices.size(); i++) {
		finalIndices[i] = tmpfinalIndices.at(i);
	}
	finalVerticesCount = currentVertexNum;

	delete [] allVertices;
}


void MD2ModelData::calculateFinalVerticesForFrame(Md2Frame_t * frame) {
	frame->finalVertices = new FVertex[finalVerticesCount];
	for (int i = 0; i < finalVerticesCount; i++) {
		// pos
		frame->finalVertices[i].x = frame->uncompressedVerts[tmpVT[i].vi][0];
		frame->finalVertices[i].y = frame->uncompressedVerts[tmpVT[i].vi][1];
		frame->finalVertices[i].z = frame->uncompressedVerts[tmpVT[i].vi][2];
		// Norms
		frame->finalVertices[i].normalIndex = frame->verts[tmpVT[i].vi].normalIndex;
	}
}



float* MD2ModelData::getTriangles() {
	float* buff = new float[_header.num_tris * 9];
	// each triangle
	for (int t = 0; t < _header.num_tris; ++t) {
	// each vertex of this triangle
		for (int j = 0; j < 3; ++j) {
			Md2Frame_t *pFrame = &_frames[0];
			// Uncompress vertex position and scale it (inverted)
			buff[t * 9 + j * 3 + 0] = pFrame->uncompressedVerts[_triangles[t].vertex[j]][0];
			buff[t * 9 + j * 3 + 1] = pFrame->uncompressedVerts[_triangles[t].vertex[j]][2];
			buff[t * 9 + j * 3 + 2] = -pFrame->uncompressedVerts[_triangles[t].vertex[j]][1];
		}
	}

	return buff;
}


int MD2ModelData::getTrianglesCount() {
	return _header.num_tris;
}


// Setup animation infos.
void MD2ModelData::setupAnimations () {
	string currentAnim;
	Md2Anim_t animInfo = { 0, 0 };

	for (int i = 0; i < _header.num_frames; ++i) {
		string frameName = _frames[i].name;
		string frameAnim;

		// Extract animation name from frame name
		string::size_type len = frameName.find_first_of ("0123456789");
		if ((len == frameName.length () - 3) &&
		(frameName[len] != '0'))
			len++;

		frameAnim.assign (frameName, 0, len);

		if (currentAnim != frameAnim) {
			if (i > 0) {
				// Previous animation is finished, insert
				// it and start new animation.
				_anims.insert (AnimMap::value_type
						(currentAnim, animInfo));
			}

			// Initialize new anim info
			animInfo.start = i;
			animInfo.end = i;

			currentAnim = frameAnim;
		} else {
			animInfo.end = i;
		}
	}

	// Insert last animation
	_anims.insert (AnimMap::value_type (currentAnim, animInfo));
}


void MD2ModelData::init() {
	_skins = NULL;
	_texCoords = NULL;
	_triangles = NULL;
	_frames = NULL;
	_glcmds = NULL;
	_scale = CVector(1.0, 1.0, 1.0);
}



bool MD2ModelData::isStatic() {
	return (_anims.size() == 1);
}


AbstractModelData* MD2ModelData::getCopy() {
	return NULL;
}


bool MD2ModelData::isMD2File(const string& fileName) {
	string tmpStr = string(fileName);

	for (unsigned int i=0;i<strlen(tmpStr.c_str());i++)
		if (tmpStr[i] >= 0x41 && tmpStr[i] <= 0x5A)
			tmpStr[i] = tmpStr[i] + 0x20;

	if (tmpStr.find(".md2") == string::npos) {
		return false;
	}
	return true;
}


MD2ModelData::~MD2ModelData() {
	delete [] _skins;
	delete [] _texCoords;
	delete [] _triangles;
	delete [] _frames;
	delete [] _glcmds;
}
