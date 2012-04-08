#ifndef IMAGETGA_H_
#define IMAGETGA_H_

#include "./platform/opengl.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include "PVRImage.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//
// class ImageBuffer - An image file loader class.  Load a whole file
// into a memory buffer.
//
/////////////////////////////////////////////////////////////////////////////

class ImageBuffer
{
public:
  // Constructors/destructor
  ImageBuffer (const string &filename);

  ImageBuffer (const ImageBuffer &that)
    : _filename (that._filename), _data (NULL), _length (that._length)
  {
    _data = new GLubyte[_length];
    memcpy (_data, that._data, _length);
  }

  ~ImageBuffer ();

private:
  // Disable default constructor
  ImageBuffer ();

public:
  // Accessors
  const string &filename () const { return _filename; }
  const GLubyte *data () const { return _data; }
  size_t length () const { return _length; }

  ImageBuffer &operator= (const ImageBuffer &rhs)
  {
    this->~ImageBuffer ();
    new (this) ImageBuffer (rhs);
    return *this;
  }

private:
  // Member variables
  string _filename;

  GLubyte *_data;
  size_t _length;
};


/////////////////////////////////////////////////////////////////////////////
//
// class Image - A generic image loader class for creating OpenGL
// textures from.  All other specific image loader are derived from it.
//
/////////////////////////////////////////////////////////////////////////////

class Image
{
protected:
  // Default constructor
  Image ()
    : _width (0), _height (0), _numMipmaps (0),
      _format (0), _components (0), _pixels (NULL),
      _standardCoordSystem (true) { }

  PVRImage* pvrImage;
private:
  // Disable copy constructor.
  Image (const Image &img);
  void init (const string &name, GLsizei w, GLsizei h, GLint numMipMaps,
			 GLenum format, GLint components, const GLubyte *pixels,
			 bool stdCoordSystem);
public:
  // Constructors/destructor
  Image (const string &name, GLsizei w, GLsizei h, GLint numMipMaps,
	 GLenum format, GLint components, const GLubyte *pixels,
	 bool stdCoordSystem);

  Image(const string &name);

  virtual ~Image();

public:
  bool isPowerOfTwo () const;

  // Accessors
  GLsizei width () const { return _width; }
  GLsizei height () const { return _height; }
  GLint numMipmaps () const { return _numMipmaps; }
  // For PVR ONLY at this time
  unsigned char* getMipmapData(int index) const;
  bool isCompressed() const;
  int getMipmapSize(int index) const;
  GLenum format () const { return _format; }
  GLint components () const { return _components; }
  const GLubyte *pixels () const { return _pixels; }
  const string &name () const { return _name; }
  bool stdCoordSystem () const { return _standardCoordSystem; }

protected:
  // Member variables
  GLsizei _width;
  GLsizei _height;
  GLint _numMipmaps;
  bool _isCompressed;

  // OpenGL texture format and internal
  // format (components)
  GLenum _format;
  GLint _components;

  // Image data
  GLubyte *_pixels;

  string _name;

  // Is the picture in standard OpenGL 2D coordinate
  // system? (starts lower-left corner)
  bool _standardCoordSystem;
};


/////////////////////////////////////////////////////////////////////////////
//
// class glImageTGA - A TrueVision TARGA (TGA) image loader class.
// Support 24-32 bits BGR files; 16 bits RGB; 8 bits indexed (BGR
// palette); 8 and 16 bits grayscale; all compressed and uncompressed.
// Compressed TGA images use RLE algorithm.
//
/////////////////////////////////////////////////////////////////////////////

class ImageTGA : public Image
{
public:
  // Constructor
  ImageTGA (const string &filename);
  static int writeTGAFile(char *filename, short int width, short int height, unsigned char* imageData); // save tagra file
private:
  // Internal functions
  void getTextureInfo ();

  void readTGA8bits (const GLubyte *data, const GLubyte *colormap);
  void readTGA16bits (const GLubyte *data);
  void readTGA24bits (const GLubyte *data);
  void readTGA32bits (const GLubyte *data);
  void readTGAgray8bits (const GLubyte *data);
  void readTGAgray16bits (const GLubyte *data);

  void readTGA8bitsRLE (const GLubyte *data, const GLubyte *colormap);
  void readTGA16bitsRLE (const GLubyte *data);
  void readTGA24bitsRLE (const GLubyte *data);
  void readTGA32bitsRLE (const GLubyte *data);
  void readTGAgray8bitsRLE (const GLubyte *data);
  void readTGAgray16bitsRLE (const GLubyte *data);

private:
  // Member variables
#pragma pack(push, 1)
  // tga header
  struct TGA_Header
  {
    GLubyte id_lenght;        // size of image id
    GLubyte colormap_type;    // 1 is has a colormap
    GLubyte image_type;       // compression type

    short cm_first_entry;     // colormap origin
    short cm_length;          // colormap length
    GLubyte cm_size;          // colormap size

    short x_origin;           // bottom left x coord origin
    short y_origin;           // bottom left y coord origin

    short width;              // picture width (in pixels)
    short height;             // picture height (in pixels)

    GLubyte pixel_depth;      // bits per pixel: 8, 16, 24 or 32
    GLubyte image_descriptor; // 24 bits = 0x00; 32 bits = 0x80

  };
#pragma pack(pop)

  const TGA_Header *_header;

  // NOTE:
  // 16 bits images are stored in RGB
  // 8-24-32 images are stored in BGR(A)

  // RGBA/BGRA component table access -- usefull for
  // switching from bgra to rgba at load time.
  static int rgbaTable[4]; // bgra to rgba: 2, 1, 0, 3
  static int bgraTable[4]; // bgra to bgra: 0, 1, 2, 3
};


#endif /*IMAGETGA_H_*/
