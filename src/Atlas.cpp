/*
 * Atlas.cpp
 *
 *  Created on: 19.12.2010
 *      Author: Администратор
 */

#include "Atlas.h"
#include "Game.h"
#include <stdexcept>

Atlas::Atlas() {
}


Atlas::Atlas(string path) {
	FILE* f = fopen(path.c_str(), "rb");
	if (!f) {
		throw runtime_error("Can't open file");
	}

	// Read count of objects
	int count = readInt(f);
	// load objects
	for (int i = 0; i < count; i++) {
		AtlasElement* ae = new AtlasElement;
		fread(ae, sizeof(AtlasElement), 1, f);
		elements.push_back(ae);
	}
	fclose(f);
}


AtlasElement* Atlas::getElement(const string &name) {
	AtlasElement* result = NULL;
	for (unsigned int i = 0; i < elements.size(); i++) {
		if (string(elements.at(i)->name) == name) {
			result = elements.at(i);
			break;
		}
	}
	return result;
}


vector<AtlasElement*> Atlas::getAllElements() {
	return elements;
}


Atlas::~Atlas() {
	// TODO Auto-generated destructor stub
}
