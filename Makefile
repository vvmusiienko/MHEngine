CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

SRCS =		MapManager.cpp MGConfig.cpp LuaVM.cpp Console.cpp main.cpp Game.cpp GraphicEngine.cpp Log.cpp KeyListener.cpp Camera.cpp MouseListener.cpp Common.cpp \
			.\object\GameObject.cpp .\object\GOCube.cpp .\object\GOPlate.cpp \
			.\libs\glfont2\glfont2.cpp .\platform\WindowsOSAdapter.cpp .\platform\IOSAdapter.cpp \
			.\libs\md2\Md2Model.cpp Texture.cpp \
			ImageTGA.cpp .\object\GOModel.cpp .\object\GOLight.cpp .\object\GOPlayer.cpp

LIBS =		user32.lib advapi32.lib gdi32.lib opengl32.lib glu32.lib glew32.lib lua52.lib

TARGET =	start.exe

all:
	SET INCLUDE=%INCLUDE%;platform;object;libs;libs\glfont2;libs\md2;libs\lua;
	cl $(SRCS) $(LIBS) /Fe"$(TARGET)" /EHsc
	
	
clean:
	del /q $(OBJS) $(TARGET)


	