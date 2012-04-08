/*
 * AtlasTexture.cpp
 *
 *  Created on: 17.12.2010
 *      Author: Мусієнко Володимир
 */

#include "AtlasTexture.h"
#include "Game.h"
#include "TextureCache.h"
#include "AtlasCache.h"

AtlasTexture::AtlasTexture() {

}

AtlasTexture::AtlasTexture(string path) {
	emptyAtlasElement = new AtlasElement;
	emptyAtlasElement->u = 0.0f;
	emptyAtlasElement->v = 0.0f;
	emptyAtlasElement->uh = 1.0f;
	emptyAtlasElement->vh = 1.0f;

	currentAtlas = NULL;
	currentAtlasElement = emptyAtlasElement;

	if (isNameWithAtlas(path)) { // parse atlas and element names
		string textureName, atlasName, atlasElementName;
		string::size_type dogIndex = path.find("@");
		string::size_type twoPointsIndex = path.find(":");
		if (dogIndex == string::npos) { // default name for atlas (is equal to texture name)
			textureName = path.substr(0, twoPointsIndex);
			atlasName = textureName;
			atlasElementName = path.substr(twoPointsIndex + 1);
		} else {
			textureName = path.substr(0, dogIndex);
			atlasName = path.substr(dogIndex + 1, twoPointsIndex - dogIndex - 1);
			atlasElementName = path.substr(twoPointsIndex + 1);
		}

		currentTexture = TextureCache::getInstance()->load(textureName);
		setAtlas(atlasName);
		setAtlasElement(atlasElementName);
		//Log::info("text \"%s\"   atlas  \"%s\"     elem  \"%s\" \n %i  %i ",textureName.c_str(), atlasName.c_str(), atlasElementName.c_str(), dogIndex, twoPointsIndex);
	} else {
		currentTexture = TextureCache::getInstance()->load(path);
	}
	if (!currentTexture) {
		_fail = true;
		return;
	}

	img = currentTexture->img;
	_name = currentTexture->name();
	_handle = currentTexture->handle();

	_fail = false;
}


bool AtlasTexture::isNameWithAtlas(const string &texName) {
	if (texName.find(":") != string::npos) {
		return true;
	}
	return false;
}


void AtlasTexture::setAtlas(const string &atlas) {
	currentAtlasName = atlas;
	currentAtlas = AtlasCache::getInstance()->load(atlas);
	// try set latest atlas element
}


void AtlasTexture::setAtlasElement(const string &element) {
	if (!currentAtlas) {
		Log::warning("Can't set atlas element without Atlas assigned (%s)", element.c_str());
		return;
	}

	AtlasElement* ae = currentAtlas->getElement(element);
	if (!ae) {
		Log::warning("Can't find element \"%s\" for Atlas \"%s\".", element.c_str(), currentAtlasName.c_str());
		return;
	}

	currentAtlasElement = ae;
	currentAtlasElementName = element;
}


const string &AtlasTexture::getAtlas() {
	return currentAtlasName;
}


const string &AtlasTexture::getAtlasElement() {
	return currentAtlasElementName;
}


float AtlasTexture::getU() {
	return currentAtlasElement->u;
}


float AtlasTexture::getV() {
	return currentAtlasElement->v;
}


float AtlasTexture::getUL() {
	return currentAtlasElement->uh;
}


float AtlasTexture::getVL() {
	return currentAtlasElement->vh;
}


AtlasTexture::~AtlasTexture() {
	// TODO Auto-generated destructor stub
}
