/*
 * UIElement.h
 *
 *  Created on: 08.06.2010
 *      Author: Администратор
 */

#ifndef UIELEMENT_H_
#define UIELEMENT_H_

#include "UIResponder.h"
#include "../GameObject.h"


enum UIContentMode {
	UIContentModeCenter,
	UIContentModeFill
};


class UIElement : public GameObject, public UIResponder {

protected:
	CVector size;


protected:
	virtual void onAddedToWorld(); // called while this object processed by MapManager::addObject
	virtual void onWillRemoveFromWorld(); // called while this object removed from game queue
	bool disabled;
	UIContentMode contentMode;

private:
	void init();
	void bindLua();

public:

	void setContentMode(UIContentMode contentMode);
	UIContentMode getContentMode();
	void setDisabled(bool v);
	bool getDisabled();

	virtual bool hitTest(CursorState c);
	virtual float responseOrder();

	UIElement();
	UIElement(string id);
	virtual ~UIElement();
};

#endif /* UIELEMENT_H_ */
