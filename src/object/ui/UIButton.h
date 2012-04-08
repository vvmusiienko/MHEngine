/*
 * UIButton.h
 *
 *  Created on: 08.06.2010
 *      Author: Администратор
 */

#ifndef UIBUTTON_H_
#define UIBUTTON_H_

#include "UIElement.h"

class GOPlate;
class Texture;
class UILabel;
class GOSound;

class UIButton : public UIElement {
private:
	GOSound* buttonOverSound;
	GOSound* buttonDownSound;
	GOSound* buttonUpSound;


	Texture* normalTexture;
	Texture* activeTexture;
	Texture* disabledTexture;
	GOPlate* plate;
	GOPlate* overPlate;
	bool pressed; // true if currently pressed
	bool presByOver; // if true button press if cursor over
	// text
	UILabel* normalLabel;
	UILabel* activeLabel;
	UILabel* disabledLabel;

	// Actions
	Action* clickAction;
	Action* downAction;
	Action* upAction;
	Action* overAction;
	Action* leaveAction;

    bool isAlphaTransitionEnabled;
    float transitionDuration;
    bool transFlag;
    

private:
	void init();
	void bindLua();

public:

	void setNormalTexture(string tex);
	void setActiveTexture(string tex);
	void setDisabledTexture(string tex);
	Texture* getNormalTexture();
	Texture* getActiveTexture();
	Texture* getDisabledTexture();

	void setButtonOverSound(string name);
	void setButtonDownSound(string name);
	void setButtonUpSound(string name);
	GOSound* getButtonOverSound();
	GOSound* getButtonDownSound();
	GOSound* getButtonUpSound();

    void enableAlphaTransition(float duration);
	void setSize(CVector size);
	void setArea(float x, float y, float w, float h);
	void setNeedPressByOver(bool v);
	bool getNeedPressByOver();
	void setContentMode(UIContentMode contentMode);
	void setButtonClickAction(Action* a);
	void setButtonDownAction(Action* a);
	void setButtonUpAction(Action* a);
	void setButtonOverAction(Action* a);
	void setButtonLeaveAction(Action* a);

	// TEXT
	UILabel* getNormalLabel();
	UILabel* getActiveLabel();
	UILabel* getDisabledLabel();
	/*void setNormalLabel(UILabel* l);
	void setActiveLabel(UILabel* l);
	void setDisabledLabel(UILabel* l);*/


	// Overwrite
	virtual void onAddedToWorld(); // called while this object processed by MapManager::addObject
	virtual void onWillRemoveFromWorld(); // called while this object removed from game queue

	virtual void setPositioningMode(GOPositioningMode v);

	void draw();
	void update(float dt);
	virtual int methodsBridge(lua_State* luaVM);

	virtual void cursorDown(CursorState c);
	virtual void cursorMoved(CursorState c);
	virtual void cursorUp(CursorState c);
	virtual void cursorOver(CursorState c);
	virtual void cursorLeave(CursorState c);

	UIButton();
	UIButton(string id);
	virtual ~UIButton();
};

#endif /* UIBUTTON_H_ */
