/*
  this code consists in many parts on code from the DDS GIMP plugin
  while it's also a rewrite of the texcompress utility from mother.
  the new codebase features a gui-less texcompress utility which
  makes script based batch jobs on - console only - linux possible.
  see README-texcompress_nogui.txt for more details.

  code made available by me according to the terms of the GPL.

  Copyright (C) 2007 Joachim Schiele <js@dune2.de>


  --

  DDS GIMP plugin

  Copyright (C) 2004 Shawn Kirst <skirst@fuse.net>,
   with parts (C) 2003 Arne Reuter <homepage@arnereuter.de> where specified.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
*/

#define IS_POT(x)      (!((x) & ((x) - 1)))
#include <stdlib.h>

/*#include <GL/gl.h>
#include <GL/glew.h>
#include <GL/glu.h>*/


#include <string.h>
#include <math.h>

void *hdxtn = NULL;
#define DXTN_DLL "dxtn.dll"
#define RTLD_LAZY   1
#define RTLD_GLOBAL 1

static void (*compress_dxtn)(int, int, int, const unsigned char*, int, unsigned char *) = NULL;

void compressed_rgba_s3tc_dxt1_ext_software(int mipmaps,unsigned char *src, int w, int h, unsigned char *dst);
int dxt_compress(unsigned char *dst, unsigned char *src, int format,
                 unsigned int width, unsigned int height, int bpp,
                 int mipmaps);
static void swap_rb(unsigned char *pixels, unsigned int n, int bpp);
int generate_mipmaps_software(unsigned char *dst, unsigned char *src,
                              unsigned int width, unsigned int height,
                              int bpp, int indexed, int mipmaps);
static void scale_image_cubic(unsigned char *dst, int dw, int dh,
                              unsigned char *src, int sw, int sh,
                              int bpp);
float cubic_interpolate(float a, float b, float c, float d, float x);
unsigned int get_mipmapped_size(int width, int height, int bpp,
                                int level, int num, int format);
int get_num_mipmaps(int width, int height);
bool ccompress_one(const char * in_filename);
bool compress_one(const char * in_filename);
typedef double (*importFunction)(double, double);



#ifndef DDS_H
#define DDS_H

typedef enum
{
   DDS_COMPRESS_NONE = 0,
   DDS_COMPRESS_BC1,   /* DXT1  */
   DDS_COMPRESS_BC2,   /* DXT3  */
   DDS_COMPRESS_BC3,   /* DXT5  */
   DDS_COMPRESS_BC3N,  /* DXT5n */
   DDS_COMPRESS_BC4,   /* ATI1  */
   DDS_COMPRESS_BC5,   /* ATI2  */
   DDS_COMPRESS_MAX
} DDS_COMPRESSION_TYPE;

typedef enum
{
   DDS_SAVE_SELECTED_LAYER = 0,
   DDS_SAVE_CUBEMAP,
   DDS_SAVE_VOLUMEMAP,
   DDS_SAVE_MAX
} DDS_SAVE_TYPE;

typedef enum
{
   DDS_FORMAT_DEFAULT = 0,
   DDS_FORMAT_RGB8,
   DDS_FORMAT_RGBA8,
   DDS_FORMAT_BGR8,
   DDS_FORMAT_ABGR8,
   DDS_FORMAT_R5G6B5,
   DDS_FORMAT_RGBA4,
   DDS_FORMAT_RGB5A1,
   DDS_FORMAT_RGB10A2,
   DDS_FORMAT_R3G3B2,
   DDS_FORMAT_L8,
   DDS_FORMAT_L8A8,
   DDS_FORMAT_MAX
} DDS_FORMAT_TYPE;

#define DDS_HEADERSIZE             128

#define DDSD_CAPS                  0x00000001
#define DDSD_HEIGHT                0x00000002
#define DDSD_WIDTH                 0x00000004
#define DDSD_PITCH                 0x00000008
#define DDSD_PIXELFORMAT           0x00001000
#define DDSD_MIPMAPCOUNT           0x00020000
#define DDSD_LINEARSIZE            0x00080000
#define DDSD_DEPTH                 0x00800000

#define DDPF_ALPHAPIXELS           0x00000001
#define DDPF_FOURCC                0x00000004
#define DDPF_PALETTEINDEXED8       0x00000020
#define DDPF_RGB                   0x00000040
#define DDPF_LUMINANCE             0x00020000

#define DDSCAPS_COMPLEX            0x00000008
#define DDSCAPS_TEXTURE            0x00001000
#define DDSCAPS_MIPMAP             0x00400000

#define DDSCAPS2_CUBEMAP           0x00000200
#define DDSCAPS2_CUBEMAP_POSITIVEX 0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX 0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY 0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY 0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ 0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ 0x00008000
#define DDSCAPS2_VOLUME            0x00200000

typedef struct __attribute__((packed))
{
   unsigned int size;
   unsigned int flags;
   char fourcc[4];
   unsigned int bpp;
   unsigned int rmask;
   unsigned int gmask;
   unsigned int bmask;
   unsigned int amask;
} dds_pixel_format_t;

typedef struct __attribute__((packed))
{
   unsigned int caps1;
   unsigned int caps2;
   unsigned int reserved[2];
} dds_caps_t;

typedef struct __attribute__((packed))
{
   char magic[4];
   unsigned int size;
   unsigned int flags;
   unsigned int height;
   unsigned int width;
   unsigned int pitch_or_linsize;
   unsigned int depth;
   unsigned int num_mipmaps;
   unsigned char reserved[4 * 11];
   dds_pixel_format_t pixelfmt;
   dds_caps_t caps;
   unsigned int reserved2;
} dds_header_t;

#endif
