/*
 * UIButton.cpp
 *
 *  Created on: 08.06.2010
 *      Author: Администратор
 */

#include "UIButton.h"
#include "../GOPlate.h"
#include "../../Texture.h"
#include "../../TextureCache.h"
#include "UILabel.h"
#include "../GOSound.h"
#include "../animation/MHAlphaAnimation.h"

UIButton::UIButton() : UIElement() {
	init();
}


UIButton::UIButton(string id) : UIElement(id) {
	init();
	bindLua();
}


void UIButton::init() {
	normalTexture = NULL;
	activeTexture = NULL;
	disabledTexture = NULL;
	buttonOverSound = NULL;
	buttonDownSound = NULL;
	buttonUpSound = NULL;
	pressed = false;
	clickAction = NULL;
	downAction = NULL;
	upAction = NULL;
	overAction = NULL;
	leaveAction = NULL;
    isAlphaTransitionEnabled = false;
    transitionDuration = 0.5f;
    overPlate = NULL;
    transFlag = false;
    
	plate = new GOPlate();
	plate->setAutoSize(true);
	plate->setPositioningMode(GOPositioningModeCenter);
	this->addChild(plate);
    
    

	normalLabel = new UILabel();
	normalLabel->setPositioningMode(GOPositioningModeCenter);
	this->addChild(normalLabel);
	activeLabel = new UILabel();
	activeLabel->setPositioningMode(GOPositioningModeCenter);
	this->addChild(activeLabel);
	disabledLabel = new UILabel();
	disabledLabel->setPositioningMode(GOPositioningModeCenter);
	this->addChild(disabledLabel);

	setPositioningMode(GOPositioningModeBottomLeft);
}


void UIButton::enableAlphaTransition(float duration) {
    isAlphaTransitionEnabled = true;
    transitionDuration = duration;
    // init over plate
    overPlate = new GOPlate();
    overPlate->setEnabled(true);
    overPlate->setSize(plate->getSize());
	overPlate->setAutoSize(plate->getAutoSize());
	overPlate->setPositioningMode(GOPositioningModeCenter);
    overPlate->setAlpha(0.0f);
	this->addChild(overPlate);
}


void UIButton::bindLua() {
	//registrateMethods(id);
}


void UIButton::setButtonClickAction(Action* a) {
	clickAction = a;
}


void UIButton::setButtonDownAction(Action* a) {
	downAction = a;
}


void UIButton::setButtonUpAction(Action* a) {
	upAction = a;
}


void UIButton::setButtonOverAction(Action* a) {
	overAction = a;
}


void UIButton::setButtonLeaveAction(Action* a) {
	leaveAction = a;
}



void UIButton::draw() {
	glPushMatrix();
	GameObject::draw();
	glPopMatrix();

	color = MGColor::whiteColor();
	if (!disabled) {
		if (pressed) {
			if (activeTexture == NULL)
				color = MGColor(1.0f, 1.0f, 1.0f, 0.5f);
			if (isAlphaTransitionEnabled) {
                if (!transFlag) {
                    overPlate->setTexture(activeTexture);
                    overPlate->addAnimation(new MHAlphaAnimation(1.0f, transitionDuration));
                    transFlag = true;
                }
            } else {
                plate->setTexture(activeTexture);
            }
		} else {
			if (normalTexture == NULL)
				color = MGColor::transparentColor();
			if (isAlphaTransitionEnabled && transFlag) {
                    overPlate->setTexture(activeTexture);
                    overPlate->addAnimation(new MHAlphaAnimation(0.0f, transitionDuration));
                    transFlag = false;
            } else {
                plate->setTexture(normalTexture);
            }
		}
	} else {
		if (disabledTexture == NULL) {
			if (normalTexture != NULL) {
				plate->setTexture(normalTexture);
				color = MGColor(1.0f, 1.0f, 1.0f, 0.7f);
			}
		} else {
			plate->setTexture(disabledTexture);
		}
	}

	plate->color = color;
	plate->draw();
    if (overPlate) {
        overPlate->color = color;
        overPlate->draw();
    }
}


void UIButton::update(float dt) {
	GameObject::update(dt);

    if (overPlate) {
        overPlate->update(dt);
    }
    
    
	if (!disabled) {
		if (pressed) {
			normalLabel->setHidden(true);
			activeLabel->setHidden(false);
			disabledLabel->setHidden(true);
		} else {
			normalLabel->setHidden(false);
			activeLabel->setHidden(true);
			disabledLabel->setHidden(true);
		}
	} else {
		normalLabel->setHidden(true);
		activeLabel->setHidden(true);
		disabledLabel->setHidden(false);
	}

	CVector offset = getPositioningOffset(getPositioningMode(), size.x, size.y) + CVector(size.x / 2.0f, size.y / 2.0f);
	plate->setPosition(offset);
    if (overPlate) overPlate->setPosition(offset);
	normalLabel->setPosition(offset);
	activeLabel->setPosition(offset);
	disabledLabel->setPosition(offset);

	CVector tmpPos = normalLabel->getPosition();
	tmpPos.z = 1.0f;
	normalLabel->setPosition(tmpPos);
	tmpPos = activeLabel->getPosition();
	tmpPos.z = 1.0f;
	activeLabel->setPosition(tmpPos);
	tmpPos = disabledLabel->getPosition();
	tmpPos.z = 1.0f;
	disabledLabel->setPosition(tmpPos);
}


