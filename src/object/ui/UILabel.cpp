/*
 * UILabel.cpp
 *
 *  Created on: 09.06.2010
 *      Author: Администратор
 */

#include "UILabel.h"
#include "../../FontCache.h"
#include "../GOPlate.h"
#include "../../glfont2/glfont2.h"

using namespace glfont;

UILabel::UILabel() : UIElement() {
	init();
}


UILabel::UILabel(string id) : UIElement(id){
	init();
	bindLua();
}


void UILabel::init() {
	type = GO_LABEL;
	linesNumber = 1;
	needSize = false;
	needReposition = true;
	setPositioningMode(GOPositioningModeTopLeft);
	fontScale = 1.0f;
	fontSize = 0.0f;
	text = "";
	setFont("standart_14_normal");
	useGradientColor = false;
	bufferSize = 0;
	oldBufferSize = 0;
	setResponsible(false);
	glGenBuffers(1, &VBOBuffer);
}


void UILabel::bindLua() {
	//registrateMethods(id);
}


void UILabel::refreshBuffers() {
	GameObject::refreshBuffers();

	int linesCount = lines.size();
	// calculate vertices count
	int bufferSize = 0;
	verticesCount = 0;

	for (int i = 0; i < linesCount; i++) {
		string s = lines.at(i);
		verticesCount += font->charactersToDrawCount(s.c_str()) * 6;
	}

	if (useGradientColor) {
		bufferSize = verticesCount * sizeof(MHVertexPTC);
	} else {
		bufferSize = verticesCount * sizeof(MHVertexPT);
	}


	if (bufferSize != oldBufferSize) {
		glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, 0, GL_STATIC_DRAW);
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);
	}
	void* vertexBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (!vertexBuffer) {
		Log::warning("Can't map buffer. GOPlate(\"%s\")", id.c_str());
		return;
	}

	oldBufferSize = bufferSize;

	int bufferOffset = 0;
	float fontHeight = font->getFontHeight() * fontScale;
	for (int i = 0; i < linesCount; i++) {
		string s = lines.at(i);
		if (useGradientColor) {
			MGColor tmpGradientTopColor(gradientTopColor.getR(), gradientTopColor.getG(), gradientTopColor.getB(), 0.0f);
			MGColor tmpGradientBottomColor(gradientBottomColor.getR(), gradientBottomColor.getG(), gradientBottomColor.getB(), 0.0f);
			float a = gradientTopColor.getA() -  (1 - getAbsoluteAlpha());
			if (a > 1.0f) a = 1.0f;
			if (a < 0.0f) a = 0.0f;
			tmpGradientTopColor.setA(a);
			a = gradientBottomColor.getA() -  (1 - getAbsoluteAlpha());
			if (a > 1.0f) a = 1.0f;
			if (a < 0.0f) a = 0.0f;
			tmpGradientBottomColor.setA(a);
			font->DrawStringToBuffer((void *)((int)vertexBuffer + bufferOffset), s.c_str(), fontScale, textPositioning.x, textPositioning.y - fontHeight * i, tmpGradientTopColor.getByteComponents(), tmpGradientBottomColor.getByteComponents());
			bufferOffset += (font->charactersToDrawCount(s.c_str()) * 6) * sizeof(MHVertexPTC);
		} else {
			font->DrawStringToBuffer((void *)((int)vertexBuffer + bufferOffset), s.c_str(), fontScale, textPositioning.x, textPositioning.y - fontHeight * i);
			bufferOffset += (font->charactersToDrawCount(s.c_str()) * 6) * sizeof(MHVertexPT);
		}
	}


		if (!glUnmapBuffer(GL_ARRAY_BUFFER) ) {
#ifdef MH_DEBUG
			Log::warning("Unmap buffer failed. GOPlate(\"%s\")", id.c_str());
#endif
		}
}



