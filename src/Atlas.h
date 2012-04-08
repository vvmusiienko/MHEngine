/*
 * Atlas.h
 *
 *  Created on: 19.12.2010
 *      Author: Администратор
 */

#ifndef ATLAS_H_
#define ATLAS_H_

#include <string>
#include <vector>

using namespace std;

struct AtlasElement {
	char name[31]; // max name lenght is 30
	float u;
	float v;
	float uh;
	float vh;
};


class Atlas {
private:
	Atlas();
	vector<AtlasElement*> elements;

public:

	AtlasElement* getElement(const string &name);
	vector<AtlasElement*> getAllElements();

	Atlas(string path);
	virtual ~Atlas();
};

#endif /* ATLAS_H_ */
