#ifndef COMMON_H_
#define COMMON_H_

#include "CVector.h"
#include <string>
extern "C" {
# include "./lua/lua.h"
# include "./lua/lauxlib.h"
# include "./lua/lualib.h"
}
using namespace std;
class GameObject;

// +++++++++++++++++++  KEYS ++++++++++++++++++++++

#define	KEY_UP			38
#define	KEY_DOWN		40
#define	KEY_LEFT		37
#define	KEY_RIGHT		39
#define	KEY_ESC			27
#define	KEY_SPACE		32
#define	KEY_TILD		192
#define	KEY_BACKSPACE	8
#define	KEY_RETURN		13
#define	KEY_SHIFT		16
#define	KEY_CTRL		17

// OPENGL Structures
struct MHVertex {
	float x, y, z;		//Vertex
	float nx, ny, nz;	//Normal
	float s, t;			//Texcoord
};


struct MHVertexPT {
	float x, y, z;		//Vertex
	float s, t;			//Texcoord
};

struct MHVertexPTC {
	float x, y, z;		//Vertex
	float s, t;			//Texcoord
	unsigned char color[4];
};


class BBox {
public:
	CVector upperLeft;
	CVector lowerRight;
	BBox(CVector upperLeft, CVector lowerRight) {
		this->upperLeft = upperLeft;
		this->lowerRight = lowerRight;
	}
	BBox() {
	}
};


// ++++++++++++++++++ OTHER  ++++++++++++++++++++
#define GRAD_TO_RAD_COEF	0.0174532925f


#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#if !defined(MAX)
#define MAX(A,B)	({ __typeof__(A) __a = (A); __typeof__(B) __b = (B); __a < __b ? __b : __a; })
#endif

string generateRandomString(int size);
CVector fromEulerToAxis(CVector v);
void buildIDPath(string path);
float randf();


// +++++++++++++++++++  Commands ++++++++++++++++++++++
class Action {
public:
	virtual void execute(GameObject* sender) = 0;
};


// ++++++++++++++++++ COLORS ++++++++++++++++
#include "MGColor.h"


// +++++++++++++++++++  Event Handling ++++++++++++++++++++++

struct CursorState {
      float  dx,dy;
      float  x,y;
      float  lastX, lastY;

      bool lButton;
      bool rButton;
      bool mButton;

      float hAngle;
      float vAngle;
      float sencivity;

};


enum CursorButton {
	LEFT,
	MIDDLE,
	RIGHT
};


string FormatString(string format, ...);

//
float modf(float a, float b);


// ++++++++++++++++ FILE STREAMS ++++++++++++++
float readFloat(FILE* f);
char readChar(FILE* f);
int readInt(FILE* f);
CVector readVector(FILE* f);
string readString(FILE* f);
MGColor readColor(FILE* f);

void writeFloat(float val, FILE* f);
void writeChar(char val, FILE* f);
void writeInt(int val, FILE* f);
void writeVector(CVector val, FILE* f);
void writeString(string val, FILE* f);
void writeColor(MGColor val, FILE* f);


// +++++++++++++++++++++ LUA +++++++++++++++
void luaPushVector(lua_State* luaVM, float x, float y, float z);


// +++++++++++++++ DYM ARRAYS

template <typename T>
T **AllocateDynamicArray( int nRows, int nCols)
{
      T **dynamicArray;

      dynamicArray = new T*[nRows];
      for( int i = 0 ; i < nRows ; i++ )
      dynamicArray[i] = new T [nCols];

      return dynamicArray;
}

template <typename T>
void FreeDynamicArray(T** dArray)
{
      delete [] *dArray;
      delete [] dArray;
}


#endif /*COMMON_H_*/
