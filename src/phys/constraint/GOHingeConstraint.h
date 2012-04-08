/*
 * GOHingeConstraint.h
 *
 *  Created on: 14.02.2011
 *      Author: Администратор
 */

#ifndef GOHINGECONSTRAINT_H_
#define GOHINGECONSTRAINT_H_

#include "GOConstraint.h"

class GOHingeConstraint: public GOConstraint {

protected:
	CVector axis;
	CVector secondAxis;

	void init();
	void bindLua();
	virtual int methodsBridge(lua_State* luaVM);

public:

	void setAxis(CVector v);
	CVector getAxis();
	void setSecondAxis(CVector v);
	CVector getSecondAxis();

	virtual void createConstraint();
	virtual void save(FILE* f);
	GOHingeConstraint();
	GOHingeConstraint(string id);
	GOHingeConstraint(FILE* f);
	virtual ~GOHingeConstraint();
};

#endif /* GOHINGECONSTRAINT_H_ */
