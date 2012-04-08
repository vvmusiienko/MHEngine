/*
 * GOP2PConstraint.h
 *
 *  Created on: 14.02.2011
 *      Author: Администратор
 */

#ifndef GOP2PCONSTRAINT_H_
#define GOP2PCONSTRAINT_H_

#include "GOConstraint.h"

class GOP2PConstraint: public GOConstraint {

private:

protected:

	void init();
	void bindLua();
	virtual int methodsBridge(lua_State* luaVM);
public:

	virtual void createConstraint();
	virtual void save(FILE* f);
	GOP2PConstraint();
	GOP2PConstraint(string id);
	GOP2PConstraint(FILE* f);
	virtual ~GOP2PConstraint();
};

#endif /* GOP2PCONSTRAINT_H_ */
