//*******************************************************************
//glfont2.cpp -- glFont Version 2.0 implementation
//Copyright (c) 1998-2002 Brad Fish
//See glfont.html for terms of use
//May 14, 2002
//*******************************************************************

//STL headers
#include <string>
#include <utility>
#include <iostream>
#include <fstream>
#include "../GraphicEngine.h"
using namespace std;


//glFont header
#include "glfont2.h"
using namespace glfont;

//*******************************************************************
//GLFont Class Implementation
//*******************************************************************
GLFont::GLFont ()
{
	//Initialize header to safe state
	header.tex = -1;
	header.tex_width = 0;
	header.tex_height = 0;
	header.start_char = 0;
	header.end_char = 0;
	header.chars = NULL;
}
//*******************************************************************
GLFont::~GLFont ()
{
	//Destroy the font
	Destroy();
}
//*******************************************************************
bool GLFont::Create (const char *file_name)
{
	unsigned int tex;
	glGenTextures(1, &tex); // generate texture object

	ifstream input;
	int num_chars, num_tex_bytes;
	char *tex_bytes;

	//Destroy the old font if there was one, just to be safe
	Destroy();

	//Open input file
	input.open(file_name, ios::in | ios::binary);
	if (!input)
		return false;

	//Read the header from file
	input.read((char *)&header, sizeof(header));
	texType = header.tex;
	header.tex = tex;

	//Allocate space for character array
	num_chars = header.end_char - header.start_char + 1;
	if ((header.chars = new GLFontChar[num_chars]) == NULL)
		return false;

	//Read character array
	input.read((char *)header.chars, sizeof(GLFontChar) *
		num_chars);

	//Read texture pixel data
	num_tex_bytes = header.tex_width * header.tex_height * (texType == 120 ? 4 : 2);
	tex_bytes = new char[num_tex_bytes];
	input.read(tex_bytes, num_tex_bytes);

	//Create OpenGL texture
	glBindTexture(GL_TEXTURE_2D, tex);  
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	if (texType == 120) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, header.tex_width,
			header.tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			(void *)tex_bytes);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, header.tex_width,
			header.tex_height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE,
			(void *)tex_bytes);
	}

	//Free texture pixels memory
	delete[] tex_bytes;

	//Close input file
	input.close();

	//Return successfully
	return true;
}
//*******************************************************************
bool GLFont::Create (const std::string &file_name)
{
	return Create(file_name.c_str());
}
//*******************************************************************
void GLFont::Destroy (void)
{
	//Delete the character array if necessary
	if (header.chars)
	{
		delete[] header.chars;
		header.chars = NULL;
	}
}
//*******************************************************************
void GLFont::GetTexSize (std::pair<int, int> *size)
{
	//Retrieve texture size
	size->first = header.tex_width;
	size->second = header.tex_height;
}
//*******************************************************************
int GLFont::GetTexWidth (void)
{
	//Return texture width
	return header.tex_width;
}
//*******************************************************************
int GLFont::GetTexHeight (void)
{
	//Return texture height
	return header.tex_height;
}
//*******************************************************************
void GLFont::GetCharInterval (std::pair<int, int> *interval)
{
	//Retrieve character interval
	interval->first = header.start_char;
	interval->second = header.end_char;
}
//*******************************************************************
int GLFont::GetStartChar (void)
{
	//Return start character
	return header.start_char;
}
//*******************************************************************
int GLFont::GetEndChar (void)
{
	//Return end character
	return header.end_char;
}
//*******************************************************************
float GLFont::getFontHeight()
{
	int num_chars = header.end_char - header.start_char + 1;
	float height = 0.0f;
	GLFontChar *glfont_char;
	for (int i = 0; i < num_chars; i++) {
		//Get pointer to glFont character
		glfont_char = &header.chars[i];


		//Get width and height
		height += glfont_char->dy * header.tex_width;
	}
	return height / (float)num_chars;
}
//*******************************************************************
void GLFont::GetCharSize (int c, std::pair<int, int> *size)
{
	//Make sure character is in range
	if (c < header.start_char || c > header.end_char)
	{
		//Not a valid character, so it obviously has no size
		size->first = 0;
		size->second = 0;
	}
	else
	{
		GLFontChar *glfont_char;

		//Retrieve character size
		glfont_char = &header.chars[c - header.start_char];
		size->first = (int)(glfont_char->dx * header.tex_width);
		size->second = (int)(glfont_char->dy *
			header.tex_height);
	}
}
//*******************************************************************
int GLFont::GetCharWidth (int c)
{
	//Make sure in range
	if (c < header.start_char || c > header.end_char)
		return 0;
	else
	{
		GLFontChar *glfont_char;
		
		//Retrieve character width
		glfont_char = &header.chars[c - header.start_char];
		return (int)(glfont_char->dx * header.tex_width);
	}
}
//*******************************************************************
int GLFont::GetCharHeight (int c)
{
	//Make sure in range
	if (c < header.start_char || c > header.end_char)
		return 0;
	else
	{
		GLFontChar *glfont_char;

		//Retrieve character height
		glfont_char = &header.chars[c - header.start_char];
		return (int)(glfont_char->dy * header.tex_height);
	}
}
//*******************************************************************
void GLFont::Begin (void)
{
	//Bind to font texture
	glBindTexture(GL_TEXTURE_2D, header.tex);
	GraphicEngine::lastBindedTexture = header.tex;
}
//*******************************************************************


