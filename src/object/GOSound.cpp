/*
 * GOSound.cpp
 *
 *  Created on: 31.07.2010
 *      Author: Администратор
 */

#include "GOSound.h"
#include "../SoundCache.h"


GOSound::GOSound() : GameObject() {
	init();
}


GOSound::GOSound(string id) : GameObject(id) {
	init();
	bindLua();
}


GOSound::GOSound(FILE* f) : GameObject(f) {

}


void GOSound::init() {
	canPlay = false;
	soundName = "";

	alGenSources(1, &source);
    ALfloat sourceVel[] = { 0.0, 0.0, 0.0 };
    alSourcef(source, AL_PITCH,    1.0f     );
    alSourcef(source, AL_GAIN,     1.0f     );
    alSourcefv(source, AL_VELOCITY, sourceVel);
	alSourcei(source, AL_LOOPING,  false);

    ALenum error = alGetError();
	if (error != AL_NO_ERROR)
    	Log::warning("error create sound (%s)", alutGetErrorString(error));
}


void GOSound::bindLua() {

}


void GOSound::setSound(string name) {
	soundData = SoundCache::getInstance()->load(name);
	if (soundData) {
		soundName = name;
		alSourcei (source, AL_BUFFER, soundData->getBuffer());
		canPlay = true;
	} else {
		soundName = "";
		canPlay = false;
	}
}


string GOSound::getSoundName() {
	return soundName;
}


void GOSound::setLoopCount(int count) { // -1 - one time play, 0 - infinity play, X = X play. Default -1
	if (count == -1)
		alSourcei (source, AL_LOOPING,  false);
    if (count == 0)
		alSourcei (source, AL_LOOPING,  true);
}


void GOSound::play() {
	if (canPlay)
		alSourcePlay(source);
}


void GOSound::stop() {
	if (canPlay)
		alSourceStop(source);
}


void GOSound::pause() {
	if (canPlay)
		alSourcePause(source);
}


bool GOSound::isPlaying() {
	return true;
}


void GOSound::draw() {

}


void GOSound::update(float dt) {
	GameObject::update(dt);
	ALfloat sourcePos[] = { getPosition().x, getPosition().y, getPosition().z };
    alSourcefv(source, AL_POSITION, sourcePos);
}


void GOSound::save(FILE* f) {

}


GOSound::~GOSound() {
	// TODO Auto-generated destructor stub
}
