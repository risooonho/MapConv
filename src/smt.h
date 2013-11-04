#ifndef __SMT_H
#define __SMT_H

#include <sys/time.h>
#include <deque>

#include <OpenImageIO/imageio.h>
#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagebufalgo.h>
#include "byteorder.h"
#include "nvtt_output_handler.h"
#include "smf.h"
#include "dxt1load.h"

#define DXT1 1

OIIO_NAMESPACE_USING
using namespace std;

// struct used when comparing tiles.
struct TileMip
{
	TileMip();
	char crc[8];
	char n;
};

/*
map texture tile file (.smt) layout is like this

TileFileHeader

Tiles
.
.
.
*/

struct SMTHeader
{
	SMTHeader();
	char magic[16];   //"spring tilefile\0"
	int version;      //must be 1 for now
	int nTiles;        //total number of tiles in this file
	int tileRes;     //must be 32 for now
	int comp;     //must be 1=dxt1 for now
};

#define READ_SMTHEADER(tfh,src)                \
do {                                                \
	unsigned int __tmpdw;                           \
	(src).Read(&(tfh).magic,sizeof((tfh).magic));   \
	(src).Read(&__tmpdw,sizeof(unsigned int));      \
	(tfh).version = (int)swabdword(__tmpdw);        \
	(src).Read(&__tmpdw,sizeof(unsigned int));      \
	(tfh).count = (int)swabdword(__tmpdw);       \
	(src).Read(&__tmpdw,sizeof(unsigned int));      \
	(tfh).tileRes = (int)swabdword(__tmpdw);       \
	(src).Read(&__tmpdw,sizeof(unsigned int));      \
	(tfh).comp = (int)swabdword(__tmpdw);\
} while (0)

//this is followed by the raw data for the tiles

class SMT {
	bool verbose, quiet, slowcomp;
	
	// Loading
	SMTHeader header;
	string loadFile;

	// Saving
	string outPrefix;
	int nTiles;
	int tileRes;
	int comp;

	// Construction
	int width, length;
	vector<string> sourceFiles;
	int stride;
	vector<TileMip> hash;
	int tileSize;

	// Reconstruction
	string tilemapFile;
	
public:
	SMT(bool v, bool q, int c);

	bool load(string fileName);
	bool save();
	bool save2();

	void setPrefix(string prefix);
	void setTileindex(string filename);
	void setRes(int r); // square resolution of the tiles.
	void setType(int comp);
	void setDim(int w, int l); // width and length of tileindex to construct in spring map units.

	void addImage(string filename);

//TODO	bool setSourceImages(vector<string> fileNames, int stride);
//TODO	bool setSourceImages(string fileName);


	bool decompileTiles();
	bool decompileCollate();
	bool decompileReconstruct();
//TODO
// Build SMT File (image, width, length)
// Build SMT File (Vector<Images>, stride, width, length)
//
// TODO
// Decompile from tile index maps that depend on more than one SMT
//
// TODO
// Replace Tiles(SMT, startx, starty, endx, endy, Replacement Image)
// Paste Decals(SMT, TileIndex, ListCSV, Decals)
};


#endif //ndef __SMT_H
