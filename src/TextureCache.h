/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// TextureManager.h -- Copyright (c) 2006 David Henry
// last modification: feb. 25, 2006
//
// This code is licenced under the MIT license.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
// Definitions of a texture manager class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __TEXTURE2DMANAGER_H__
#define __TEXTURE2DMANAGER_H__

#include "DataManager.h"
#include "Texture.h"
#include "Game.h"
#include "AtlasTexture.h"

/////////////////////////////////////////////////////////////////////////////
//
// class TextureCache -- a texture manager which can register/unregister
// Texture2D objects.  Destroy all registred textures at death.
//
// The texture manager is a singleton.
//
/////////////////////////////////////////////////////////////////////////////

class TextureCache :
  public DataManager<Texture, TextureCache>
{
  friend class DataManager<Texture, TextureCache>;

public:
  // Load and register a texture.  If the texture has already been
  // loaded previously, return it instead of loading it.
  Texture *load (const string &filename)
  {
    // Look for the texture in the registry
    Texture *tex = request (filename);

    // If not found, load the texture
    if (tex == NULL) {

    	if (AtlasTexture::isNameWithAtlas(filename)) {
    		return new AtlasTexture(filename);
    	} else {
            string texture_folder;
    		for (int i = Game::instance->getTextureQuality(); i >= 1; i--) {
                texture_folder = FormatString("textures_%i/", i);
                FILE* f = fopen((Game::instance->osAdapter->getResourcePath() + texture_folder + filename).c_str(), "rb");
                if (!f) {
                    fclose(f);
                    continue;
                } else {
                    fclose(f);
                    break;
                }
            }
            
            tex = new Texture2D(Game::instance->osAdapter->getResourcePath() + texture_folder + filename);
    	}
	
		// If the texture creation failed, delete the
		// unusable object and return NULL
		if (tex->fail ())
		{
			//Log::warning("Can't load texture! (%s)", filename.c_str());
			delete tex;
		    tex = NULL;
		}
		else
		{
		    // The texture has been successfully loaded,
		    // register it.
		    registerObject (filename, tex);
		    //Log::info("Texture %s loaded.", (filename + ".tga").c_str());
		}
      }

    return tex;
  }
};


#endif // __TEXTUREMANAGER_H__
