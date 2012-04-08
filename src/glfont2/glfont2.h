//*******************************************************************
//glfont2.h -- Header for glfont2.cpp
//Copyright (c) 1998-2002 Brad Fish
//See glfont.html for terms of use
//May 14, 2002
//*******************************************************************

#ifndef GLFONT2_H
#define GLFONT2_H

//*******************************************************************
//GLFont Interface
//*******************************************************************

#include "../platform/opengl.h"
#include "../Common.h"

//glFont namespace
namespace glfont
{
	class GLFont;	
}


//glFont class
class glfont::GLFont
{
private:

	typedef unsigned char vec4ub[4];
	typedef float vec4f[4];
	typedef float vec3f[3];
	typedef float vec2f[2];


	//glFont character structure
	typedef struct
	{
		float dx, dy;
		float tx1, ty1;
		float tx2, ty2;
	} GLFontChar;

	//glFont header structure
	struct
	{
		int tex;
		int tex_width, tex_height;
		int start_char, end_char;
		GLFontChar *chars;
	} header;

	int texType; // 120 - is rgba else la

public:

	//Constructor
	GLFont ();

	//Destructor
	~GLFont ();

public:

	//Creates the glFont
	bool Create (const char *file_name);
	bool Create (const std::string &file_name);

	//Destroys the glFont
	void Destroy (void);

	//Texture size retrieval methods
	void GetTexSize (std::pair<int, int> *size);
	int GetTexWidth (void);
	int GetTexHeight (void);

	//Character interval retrieval methods
	void GetCharInterval (std::pair<int, int> *interval);
	int GetStartChar (void);
	int GetEndChar (void);

	//Character size retrieval methods
	float getFontHeight();// return average height for each character
	void GetCharSize (int c, std::pair<int, int> *size);
	int GetCharWidth (int c);
	int GetCharHeight (int c);

	//Calculates the size in pixels of a character array
	void GetStringSize (const char *text, std::pair<float, float> *size)
	{
		const char *i;
		GLFontChar *glfont_char;
		float width;
		
		//Height is the same for now...might change in future
		size->second = (int)getFontHeight();

		//Calculate width of string
		width = 0.0F;
		for (i = text; *i != (char)'\0'; i++)
		{
			//Make sure character is in range
			if (*i < header.start_char || *i > header.end_char)
				continue;

			//Get pointer to glFont character
			glfont_char = &header.chars[*i - header.start_char];

			//Get width and height
			width += glfont_char->dx * header.tex_width;		
		}

		//Save width
		size->first = (int)width;
	}
	


	//Begins text output with this font
	void Begin (void);


	//Template function to output a character array
	void DrawString (const char *text, float x,
		float y);

	//Template function to output a scaled character array
	void DrawString (const char *text, float scalar,
		float x, float y);

	//Template function to output a colored character array
	void DrawString (const char *text, float scalar, float x,
		float y, const unsigned char *top_color,
		const unsigned char *bottom_color);



	// Draw to buffer
	//Template function to output a character array
	void DrawStringToBuffer (void* buffer, const char *text, float x,
		float y);


	//Template function to output a scaled character array
	void DrawStringToBuffer (void* buffer, const char *text, float scalar,
		float x, float y);

	//Template function to output a colored character array
	void DrawStringToBuffer (void* buffer, const char *text, float scalar, float x,
		float y, const unsigned char *top_color,
		const unsigned char *bottom_color);


	int charactersToDrawCount(const char *text);

};

//*******************************************************************

#endif

//End of file

