/*
 *  PVRImage.cpp
 *  PVRTextureLoader
 *
 *  Created by Volodymyr Musienko on 7/31/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include "PVRImage.h"
#include <string>
#include <vector>
#include "Common.h"
#include <stdexcept>


using namespace std;

#define PVR_TEXTURE_FLAG_TYPE_MASK	0xff
static char gPVRTexIdentifier[] = "PVR!";

enum
{
	kPVRTextureFlagTypePVRTC_2 = 24,
	kPVRTextureFlagTypePVRTC_4
};

typedef struct _PVRTexHeader
{
	uint32_t headerLength;
	uint32_t height;
	uint32_t width;
	uint32_t numMipmaps;
	uint32_t flags;
	uint32_t dataLength;
	uint32_t bpp;
	uint32_t bitmaskRed;
	uint32_t bitmaskGreen;
	uint32_t bitmaskBlue;
	uint32_t bitmaskAlpha;
	uint32_t pvrTag;
	uint32_t numSurfs;
} PVRTexHeader;



PVRImage::PVRImage(string path) {
	PVRTexHeader header;
	unsigned int flags, pvrTag, formatFlags;
	unsigned int dataLength = 0, dataOffset = 0, dataSize = 0;
	unsigned int blockSize = 0, widthBlocks = 0, heightBlocks = 0;
	unsigned int bpp = 4;
	unsigned char* bytes = NULL;
	numOfMipmaps = 0;

	FILE* f = fopen(path.c_str(), "rb");
	if (!f) {
		throw runtime_error("Can't open file");
	}

	fread(&header, sizeof(PVRTexHeader), 1, f);

	pvrTag = header.pvrTag;

	if (gPVRTexIdentifier[0] != (char)((pvrTag >>  0) & 0xff) ||
		gPVRTexIdentifier[1] != (char)((pvrTag >>  8) & 0xff) ||
		gPVRTexIdentifier[2] != (char)((pvrTag >> 16) & 0xff) ||
		gPVRTexIdentifier[3] != (char)((pvrTag >> 24) & 0xff))
	{
		throw runtime_error("Incorrect PVR tag");
	}

	flags = header.flags;
	formatFlags = flags & PVR_TEXTURE_FLAG_TYPE_MASK;

	if (formatFlags == kPVRTextureFlagTypePVRTC_4 || formatFlags == kPVRTextureFlagTypePVRTC_2)
	{
		if (formatFlags == kPVRTextureFlagTypePVRTC_4)
			internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
		else if (formatFlags == kPVRTextureFlagTypePVRTC_2)
			internalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;

		width = header.width;
		height = header.height;

		if (header.bitmaskAlpha)
			hasAlpha = true;
		else
			hasAlpha = false;

		dataLength = header.dataLength;
		bytes = new unsigned char[dataLength];
		if (fread(bytes, dataLength, 1, f) != 1) {
			throw runtime_error("fread error");
		}
		int _width = width;
		int _height = width;

		// Calculate the data size for each texture level and respect the minimum number of blocks
		while (dataOffset < dataLength)
		{
			if (formatFlags == kPVRTextureFlagTypePVRTC_4)
			{
				blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
				widthBlocks = _width / 4;
				heightBlocks = _height / 4;
				bpp = 4;
			}
			else
			{
				blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
				widthBlocks = _width / 8;
				heightBlocks = _height / 4;
				bpp = 2;
			}
			
			// Clamp to minimum number of blocks
			if (widthBlocks < 2)
				widthBlocks = 2;
			if (heightBlocks < 2)
				heightBlocks = 2;
			
			dataSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
			
			imageData.push_back(bytes+dataOffset);
			imageDataSize.push_back(dataSize);
			//[_imageData addObject:[NSData dataWithBytes:bytes+dataOffset length:dataSize]];
			
			dataOffset += dataSize;
			
			_width = MAX(_width >> 1, 1);
			_height = MAX(_height >> 1, 1);
			
			numOfMipmaps ++;
		}
	} else {
		throw runtime_error("Incorrect PVR type");
	}
}


int PVRImage::getWidth() {
	return width;
}

int PVRImage::getHeight() {
	return height;
}

GLenum PVRImage::getInternalFormat() {
	return internalFormat;
}

bool PVRImage::isHasAlpha() {
	return hasAlpha;
}

int PVRImage::getNumOfMipmaps() {
	return numOfMipmaps;
}

unsigned int PVRImage::getImageDataSize(int mipmapIndex) {
	if (mipmapIndex >= 0 &&  mipmapIndex <= numOfMipmaps - 1) {
		return imageDataSize[mipmapIndex];
	}
	// TODO: throw error
	return 0;
}



unsigned char* PVRImage::getImageData(int mipmapIndex) {
	if (mipmapIndex >= 0 &&  mipmapIndex <= numOfMipmaps - 1) {
		return imageData[mipmapIndex];
	}
	// TODO: throw error
	return NULL;
}


PVRImage::~PVRImage() {
}

