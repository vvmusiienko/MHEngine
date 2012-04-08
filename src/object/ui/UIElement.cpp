/*
 * UIElement.cpp
 *
 *  Created on: 08.06.2010
 *      Author: Администратор
 */

#include "UIElement.h"


UIElement::UIElement() : GameObject(), UIResponder() {
	init();
}


UIElement::UIElement(string id) : GameObject(id), UIResponder() {
	init();
	bindLua();
}


void UIElement::init() {
	contentMode = UIContentModeCenter;
	disabled = false;
	setResponsible(false);
	is2D = true;
	enablePhysics(false);
}


void UIElement::setContentMode(UIContentMode contentMode) {
	this->contentMode = contentMode;
}


UIContentMode UIElement::getContentMode() {
	return contentMode;
}


void UIElement::setDisabled(bool v) {
	disabled = v;
}


bool UIElement::getDisabled() {
	return disabled;
}


void UIElement::onAddedToWorld() { // called while this object processed by MapManager::addObject
	setResponsible(true);
}


void UIElement::onWillRemoveFromWorld() { // called while this object removed from game queue
	setResponsible(false);
}


void UIElement::bindLua() {

}


bool UIElement::hitTest(CursorState c) {
	if (!getNeedUpdate() || !getNeedDraw() || disabled)
		return false;

	CVector realPosition = getPositioningOffset(getPositioningMode(), size.x, size.y) + getAbsolutePosition();
	if (c.x >= realPosition.x && c.x <= realPosition.x + size.x
			&& c.y >= realPosition.y && c.y <= realPosition.y + size.y)
	{
		return true;
	}
	return false;
}


float UIElement::responseOrder() {
	return getAbsolutePosition().z;
}



UIElement::~UIElement() {
	// TODO Auto-generated destructor stub
}
