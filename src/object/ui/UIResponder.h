/*
 * UIResponder.h
 *
 *  Created on: 08.06.2010
 *      Author: Администратор
 */

#ifndef UIRESPONDER_H_
#define UIRESPONDER_H_

#include "../../Common.h"


class UIResponder {
	friend class Game;
private:
	bool responsible;

	// internal data
	int isDownByCursor;
	bool isSomeCursorOver;
	bool cursorsOver[15];

public:

	virtual bool hitTest(CursorState c);
	virtual void cursorDown(CursorState c);
	virtual void cursorMoved(CursorState c);
	virtual void cursorUp(CursorState c);
	virtual void cursorOver(CursorState c);
	virtual void cursorLeave(CursorState c);

	virtual float responseOrder();

	bool getResponsible();
	void setResponsible(bool v);

	UIResponder();
	virtual ~UIResponder();
};

#endif /* UIRESPONDER_H_ */
