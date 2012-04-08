/*
 * FontCache.h
 *
 *  Created on: 21.06.2010
 *      Author: Администратор
 */

#ifndef FONTCACHE_H_
#define FONTCACHE_H_

#include "DataManager.h"
#include "./glfont2/glfont2.h"
#include "Game.h"
#include "glfont2/glfont2.h"

class FontCache :
  public DataManager<glfont::GLFont, FontCache>
{
  friend class DataManager<glfont::GLFont, FontCache>;

public:
  // Public interface
  static Game* game;

  // Load and register a texture.  If the texture has already been
  // loaded previously, return it instead of loading it.
  glfont::GLFont *load (const string &filename)
  {
    // Look for the object in the registry
	  glfont::GLFont *obj = request (filename);

    // If not found, load the object
    if (obj == NULL)
    {
    	obj = new glfont::GLFont();

		// If the object creation failed, delete the
		// unusable object and return NULL
		if (! obj->Create((Game::instance->osAdapter->getResourcePath() + "fonts/" + filename + ".glf").c_str()))
		{
			Log::warning("Can't load font! (%s)", filename.c_str());
			delete obj;
			obj = NULL;
		}
		else
		{
		    // The texture has been successfully loaded,
		    // register it.
		    registerObject (filename, obj);
		    //Log::info("Texture %s loaded.", (filename + ".tga").c_str());
		}
      }

    return obj;
  }
};

#endif /* FONTCACHE_H_ */
