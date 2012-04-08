/*
 * GOSkyBox.h
 *
 *  Created on: 18.03.2010
 *      Author: Администратор
 */

#ifndef GOSKYBOX_H_
#define GOSKYBOX_H_

#include "GameObject.h"
#include "GOPlate.h"

class GOSkyBox: public GameObject {
	GOPlate* p;
	const static int width = 1800 * 5;
	const static int height = 1800 * 5;
public:

	void draw();
	void update(float dt);
	void save(FILE* f);

	GOSkyBox(string texture);
	virtual ~GOSkyBox();
};

#endif /* GOSKYBOX_H_ */
