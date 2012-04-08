/*
 * SoundCache.h
 *
 *  Created on: 03.08.2010
 *      Author: Администратор
 */

#ifndef SOUNDCACHE_H_
#define SOUNDCACHE_H_

#include "DataManager.h"
#include "SoundData.h"
#include "Game.h"


class SoundCache :
  public DataManager<SoundData, SoundCache>
{
  friend class DataManager<SoundData, SoundCache>;

public:
  // Load and register a texture.  If the texture has already been
  // loaded previously, return it instead of loading it.
  SoundData *load (const string &filename)
  {
    // Look for the object in the registry
	  SoundData *obj = request (filename);

    // If not found, load the object
    if (obj == NULL)
    {
    	try {
        	obj = new SoundData(Game::instance->osAdapter->getResourcePath() + "sounds/" + filename);
		    registerObject (filename, obj);
		}catch (std::exception& e) {
			Log::warning("Couldn't create sound from %s Reason: %s", filename.c_str(), e.what());
		}
      }

    return obj;
  }
};

#endif /* SOUNDCACHE_H_ */
