/*
 *  PVRImage.h
 *  PVRTextureLoader
 *
 *  Created by Volodymyr Musienko on 7/31/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef PVRImage_H_
#define PVRImage_H_

#include "./platform/opengl.h"
#include <vector>
#include <string>


class PVRImage {
private:
	int width;
	int height;
	GLenum internalFormat;
	bool hasAlpha;
	int numOfMipmaps;
	std::vector<unsigned char*> imageData;
	std::vector<unsigned int> imageDataSize;

public:
	int getWidth();
	int getHeight();
	GLenum getInternalFormat();
	bool isHasAlpha();
	int getNumOfMipmaps();
	unsigned char* getImageData(int mipmapIndex);
	unsigned int getImageDataSize(int mipmapIndex);

	PVRImage(std::string path);
	~PVRImage();
};

#endif
