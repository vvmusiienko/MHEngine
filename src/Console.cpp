/*
 * Console.cpp
 *
 *  Created on: 25.01.2010
 *      Author: Администратор
 */

#include "Console.h"
#include "Game.h"
#include "object/GOPlate.h"
#include "GraphicEngine.h"
#include "MGConfig.h"

Console::Console(Game* game) {
	this->game = game;
	lastTypedChar = 0;
	lastTypedText = string(1, KEY_RETURN);
	currentTypedText = string(1, KEY_RETURN);
	opened = false;
}


void Console::init()
{
	lastCommand = game->config->getString("last_console_command");
}


void Console::onCharEntered(unsigned char c)
{
	if (c == KEY_BACKSPACE)
		return;

	if (c == '`') {
		opened = false;
		return;
	}

	if (c == KEY_RETURN && lastTypedChar == KEY_RETURN) {
		lastTypedText += currentTypedText;
		game->luaVM->doString(currentTypedText);
		lastCommand = currentTypedText;
		game->config->setString("last_console_command", lastCommand);
		currentTypedText = "";
	}

	currentTypedText = currentTypedText + string(1, c);
	lastTypedChar = c;
}

void Console::onKeyDown(unsigned char key)
{
	if (key == KEY_BACKSPACE && currentTypedText.length() > 0) { // backspace
		currentTypedText.resize(currentTypedText.length() - 1);
	}

	if (key == KEY_UP) {
		currentTypedText = lastCommand;
	}
}

void Console::onKeyUp(unsigned char key)
{
}

void Console::draw() {
#ifndef OPENGL_ES
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
#endif
	// save previous state of texturing
	bool lightingEnabled = glIsEnabled(GL_LIGHTING);
	glDisable(GL_LIGHTING);

	// Write 2D stuff
    glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	    glPushMatrix();
	    glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glOrtho(0, game->osAdapter->getScreenWidth(), 0, game->osAdapter->getScreenHeight(), -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable (GL_DEPTH_TEST);

		// draw shadow
		static GOPlate* p = new GOPlate();
		p->setSize(CVector(game->osAdapter->getScreenWidth(), game->osAdapter->getScreenHeight(), 0));
		p->setPosition(CVector(game->osAdapter->getScreenWidth() / 2, game->osAdapter->getScreenHeight() / 2, 0));
		p->color = MGColor(0.0f, 0.0f, 0.0f ,0.6f);
		p->setOrientation(CVector(0,0,0));
		p->draw();


		float heightOffset = 50;
		int lastStringSize = 0;

		for (unsigned int i = 0; i < currentTypedText.length(); i++) {

			int currentCharPosition = currentTypedText.length() - 1 - i;

			if (currentTypedText[currentCharPosition] == KEY_RETURN) {
				string currentString = currentTypedText.substr(currentCharPosition, currentTypedText.length() - currentCharPosition - lastStringSize);

				game->graphicEngine->writeText(currentString, CVector(10, heightOffset, 0), CONSOLE_8_NORMAL, MGColor::greenColor());
				heightOffset += 15;
				lastStringSize += currentTypedText.length() - currentCharPosition - lastStringSize;
			}
		}

		lastStringSize = 0;
		{
			for (unsigned int i = 0; i < lastTypedText.length(); i++) {

				int currentCharPosition = lastTypedText.length() - 1 - i;

				if (lastTypedText[currentCharPosition] == KEY_RETURN) {
					string currentString = lastTypedText.substr(currentCharPosition, lastTypedText.length() - currentCharPosition - lastStringSize);

					game->graphicEngine->writeText(currentString, CVector(10, heightOffset, 0), CONSOLE_8_NORMAL, MGColor::redColor());
					heightOffset += 16;
					lastStringSize += lastTypedText.length() - currentCharPosition - lastStringSize;
				}
			}
		}
	glEnable (GL_DEPTH_TEST); // Включаем тест глубины
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	if (lightingEnabled) {
		glEnable(GL_LIGHTING);
	} else {
		glDisable(GL_LIGHTING);
	}
}

void Console::open()
{
	opened = true;
}


bool Console::isOpened()
{
	return opened;
}

void Console::addString(string s)
{
	lastTypedText += s;
}


Console::~Console() {
	// TODO Auto-generated destructor stub
}
