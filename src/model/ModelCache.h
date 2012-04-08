/*
 * ModelCache.h
 *
 *  Created on: 11.01.2011
 *      Author: Администратор
 */

#ifndef MODELCACHE_H_
#define MODELCACHE_H_

#include "../DataManager.h"
#include "MD2ModelData.h"
#include "../Game.h"


class ModelCache :
  public DataManager<AbstractModelData, ModelCache>
{
  friend class DataManager<AbstractModelData, ModelCache>;

public:
  // Load and register a object.  If the object has already been
  // loaded previously, return it instead of loading it.
  AbstractModelData *load (const string &filename)
  {
    // Look for the object in the registry
	  AbstractModelData *obj = request (filename);

    // If not found, load the object
    if (obj == NULL)
    {
    	try {

    		if (MD2ModelData::isMD2File(filename)) {
        		obj = new MD2ModelData(Game::instance->osAdapter->getResourcePath() + "models/" + filename);
    		}

    		if (obj == NULL) {
    			throw std::runtime_error ("Undefined model type");
    		}

        	registerObject (filename, obj);
		}catch (std::exception& e) {
			Log::warning("Couldn't create AbstractModelData from %s Reason: %s", filename.c_str(), e.what());
		}
    }

    return obj;
  }
};



#endif /* MODELCACHE_H_ */