//Template function to output a character array
void GLFont::DrawString (const char *text, float x,
	float y)
{
	const char *i;
	GLFontChar *glfont_char;
	float width, height;

	static int buffersSize = 0;
	static MHVertexPT* verticesBuff = 0;


	int verticesCount = 0;

	//Count of characters
	for (i = text; *i != (char)'\0'; i++) {
		//Make sure character is in range
		if (*i < header.start_char || *i > header.end_char)
			continue;
		verticesCount ++;
	}
	verticesCount = verticesCount * 6;

	// Mem allocation
	if (verticesCount > buffersSize) {
		buffersSize = verticesCount + 300;
		if (verticesBuff) delete [] verticesBuff;
		verticesBuff = new MHVertexPT[buffersSize];
	}

	//Loop through characters
	int j = 0; // if each incorrect character increment this variable
	for (i = text; *i != (char)'\0'; i++)
	{
		//Make sure character is in range
		if (*i < header.start_char || *i > header.end_char) {
			continue;
		}

		//Get pointer to glFont character
		glfont_char = &header.chars[*i - header.start_char];

		//Get width and height
		width = glfont_char->dx * header.tex_width;
		height = glfont_char->dy * header.tex_height;

		int pos = j * 6;
		j ++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty1;

		pos ++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty2;

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty2;

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty2;

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty1;

		pos++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty1;

		//Move to next character
		x += width;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3/*size of one vertex*/,
					GL_FLOAT/*type of each component*/,
					sizeof(MHVertexPT)/*is the byte offset between vertices*/,
					verticesBuff/*no comments:P*/);
	glTexCoordPointer(2/*size of one vertex*/,
					GL_FLOAT/*type of each component*/,
					sizeof(MHVertexPT)/*is the byte offset between vertices*/,
					&verticesBuff[0].s/*no comments:P*/);
	glDrawArrays(GL_TRIANGLES/*type of vertices*/, 0/*offset*/, verticesCount/*count*/);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


//Template function to output a scaled character array
void GLFont::DrawString (const char *text, float scalar,
	float x, float y)
{
	const char *i;
	GLFontChar *glfont_char;
	float width, height;

	static int buffersSize = 0;
	static MHVertexPT* verticesBuff = 0;


	int verticesCount = 0;

	//Count of characters
	for (i = text; *i != (char)'\0'; i++) {
		//Make sure character is in range
		if (*i < header.start_char || *i > header.end_char)
			continue;
		verticesCount ++;
	}
	verticesCount = verticesCount * 6;

	// Mem allocation
	if (verticesCount > buffersSize) {
		buffersSize = verticesCount + 300;
		if (verticesBuff) delete [] verticesBuff;
		verticesBuff = new MHVertexPT[buffersSize];
	}

	int j = 0;
	for (i = text; *i != (char)'\0'; i++)
	{
		//Make sure character is in range
		if (*i < header.start_char || *i > header.end_char) {
			continue;
		}
		//Get pointer to glFont character
		glfont_char = &header.chars[*i - header.start_char];

		//Get width and height
		width = (glfont_char->dx * header.tex_width) * scalar;
		height = (glfont_char->dy * header.tex_height) * scalar;

		int pos = j * 6;
		j ++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty1;

		pos ++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty2;

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty2;

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty2;

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty1;

		pos++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty1;

		//Move to next character
		x += width;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3/*size of one vertex*/,
					GL_FLOAT/*type of each component*/,
					sizeof(MHVertexPT)/*is the byte offset between vertices*/,
					verticesBuff/*no comments:P*/);
	glTexCoordPointer(2/*size of one vertex*/,
					GL_FLOAT/*type of each component*/,
					sizeof(MHVertexPT)/*is the byte offset between vertices*/,
					&verticesBuff[0].s/*no comments:P*/);
	glDrawArrays(GL_TRIANGLES/*type of vertices*/, 0/*offset*/, verticesCount/*count*/);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


//Template function to output a colored character array
void GLFont::DrawString (const char *text, float scalar, float x,
	float y, const unsigned char *top_color,
	const unsigned char *bottom_color)
{
	const char *i;
	GLFontChar *glfont_char;
	float width, height;

	static int buffersSize = 0;
	static MHVertexPTC* verticesBuff = 0;
	int verticesCount = 0;

	//Count of characters
	for (i = text; *i != (char)'\0'; i++) {
		//Make sure character is in range
		if (*i < header.start_char || *i > header.end_char)
			continue;
		verticesCount ++;
	}
	verticesCount = verticesCount * 6;

	// Mem allocation
	if (verticesCount > buffersSize) {
		buffersSize = verticesCount + 300;
		if (verticesBuff) delete [] verticesBuff;
		verticesBuff = new MHVertexPTC[buffersSize];
	}

	int j = 0;
	for (i = text; *i != (char)'\0'; i++)
	{
		//Make sure character is in range
		if (*i < header.start_char || *i > header.end_char) {
			continue;
		}
		//Get pointer to glFont character
		glfont_char = &header.chars[*i - header.start_char];

		//Get width and height
		width = (glfont_char->dx * header.tex_width) * scalar;
		height = (glfont_char->dy * header.tex_height) * scalar;

		int pos = j * 6;
		j ++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty1;
		memcpy(verticesBuff[pos].color, top_color, 4);


		pos ++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty2;
		memcpy(verticesBuff[pos].color, bottom_color, 4);

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty2;
		memcpy(verticesBuff[pos].color, bottom_color, 4);

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty2;
		memcpy(verticesBuff[pos].color, bottom_color, 4);

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty1;
		memcpy(verticesBuff[pos].color, top_color, 4);

		pos ++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty1;
		memcpy(verticesBuff[pos].color, top_color, 4);

		//Move to next character
		x += width;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3/*size of one vertex*/,
					GL_FLOAT/*type of each component*/,
					sizeof(MHVertexPTC)/*is the byte offset between vertices*/,
					verticesBuff/*no comments:P*/);
	glTexCoordPointer(2/*size of one vertex*/,
					GL_FLOAT/*type of each component*/,
					sizeof(MHVertexPTC)/*is the byte offset between vertices*/,
					&verticesBuff[0].s/*no comments:P*/);
	glColorPointer(4/*number components per color */,
					GL_UNSIGNED_BYTE/*type of each color component*/,
					sizeof(MHVertexPTC)/*is the number of bytes between consecutive colors;*/,
					verticesBuff[0].color);
	glDrawArrays(GL_TRIANGLES/*type of vertices*/, 0/*offset*/, verticesCount/*count*/);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}










int GLFont::charactersToDrawCount(const char *text) {
	int verticesCount = 0;
	const char *i;

	//Count of characters
	for (i = text; *i != (char)'\0'; i++) {
		//Make sure character is in range
		if (*i < header.start_char || *i > header.end_char)
			continue;
		verticesCount ++;
	}
	return verticesCount;
}




//Template function to output a character array
void GLFont::DrawStringToBuffer (void* buffer, const char *text, float x,
		float y)
{
	const char *i;
	GLFontChar *glfont_char;
	float width, height;

	static MHVertexPT* verticesBuff = 0;
	verticesBuff = (MHVertexPT*)buffer;

	int verticesCount = 0;

	//Count of characters
	for (i = text; *i != (char)'\0'; i++) {
		//Make sure character is in range
		if (*i < header.start_char || *i > header.end_char)
			continue;
		verticesCount ++;
	}
	verticesCount = verticesCount * 6;

	//Loop through characters
	int j = 0; // if each incorrect character increment this variable
	for (i = text; *i != (char)'\0'; i++)
	{
		//Make sure character is in range
		if (*i < header.start_char || *i > header.end_char) {
			continue;
		}

		//Get pointer to glFont character
		glfont_char = &header.chars[*i - header.start_char];

		//Get width and height
		width = glfont_char->dx * header.tex_width;
		height = glfont_char->dy * header.tex_height;

		int pos = j * 6;
		j ++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty1;

		pos ++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty2;

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty2;

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty2;

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty1;

		pos++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty1;

		//Move to next character
		x += width;
	}
}



//Template function to output a scaled character array
void GLFont::DrawStringToBuffer (void* buffer, const char *text, float scalar,
		float x, float y)
{
	const char *i;
	GLFontChar *glfont_char;
	float width, height;

	static MHVertexPT* verticesBuff = 0;
	verticesBuff = (MHVertexPT*)buffer;

	int verticesCount = 0;

	//Count of characters
	for (i = text; *i != (char)'\0'; i++) {
		//Make sure character is in range
		if (*i < header.start_char || *i > header.end_char)
			continue;
		verticesCount ++;
	}
	verticesCount = verticesCount * 6;

	int j = 0;
	for (i = text; *i != (char)'\0'; i++)
	{
		//Make sure character is in range
		if (*i < header.start_char || *i > header.end_char) {
			continue;
		}
		//Get pointer to glFont character
		glfont_char = &header.chars[*i - header.start_char];

		//Get width and height
		width = (glfont_char->dx * header.tex_width) * scalar;
		height = (glfont_char->dy * header.tex_height) * scalar;

		int pos = j * 6;
		j ++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty1;

		pos ++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty2;

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty2;

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty2;

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty1;

		pos++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty1;

		//Move to next character
		x += width;
	}
}


//Template function to output a colored character array
void GLFont::DrawStringToBuffer (void* buffer, const char *text, float scalar, float x,
		float y, const unsigned char *top_color,
		const unsigned char *bottom_color)
{
	const char *i;
	GLFontChar *glfont_char;
	float width, height;

	static MHVertexPTC* verticesBuff = 0;
	verticesBuff = (MHVertexPTC*)buffer;
	int verticesCount = 0;

	//Count of characters
	for (i = text; *i != (char)'\0'; i++) {
		//Make sure character is in range
		if (*i < header.start_char || *i > header.end_char)
			continue;
		verticesCount ++;
	}
	verticesCount = verticesCount * 6;

	int j = 0;
	for (i = text; *i != (char)'\0'; i++)
	{
		//Make sure character is in range
		if (*i < header.start_char || *i > header.end_char) {
			continue;
		}
		//Get pointer to glFont character
		glfont_char = &header.chars[*i - header.start_char];

		//Get width and height
		width = (glfont_char->dx * header.tex_width) * scalar;
		height = (glfont_char->dy * header.tex_height) * scalar;

		int pos = j * 6;
		j ++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty1;
		memcpy(verticesBuff[pos].color, top_color, 4);


		pos ++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty2;
		memcpy(verticesBuff[pos].color, bottom_color, 4);

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty2;
		memcpy(verticesBuff[pos].color, bottom_color, 4);

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y - height;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty2;
		memcpy(verticesBuff[pos].color, bottom_color, 4);

		pos ++;
		verticesBuff[pos].x = x + width;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx2;
		verticesBuff[pos].t = glfont_char->ty1;
		memcpy(verticesBuff[pos].color, top_color, 4);

		pos ++;
		verticesBuff[pos].x = x;
		verticesBuff[pos].y = y;
		verticesBuff[pos].z = 0.0f;
		verticesBuff[pos].s = glfont_char->tx1;
		verticesBuff[pos].t = glfont_char->ty1;
		memcpy(verticesBuff[pos].color, top_color, 4);

		//Move to next character
		x += width;
	}
}

//End of file