void UILabel::draw() {
	if (game->debugLabels && needSize) {
		static GOPlate* p = new GOPlate();
		p->setSize(size);
		p->setPosition(getAbsolutePosition());
		p->color.setA(0.5f);
		p->setPositioningMode(getPositioningMode());
		p->draw();
	}

	if (text == "")
		return;

	if (lastAlpha != getAbsoluteAlpha()) {
		lastAlpha = getAbsoluteAlpha();
		needRefreshBuffers = true;
	}

	
	static std::pair<float, float>* textSize = new std::pair<float, float>();
	if (needReposition) {
		if (needSize) {
			textSize->first = 0.0f;
			// get widest string width
			for (unsigned int i = 0; i < lines.size(); i++) {
				string s = lines.at(i);
				static std::pair<float, float>* tmpSize = new std::pair<float, float>();
				font->GetStringSize(text.c_str(), tmpSize);
				if (tmpSize->first > textSize->first)
					textSize->first = tmpSize->first;
			}
			textSize->first = textSize->first * fontScale;
			textSize->second = font->getFontHeight() * fontScale * lines.size();
		} else {
			font->GetStringSize(text.c_str(), textSize);
			textSize->first = textSize->first * fontScale;
			textSize->second = textSize->second * fontScale;
		}
        
		textPositioning = getPositioningOffset(getPositioningMode(), textSize->first, textSize->second);
		textPositioning.y = textPositioning.y + textSize->second;
		needReposition = false;
	}
    
    
    glPushMatrix();
	GameObject::draw();

	font->Begin();
	glBindBuffer(GL_ARRAY_BUFFER, VBOBuffer);         // for vertex coordinates
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if (useGradientColor) {
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3/*size of one vertex*/,
						GL_FLOAT/*type of each component*/,
						sizeof(MHVertexPTC)/*is the byte offset between vertices*/,
						BUFFER_OFFSET(0)/*no comments:P*/);
		glTexCoordPointer(2/*size of one vertex*/,
						GL_FLOAT/*type of each component*/,
						sizeof(MHVertexPTC)/*is the byte offset between vertices*/,
						BUFFER_OFFSET(12)/*no comments:P*/);
		glColorPointer(4/*number components per color */,
						GL_UNSIGNED_BYTE/*type of each color component*/,
						sizeof(MHVertexPTC)/*is the number of bytes between consecutive colors;*/,
						BUFFER_OFFSET(20));
		glDrawArrays(GL_TRIANGLES/*type of vertices*/, 0/*offset*/, verticesCount/*count*/);
		glDisableClientState(GL_COLOR_ARRAY);
	} else {
		glVertexPointer(3/*size of one vertex*/,
						GL_FLOAT/*type of each component*/,
						sizeof(MHVertexPT)/*is the byte offset between vertices*/,
						BUFFER_OFFSET(0)/*no comments:P*/);
		glTexCoordPointer(2/*size of one vertex*/,
						GL_FLOAT/*type of each component*/,
						sizeof(MHVertexPT)/*is the byte offset between vertices*/,
						BUFFER_OFFSET(12)/*no comments:P*/);
		glDrawArrays(GL_TRIANGLES/*type of vertices*/, 0/*offset*/, verticesCount/*count*/);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glPopMatrix();
}


