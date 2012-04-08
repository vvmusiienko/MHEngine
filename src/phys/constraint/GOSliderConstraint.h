/*
 * GOSliderConstraint.h
 *
 *  Created on: 14.02.2011
 *      Author: Администратор
 */

#ifndef GOSLIDERCONSTRAINT_H_
#define GOSLIDERCONSTRAINT_H_

#include "GOConstraint.h"

class GOSliderConstraint: public GOConstraint {

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
	GOSliderConstraint();
	GOSliderConstraint(string id);
	GOSliderConstraint(FILE* f);
	virtual ~GOSliderConstraint();
};

#endif /* GOSLIDERCONSTRAINT_H_ */
