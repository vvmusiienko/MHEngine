/*
 * MD2ModelData.h
 *
 *  Created on: 11.01.2011
 *      Author: Администратор
 */

#ifndef MD2MODELDATA_H_
#define MD2MODELDATA_H_

#include "AbstractModelData.h"
#include <string>
#include <vector>
#include <map>
#include "../CVector.h"
#include "../platform/opengl.h"
#include "../Common.h"

using std::map;
using std::vector;
using std::string;


// OpenGL vector types
typedef GLfloat vec2_t[2];
typedef GLfloat vec3_t[3];


// Md2 header
struct Md2Header_t
{
  int ident;          // Magic number, "IDP2"
  int version;        // Md2 format version, should be 8

  int skinwidth;      // Texture width
  int skinheight;     // Texture height

  int framesize;      // Size of a frame, in bytes

  int num_skins;      // Number of skins
  int num_vertices;   // Number of vertices per frame
  int num_st;         // Number of texture coords
  int num_tris;       // Number of triangles
  int num_glcmds;     // Number of OpenGL commands
  int num_frames;     // Number of frames

  int offset_skins;   // offset to skin data
  int offset_st;      // offset to texture coords
  int offset_tris;    // offset to triangle data
  int offset_frames;  // offset to frame data
  int offset_glcmds;  // offset to OpenGL commands
  int offset_end;     // offset to the end of the file
};


// Skin data
struct Md2Skin_t
{
  char name[64];  // Texture's filename
};


// Texture coords.
struct Md2TexCoord_t
{
  short s;
  short t;
};


// Triangle data
struct Md2Triangle_t
{
  unsigned short vertex[3];  // Triangle's vertex indices
  unsigned short st[3];      // Texture coords. indices
};


// Vertex data
struct Md2Vertex_t
{
  unsigned char v[3];         // Compressed vertex position
  unsigned char normalIndex;  // Normal vector index
};

struct FVertex {
	float x, y, z;
	unsigned char normalIndex;
};


// Frame data
struct Md2Frame_t
{
  // Destructor
  ~Md2Frame_t () {
	    delete [] verts;
	    delete [] uncompressedVerts;
  }

  vec3_t scale;        // Scale factors
  vec3_t translate;    // Translation vector
  char name[16];       // Frame name
  Md2Vertex_t *verts;  // Frames's vertex list
  vec3_t* uncompressedVerts;

  FVertex* finalVertices;
};


// OpenGL command packet
struct Md2Glcmd_t
{
  float s;    // S texture coord.
  float t;    // T texture coord.
  int index;  // Vertex index
};


// Animation infos
struct Md2Anim_t
{
  int start;  // first frame index
  int end;    // last frame index
};

typedef float vec3f[3];
typedef float vec2f[2];

typedef map<string, Md2Anim_t> AnimMap;

struct VT {
	int vi;
	int ti;
};


class MD2ModelData: public AbstractModelData {
friend class MD2Model;

private:
	void init();

	static int _kMd2Ident;
	static int _kMd2Version;

	// Model data
	Md2Header_t _header;
	Md2Skin_t *_skins;
	Md2TexCoord_t *_texCoords;
	Md2Triangle_t *_triangles;
	Md2Frame_t *_frames;
	int *_glcmds;

	// optimization
	unsigned short* finalIndices;
	int finalVerticesCount;
	vec2f* finalTexCoords;
	VT* tmpVT;


	CVector _scale;
	AnimMap _anims;

	void setupAnimations ();
	void calculateFinalVerticesForFrame(Md2Frame_t * frame);
	void calculateFinalIndicesAndVT();

public:

	bool isStatic();
	AbstractModelData* getCopy();
	float* getTriangles();
	int getTrianglesCount();

	static bool isMD2File(const string& fileName);
	MD2ModelData(string path);
	virtual ~MD2ModelData();
};

#endif /* MD2MODELDATA_H_ */
