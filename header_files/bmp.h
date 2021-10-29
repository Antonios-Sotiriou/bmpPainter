#ifndef BMP_H
#define BMP_H

typedef struct {
    unsigned short Type;
    unsigned int Size;
    unsigned short Res1;
    unsigned short Res2;
    unsigned int OffSet;
} BMP_Header;
/* The structs in BMP_Info header which define the endpoints */
typedef struct {
    int RedX;
    int RedY;
    int RedZ;
} Red;
typedef struct {
    int BlueX;
    int BlueY;
    int BlueZ;
} Blue;
typedef struct {
    int GreenX;
    int GreenY;
    int GreenZ;
} Green;
typedef struct {
    Red red_coords;
    Green green_coords;
    Blue blue_coords;
} Color_Palete;
/* End of Endpoints structs */
typedef struct {
    unsigned int Size;
	signed int Width;
	signed int Height;
	unsigned short Planes;
	unsigned short BitsPerPixel;
	unsigned int compression;
	unsigned int SizeOfBitmap;
	long  HorzResolution;
	long VertResolution;
	unsigned int ColorsUsed;
	unsigned int ColorsImportant;
	unsigned int RedMask;
	unsigned int GreenMask;
	unsigned int BlueMask;
	unsigned int AlphaMask;
	unsigned int CSType;
    Color_Palete EndPoints;
	unsigned int GammaRed;
	unsigned int GammaGreen;
	unsigned int GammaBlue;
	unsigned int Intent;
	unsigned int ProfileData;
	unsigned int ProfileSize;
	unsigned int Reserved;
} BMP_Info ;
typedef struct {
    unsigned char RedPixel;
    unsigned char BluePixel;
    unsigned char GreenPixel;
} Pixels;

#endif /* BMP_H */

