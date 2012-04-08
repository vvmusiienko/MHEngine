/*
 * Console.h
 *
 *  Created on: 25.01.2010
 *      Author: Администратор
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <string>

using namespace std;

class Game;

class Console {
private:
	Game* game;
	char lastTypedChar;
	string lastTypedText;
	string lastCommand; // if KEY_UP the return last command;
	string currentTypedText;
	bool opened;
public:

	void init();
	void draw();
	void open();
	bool isOpened();
	void addString(string s);

	// KeyListener realization
	void onKeyDown(unsigned char key);
	void onKeyUp(unsigned char key);
	void onCharEntered(unsigned char c);

	Console(Game* game);
	virtual ~Console();
};

#endif /* CONSOLE_H_ */
