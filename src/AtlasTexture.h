/*
 * AtlasTexture.h
 *
 *  Created on: 17.12.2010
 *      Author: Администратор
 */

#ifndef ATLASTEXTURE_H_
#define ATLASTEXTURE_H_

#include "Texture.h"
#include "Atlas.h"

class AtlasTexture: public Texture {
private:
	Texture* currentTexture;
	string currentAtlasName;
	string currentAtlasElementName;
	Atlas* currentAtlas;
	AtlasElement* currentAtlasElement;
	AtlasElement* emptyAtlasElement;


public:
	static bool isNameWithAtlas(const string &texName); // return true if name contains atlas and atlas element

	void setAtlas(const string &atlas);
	void setAtlasElement(const string &element);
	const string &getAtlas();
	const string &getAtlasElement();

	float getU();
	float getV();
	float getUL();
	float getVL();

	virtual GLenum target () const { return GL_TEXTURE_2D; }

	AtlasTexture();
	AtlasTexture(string path);
	virtual ~AtlasTexture();
};

#endif /* ATLASTEXTURE_H_ */
