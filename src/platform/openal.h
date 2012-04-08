/*
 * openal.h
 *
 *  Created on: 03.08.2010
 *      Author: Администратор
 */

#ifndef OPENAL_H_
#define OPENAL_H_


#if WINDOWS_OS
	#include <al/al.h>
	#include <al/alc.h>
	#include <al/alut.h>
#elif IPHONE_OS
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
	#include "../alut/alut.h"
#endif


#endif /* OPENAL_H_ */
