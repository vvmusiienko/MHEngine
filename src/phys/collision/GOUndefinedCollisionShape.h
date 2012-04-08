/*
 * GOUndefinedCollisionShape.h
 *
 *  Created on: 07.02.2011
 *      Author: Администратор
 */

#ifndef GOUNDEFINEDCOLLISIONSHAPE_H_
#define GOUNDEFINEDCOLLISIONSHAPE_H_

#include "GOCollisionShape.h"

class GOUndefinedCollisionShape: public GOCollisionShape {

protected:
	void createShape();


public:
	virtual void save(FILE* f);

	GOUndefinedCollisionShape();
	GOUndefinedCollisionShape(FILE* f);
	virtual ~GOUndefinedCollisionShape();
};

#endif /* GOUNDEFINEDCOLLISIONSHAPE_H_ */