void UILabel::updateLines() {
	needReposition = true;

	lines.clear();

	int linesNumber = this->linesNumber;
	float fontHeight = font->getFontHeight() * fontScale;
	int realLinesNumber = (int)(size.y / fontHeight);
	if (realLinesNumber < linesNumber)
		linesNumber = realLinesNumber;

	if (needSize) {
		string text = this->text;
		for (int i = 0; i < linesNumber; i++) {
			if (text.size() <= 0)
				break;
			if (i == linesNumber - 1) { // if last line then put ... at end of line
				bool needShrink = true;
				do {
					static std::pair<float, float>* textSize = new std::pair<float, float>();
					font->GetStringSize(text.c_str(), textSize);
					textSize->first = textSize->first * fontScale;
					textSize->second = textSize->second * fontScale;

					if (textSize->first > size.x) {
						text = text.erase(text.size()-1);
						text = text.replace(text.size() - 3, 3, "...");
					} else {
						needShrink = false;
					}
				} while (needShrink);
				lines.push_back(text);
			} else { // else try break the line
				string tmps = text;

				// first try to break string at "space", else in first handy place
				bool spaceBreakSuccess = false;

				bool needShrink = true;
				do {
					static std::pair<float, float>* textSize = new std::pair<float, float>();
					font->GetStringSize(tmps.c_str(), textSize);
					textSize->first = textSize->first * fontScale;
					textSize->second = textSize->second * fontScale;

					if (textSize->first > size.x) {
						// try find "space" from end to begin
						int s_pos = tmps.find_last_of(" ");
						if (s_pos != (int)string::npos) {
							tmps = tmps.erase(s_pos);
						} else {
							needShrink = false;
						}
					} else {
						spaceBreakSuccess = true;
						needShrink = false;
					}
				} while (needShrink);

				if (!spaceBreakSuccess) { //break string in first handy place
					tmps = text;
					bool needShrink = true;
					do {
						static std::pair<float, float>* textSize = new std::pair<float, float>();
						font->GetStringSize(tmps.c_str(), textSize);
						textSize->first = textSize->first * fontScale;
						textSize->second = textSize->second * fontScale;

						if (textSize->first > size.x) {
							tmps = tmps.erase(tmps.size() - 1);
						} else {
							needShrink = false;
						}
					} while (needShrink);
				}
				lines.push_back(tmps);
				if (spaceBreakSuccess) {
					text.erase(0, tmps.size() + 1); // +1 because size of tmps not include " "
				} else {
					text.erase(0, tmps.size());
				}
			}
		}
	} else {
		lines.push_back(text);
	}
	//Log::info("lines %i ", lines.size());
}


void UILabel::update(float dt) {
	GameObject::update(dt);

}


void UILabel::setGradientColor(MGColor topColor, MGColor bottomColor) {
	useGradientColor = true;
	gradientTopColor = topColor;
	gradientBottomColor = bottomColor;
	needRefreshBuffers = true;
}


void UILabel::setFontSize(float fsize) {
	if (fsize > 0 && font != NULL) {
		fontScale = fsize / font->getFontHeight();
	}
	fontSize = fsize;
	needRefreshBuffers = true;
    needReposition = true;
}


float UILabel::getFontSize() {
	return fontSize;
}


void UILabel::setSize(CVector size) {
	this->size = size;
	needSize = true;
	updateLines();
}


void UILabel::setText(string text) {
    if (this->text != text) {
		this->text = text;
		updateLines();
		needRefreshBuffers = true;
	}
}


void UILabel::setFont(const string &fontName) {
	this->font = FontCache::getInstance()->load(fontName);
	if (font != NULL)
		setFontSize(fontSize);
	needRefreshBuffers = true;
}


void UILabel::setLinesNumber(int num) { // Default 1
	linesNumber = num;
	updateLines();
	needRefreshBuffers = true;
}


int UILabel::getLinesNumber() {
	return linesNumber;
}


int UILabel::methodsBridge(lua_State* luaVM) {
	if (isCurrentMethod("setText")) {
		setText(lua_tostring(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("setFont")) {
		setFont(lua_tostring(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("setFontSize")) {
		setFontSize(lua_tonumber(luaVM, 1));
		return 0;
	}
	if (isCurrentMethod("setSize")) {
		setSize(CVector(lua_tonumber(luaVM, 1), lua_tonumber(luaVM, 2), lua_tonumber(luaVM, 3)));
		return 0;
	}
	return UIElement::methodsBridge(luaVM);
}


UILabel::~UILabel() {
	// TODO Auto-generated destructor stub
}
