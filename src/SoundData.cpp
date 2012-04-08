/*
 * SoundData.cpp
 *
 *  Created on: 03.08.2010
 *      Author: Администратор
 */

#include "SoundData.h"
#include <stdexcept>

SoundData::SoundData(std::string path) {
	buffer = alutCreateBufferFromFile(path.c_str());
	if (buffer == AL_NONE)
		throw std::runtime_error("alutCreateBufferFromFile failed");
}


ALuint SoundData::getBuffer() {
	return buffer;
}


SoundData::~SoundData() {
	// TODO Auto-generated destructor stub
}