void UIButton::setContentMode(UIContentMode contentMode) {
	this->contentMode = contentMode;
	switch (contentMode) {
	case UIContentModeCenter:
		plate->setSize(size);
		plate->setAutoSize(true);
		break;
	case UIContentModeFill:
		plate->setAutoSize(false);
		plate->setSize(size);
		break;
	}
}


void UIButton::setPositioningMode(GOPositioningMode v) {
	GameObject::setPositioningMode(v);

}

void UIButton::cursorDown(CursorState c) {
	pressed = true;
	if (buttonDownSound) {
		buttonDownSound->play();
	}
	if (downAction)
		downAction->execute(this);
}


void UIButton::cursorMoved(CursorState c) {
	CVector realPosition = getPositioningOffset(getPositioningMode(), size.x, size.y) + getAbsolutePosition();
	if (c.x >= realPosition.x && c.x <= realPosition.x + size.x
			&& c.y >= realPosition.y && c.y <= realPosition.y + size.y)
	{
		pressed = true;
	} else {
		pressed = false;
	}
}


void UIButton::cursorUp(CursorState c) {
	if (pressed) {
		if (buttonUpSound) {
			buttonUpSound->play();
		}
		if (clickAction)
			clickAction->execute(this);
		if (luaBinded)
			game->luaVM->doString(FormatString("se(%s.onClick, %s)", id.c_str(), id.c_str()));
	}
	if (upAction)
		upAction->execute(this);

	pressed = false;
}


void UIButton::cursorOver(CursorState c) {
	if (presByOver)
		pressed = true;

	if (overAction)
		overAction->execute(this);
}


void UIButton::cursorLeave(CursorState c) {
	if (presByOver)
		pressed = false;
	if (leaveAction)
		leaveAction->execute(this);
}



void UIButton::setButtonOverSound(string name) {
	if (buttonOverSound == NULL) {
		buttonOverSound = new GOSound();
	}
	buttonOverSound->setSound(name);
}


void UIButton::setButtonDownSound(string name) {
	if (buttonDownSound == NULL) {
		buttonDownSound = new GOSound();
	}
	buttonDownSound->setSound(name);
}


void UIButton::setButtonUpSound(string name) {
	if (buttonUpSound == NULL) {
		buttonUpSound = new GOSound();
	}
	buttonUpSound->setSound(name);
}


GOSound* UIButton::getButtonOverSound() {
	return buttonOverSound;
}


GOSound* UIButton::getButtonDownSound() {
	return buttonDownSound;
}


GOSound* UIButton::getButtonUpSound() {
	return buttonUpSound;
}


void UIButton::setNormalTexture(string tex) {
	normalTexture = TextureCache::getInstance()->load(tex);
}


void UIButton::setActiveTexture(string tex) {
	activeTexture = TextureCache::getInstance()->load(tex);
}


void UIButton::setDisabledTexture(string tex) {
	disabledTexture = TextureCache::getInstance()->load(tex);
}

Texture* UIButton::getNormalTexture() {
	return normalTexture;
}

Texture* UIButton::getActiveTexture() {
	return activeTexture;
}

Texture* UIButton::getDisabledTexture() {
	return disabledTexture;
}

void UIButton::setArea(float x, float y, float w, float h) {
	plate->setAutoSize(false);
    plate->setArea(CVector(x,y), CVector(w,h));
}

void UIButton::setSize(CVector size) {
	plate->setAutoSize(false);
	this->size = size;
	plate->setSize(size);
    if (overPlate) {
        overPlate->setAutoSize(false);
        overPlate->setSize(size);
    }
}


UILabel* UIButton::getNormalLabel() {
	return normalLabel;
}

UILabel* UIButton::getActiveLabel() {
	return activeLabel;
}

UILabel* UIButton::getDisabledLabel() {
	return disabledLabel;
}


void UIButton::setNeedPressByOver(bool v) {
	presByOver = v;
}


bool UIButton::getNeedPressByOver() {
	return presByOver;
}


/*void UIButton::setNormalLabel(UILabel* l) {
	normalLabel = l;
	normalLabel->setPositioningMode(GOPositioningModeCenter);
}

void UIButton::setActiveLabel(UILabel* l) {
	activeLabel = l;
	activeLabel->setPositioningMode(GOPositioningModeCenter);
}

void UIButton::setDisabledLabel(UILabel* l) {
	disabledLabel = l;
	disabledLabel->setPositioningMode(GOPositioningModeCenter);
}*/


void UIButton::onAddedToWorld() { // called while this object processed by MapManager::addObject
	UIElement::onAddedToWorld();
	normalLabel->addToWorld();
	activeLabel->addToWorld();
	disabledLabel->addToWorld();
}


void UIButton::onWillRemoveFromWorld() { // called while this object removed from game queue
	UIElement::onWillRemoveFromWorld();
	/*normalLabel->kill();
	activeLabel->kill();
	disabledLabel->kill();*/
}


int UIButton::methodsBridge(lua_State* luaVM) {

	if (isCurrentMethod("setNormalTexture")) {
		setNormalTexture(lua_tostring(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("setActiveTexture")) {
		setActiveTexture(lua_tostring(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("setDisabledTexture")) {
		setDisabledTexture(lua_tostring(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("setSize")) {
		setSize(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	return UIElement::methodsBridge(luaVM);
}


UIButton::~UIButton() {
	// TODO Auto-generated destructor stub
}
