#ifndef IOSADAPTER_H_
#define IOSADAPTER_H_

#include <string>

using namespace std;

class IOSAdapter
{
public:
	IOSAdapter();
	
	// swap buffers function. called after al rendering precess is done
	virtual void swapBuffers() = 0;
	// return tick count in ms
	virtual unsigned long int getTickCount() = 0;
	// get full path to application folder
	virtual string getWorkingPath() = 0;
	// get full path to folder with resources
	virtual string getResourcePath() = 0;
	// terminate the application
	virtual void terminate() = 0;
	// Show system message
	virtual void showMessage(const string title, const string text) = 0;
	//
	virtual int getScreenWidth() = 0;
	//
	virtual int getScreenHeight() = 0;
	//
	virtual float getViewportWidth() = 0;
	//
	virtual float getViewportHeight() = 0;
    
    
	
	virtual float getFixedTimeStep() {
		return 30.0f;
	}
	
	virtual ~IOSAdapter();
};

#endif /*IOSADAPTER_H_*/
