/*
 * UILabel.h
 *
 *  Created on: 09.06.2010
 *      Author: Администратор
 */

#ifndef UILABEL_H_
#define UILABEL_H_

#include "UIElement.h"
#include <vector>

namespace glfont
{
	class GLFont;
}

class UILabel: public UIElement {
private:
	string text;
	glfont::GLFont* font;
	float fontScale;
	float fontSize;
	bool useGradientColor;
	MGColor gradientTopColor;
	MGColor gradientBottomColor;
	float lastAlpha;

	int linesNumber;
	bool needSize; // default false; if setSize called then become true
	std::vector<string> lines;

	bool needReposition;
	CVector textPositioning;

	int bufferSize;
	int oldBufferSize;

private:
	void init();
	void bindLua();
	void updateLines();

public:

	void setLinesNumber(int num); // Default 1
	int getLinesNumber();
	void setText(string text);
	void setFont(const string &fontName);
	void setSize(CVector size);
	void setFontSize(float size); // Set font height in pixels
	float getFontSize(); // get font height in pixels
	void setGradientColor(MGColor topColor, MGColor bottomColor);

	void draw();
	void update(float dt);
	void refreshBuffers();
	virtual int methodsBridge(lua_State* luaVM);

public:
	UILabel();
	UILabel(string id);
	virtual ~UILabel();
};

#endif /* UILABEL_H_ */
