#ifndef BUILDSETTINGS_H_
#define BUILDSETTINGS_H_

#define MH_DEBUG		1

//#define		IPHONE_OS		1
//#define		WINDOWS_OS		1

#if WINDOWS_OS
#include "platform/WindowsBuildSettings.h"
#elif IPHONE_OS
#include "platform/IPhoneBuildSettings.h"
#endif

#endif /*BUILDSETTINGS_H_*/
