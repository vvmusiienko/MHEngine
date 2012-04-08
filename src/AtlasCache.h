/*
 * AtlasCache.h
 *
 *  Created on: 20.12.2010
 *      Author: Администратор
 */

#ifndef ATLASCACHE_H_
#define ATLASCACHE_H_

#include "DataManager.h"
#include "Atlas.h"
#include "Game.h"


class AtlasCache :
  public DataManager<Atlas, AtlasCache>
{
  friend class DataManager<Atlas, AtlasCache>;

public:

  Atlas* load (const string &filename) {
 	// Look for the object in the registry
    Atlas *obj = request (filename);

    // If not found, load the texture
    if (obj == NULL) {
    	try {
    		obj = new Atlas(Game::instance->osAdapter->getResourcePath() + "atlases/" + filename + ".atl");
		    registerObject (filename, obj);
    	}catch (std::exception& e) {
    		Log::warning("1Couldn't create atlas from %s Reason: %s", filename.c_str(), e.what());
    	}
    }
    return obj;
  }

};

#endif /* ATLASCACHE_H_ */
