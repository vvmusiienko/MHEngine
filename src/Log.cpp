#include "Log.h"
#include <stdio.h>
#include <stdarg.h>
#include "Game.h"
#include "Console.h"

FILE* Log::f;
Game* Log::game;

Log::Log(Game* game)
{
	Log::game = game; 
	try {
		f = fopen((game->osAdapter->getWorkingPath() + "Log.html").c_str(), "w" );
	    fputs( "<HTML><HEAD><TITLE> MuhaSoft Inc. 3D Game Log </TITLE></HEAD><BODY>", f);
	    fflush(f);
	    fputs("3DGame log. build date ", f);
	    fputs(__DATE__, f);
	    fputs(", version ", f);
	    fputs(C_VERSION, f);
	    fflush(f);
	} catch (...) {
		printf("Log::Log(Game* game): error create log system");
		game->osAdapter->showMessage("error", "Log::Log(Game* game): error create log system");
		game->osAdapter->terminate();
	}
}


void Log::error(string format, ...)
{
	char buffer[2048];
	va_list arg;
	int ret;
	
	// Format the text
	va_start (arg, format);
	  ret = vsnprintf(buffer, sizeof (buffer), format.c_str(), arg);
	va_end (arg);
	
	string msg = FormatString("[ERROR][%i] %s", game->osAdapter->getTickCount() - game->startTime, buffer);

	printf("%s", msg.c_str());
	printf("\n");
	string html;
	html += "<font size=\"1\" face=\"Verdana\" color=\"#FF0000\"> <LI>";
	html += msg;
	html += "</LI> </font>";
	fputs(html.c_str() , f);
    fflush(f);
	game->osAdapter->showMessage("error", buffer);
	game->osAdapter->terminate();
}


void Log::warning(string format, ...)
{
	char buffer[2048];
	va_list arg;
	int ret;
	
	// Format the text
	va_start (arg, format);
	  ret = vsnprintf(buffer, sizeof (buffer), format.c_str(), arg);
	va_end (arg);

	string msg = FormatString("[WARNING][%i] %s", game->osAdapter->getTickCount() - game->startTime, buffer);

	printf("%s", msg.c_str());
	printf("\n");
	string html;
	html += "<font size=\"1\" face=\"Verdana\"> <LI>";
	html += msg;
	html += "</LI> </font>";
	fputs(html.c_str() , f);
    fflush(f);	
    if (game->console)
    	game->console->addString(msg + string(1,13));
}


void Log::info(string format, ...)
{
	char buffer[2048];
	va_list arg;
	int ret;
	
	// Format the text
	va_start (arg, format);
	  ret = vsnprintf(buffer, sizeof (buffer), format.c_str(), arg);
	va_end (arg);

	string msg = FormatString("[INFO][%i] %s", game->osAdapter->getTickCount() - game->startTime, buffer);

	printf("%s", msg.c_str());
	printf("\n");
	string html;
	html += "<font size=\"1\" face=\"Verdana\" color=\"#0000FF\"> <LI>";
	html += msg;
	html += "</LI> </font>";
	fputs(html.c_str() , f);
    fflush(f);
    if (game->console)
    	game->console->addString(msg + string(1,13));
}


Log::~Log()
{
    fputs( "</BODY></HTML>", f);
    fclose(f);
}
