#ifndef __SMT_H
#define __SMT_H

#include <OpenImageIO/imagebuf.h>

OIIO_NAMESPACE_USING
using namespace std;

#define DXT1 1

struct SMTHeader
{
    char magic[16] = "spring tilefile";   // "spring tilefile\0"
    int version = 1;      // must be 1 for now
    int nTiles = 0;       // total number of tiles in this file
    int tileRes = 32;      // x and y dimension of tiles, must remain 32 for now.
    int tileType = DXT1;     // must be 1=dxt1 for now
};

class SMT {
    bool init = false;
    // Tiles
    unsigned int nTiles    = 0,
                 tileType  = 1,
                 tileRes   = 32,
                 tileSize  = 680;

    // Input Files
    string fileName    = "output.smt";
    void calcTileSize();
    bool load();
    
public:
    bool  verbose = false, quiet = false, slow_dxt1 = false;

    SMT( ){ };
    SMT( string f, bool v, bool q )
        : fileName( f ), verbose( v ), quiet( q ) { init = !load(); };

    static SMT *create( string fileName,
            bool overwrite = false,
            bool verbose = false,
            bool quiet = false );

    static SMT *open( string fileName,
            bool verbose = false,
            bool quiet = false );

    bool initialised( ){ return init; };
    bool reset( );

    void setTileRes( unsigned int r );
    void setType   ( unsigned int t ); // 1=DXT1

    int getTileType( ){ return tileType; };
    int getTileRes ( ){ return tileRes;  };
    int getTileSize( ){ return tileSize; };
    int getNTiles  ( ){ return nTiles;   };
    string getFileName( ){ return fileName; };

    ImageBuf *getTile( int tile );
    bool append( ImageBuf * );
};

#endif //ndef __SMT_H
