/*
 * GOSound.h
 *
 *  Created on: 31.07.2010
 *      Author: Администратор
 */

#ifndef GOSOUND_H_
#define GOSOUND_H_

#include "GameObject.h"
#include "../platform/openal.h"
#include "../SoundData.h"

class GOSound: public GameObject {
private:
	ALuint source;
	SoundData* soundData;
	bool canPlay;
	string soundName;

	void init();
	void bindLua();
public:

	void setSound(string name);
	string getSoundName();
	void setLoopCount(int count); //0 - infinity play, X = X play. Default 1
	void play();
	void stop();
	void pause();
	bool isPlaying();

	void draw();
	void update(float dt);
	void save(FILE* f);

	GOSound();
	GOSound(string id);
	GOSound(FILE* f);
	virtual ~GOSound();
};

#endif /* GOSOUND_H_ */
