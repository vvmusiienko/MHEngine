#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>
#include <string>
#include "BuildSettings.h"
#include <ostream>

using namespace std;

class Game;

class Log
{
private:
    static FILE *f;
    static Game* game;
public:
	
	static void error(string format, ...);
	static void warning(string format, ...);
	static void info(string format, ...);
	
	Log(Game* game);
	virtual ~Log();
};

#endif /*LOG_H_*/
