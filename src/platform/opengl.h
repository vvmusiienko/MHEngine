/*
 * opengl.h
 *
 *  Created on: 04.08.2010
 *      Author: Администратор
 */

#ifndef OPENGL_H_
#define OPENGL_H_

#include "../BuildSettings.h"

#if WINDOWS_OS
	#include <windows.h>
	//#include <GL/gl.h>
	//#include <GL/glu.h>
	#define GLEW_STATIC
	#define GLEW_BUILD
	#include <GL/glew.h>
#elif IPHONE_OS
	#include "glu.h"
    #include <OpenGLES/ES1/glext.h>
	#define		glMapBuffer		glMapBufferOES
	#define		glUnmapBuffer	glUnmapBufferOES
	#define		GL_WRITE_ONLY	GL_WRITE_ONLY_OES
#endif

#endif /* OPENGL_H_ */
