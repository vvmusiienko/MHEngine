/*
 * SoundData.h
 *
 *  Created on: 03.08.2010
 *      Author: Администратор
 */

#ifndef SOUNDDATA_H_
#define SOUNDDATA_H_

#include <string>
#include "./platform/openal.h"

class SoundData {
private:
	SoundData();
	ALuint buffer;

public:

	ALuint getBuffer();

	SoundData(std::string path);
	virtual ~SoundData();
};

#endif /* SOUNDDATA_H_ */
