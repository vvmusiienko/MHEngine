#include "Common.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdexcept>
#include <vector>
#include "Log.h"
#include "Game.h"

using namespace std;


float randf() {
    return (rand() % 200) / 200.0f;
}



CVector fromEulerToAxis(CVector v) {
/*	float heading = v.x;
	float attitude = v.y;
	float bank = v.z;
	float c1 = cos(heading/2);
	float s1 = sin(heading/2);
	float c2 = cos(attitude/2);
	float s2 = sin(attitude/2);
	float c3 = cos(bank/2);
	float s3 = sin(bank/2);
	float c1c2 = c1*c2;
	float s1s2 = s1*s2;
	w =c1c2*c3 - s1s2*s3;
	x =c1c2*s3 + s1s2*c3;
	y =s1*c2*c3 + c1*s2*s3;
	z =c1*s2*c3 - s1*c2*s3;
	angle = 2 * Math.acos(w);
	float norm = x*x+y*y+z*z;
	if (norm < 0.001) { // when all euler angles are zero angle =0 so
		// we can set axis to anything to avoid divide by zero
		x=1;
		y=z=0;
	} else {
		norm = Math.sqrt(norm);
    	x /= norm;
    	y /= norm;
    	z /= norm;
	}*/
	return CVector();
}


float modf(float a, float b) {
	return ((a / b) - (int)(a / b)) * b;
}


string generateRandomString(int size) {
	if (size <= 0)
		return "";
	char buffer [size+1];
	buffer[size] = 0;
	for (int i = 0; i < size; i++) {
		int symbol = rand() % 36;
		if (symbol > 25) {
			// number
			buffer[i] = 48 + (symbol - 26);
		} else {
			// letter
			buffer[i] = 97 + symbol;
		}
	}
	return string(buffer);
}



void buildIDPath(string path) {
	if (path == "")
		return;

	vector<string> parts;

	parts.push_back(path);
	string tmp_path = path;
	int last_point_pos = -2;
	do {
		last_point_pos  = tmp_path.rfind(".");
		if (last_point_pos != -1) {
			tmp_path = tmp_path.substr(0, last_point_pos);
			parts.push_back(tmp_path);
		}
	} while (last_point_pos != -1);

	for (int i = parts.size()-1; i > -1; i--) {
		Game::instance->luaVM->doString(FormatString("if %s == nil then %s = {} end", parts.at(i).c_str(), parts.at(i).c_str() ));
	}
}


// ++++++++++++++++ FILE STREAMS ++++++++++++++
float readFloat(FILE* f) {
	static float val;
	if (fread(&val, sizeof val, 1, f) != 1) throw std::runtime_error("readFloat: can't read from stream");
	return val;
}


char readChar(FILE* f) {
	static char val;
	if (fread(&val, sizeof val, 1, f) != 1) throw std::runtime_error("readChar: can't read from stream");
	return val;
}


int readInt(FILE* f) {
	static int val;
	if (fread(&val, sizeof val, 1, f) != 1) throw std::runtime_error("readInt: can't read from stream");
	return val;
}


CVector readVector(FILE* f) {
	static float val[3];
	if (fread(&val, sizeof val, 1, f) != 1) throw std::runtime_error("readVector: can't read from stream");
	return CVector(val[0],val[1],val[2]);
}


string readString(FILE* f) {
	static unsigned short int sizeOfString;
	if (fread(&sizeOfString, sizeof sizeOfString, 1, f) != 1) throw std::runtime_error("readString: can't read from stream"); // size
	char buffer[sizeOfString + 1];
	if (sizeOfString > 0)
		if (fread(buffer, sizeOfString, 1, f) != 1) throw std::runtime_error("readString: can't read from stream"); // body
	buffer[sizeOfString] = 0;
	return string(buffer);
}


MGColor readColor(FILE* f) {
	static unsigned char c[4];
	if (fread(c, sizeof c, 1, f) != 1) throw std::runtime_error("readColor: can't read from stream");
	return MGColor(c[0], c[1], c[2], c[3]);
}


void writeFloat(float val, FILE* f) {
	if( fwrite(&val, sizeof val, 1, f) != 1) throw std::runtime_error("writeFloat: can't write to stream");
}


void writeChar(char val, FILE* f) {
	if( fwrite(&val, sizeof val, 1, f) != 1) throw std::runtime_error("writeChar: can't write to stream");
}


void writeInt(int val, FILE* f) {
	if( fwrite(&val, sizeof val, 1, f) != 1) throw std::runtime_error("writeInt: can't write to stream");
}


void writeVector(CVector val, FILE* f) {
	float buf[] = {val.x, val.y, val.z};
	if( fwrite(&buf, sizeof buf, 1, f) != 1) throw std::runtime_error("writeVector: can't write to stream");
}


void writeString(string val, FILE* f) {
	static unsigned short int sizeOfString;
	sizeOfString = val.size(); // Write string without null char
	if( fwrite(&sizeOfString, sizeof sizeOfString, 1, f) != 1) throw std::runtime_error("writeString: can't write to stream"); // size
	if (sizeOfString > 0)
		if( fwrite(val.c_str(), sizeOfString, 1, f) != 1) throw std::runtime_error("writeString: can't write to stream"); // body
}


void writeColor(MGColor val, FILE* f) {
	if( fwrite(val.getByteComponents(), 4, 1, f) != 1) throw std::runtime_error("writeColor: can't write to stream");
}


string FormatString(string format, ...)
{
	char buffer[1024];
	va_list arg;
	int ret;

	// Format the text
	va_start (arg, format);
	  ret = vsnprintf(buffer, sizeof (buffer), format.c_str(), arg);
	va_end (arg);

	return string(buffer);
}


// +++++++++++++++++++++ LUA +++++++++++++++
void luaPushVector(lua_State* luaVM, float x, float y, float z) {
	lua_newtable(luaVM);
	int tableIndex = lua_gettop(luaVM);
	lua_pushstring(luaVM, "x");
	lua_pushnumber(luaVM, x);
	lua_settable (luaVM, tableIndex);
	lua_pushstring(luaVM, "y");
	lua_pushnumber(luaVM, y);
	lua_settable (luaVM, tableIndex);
	lua_pushstring(luaVM, "z");
	lua_pushnumber(luaVM, z);
	lua_settable (luaVM, tableIndex);
}





