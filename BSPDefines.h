/*
	This file contains data definitions of stuff inside a GoldSrc BSP file (v30).		
	They are used by BSPLoader class to parse things like brushes, entities, textures from a BSP file.
	Based on HL BSP v30 format : http://hlbsp.sourceforge.net/index.php?content=bspdef
	[TODO] Source BSP format : http://bagthorpe.org/bob/cofrdrbob/bspformat.html
*/

#pragma once
#include <stdint.h>

// Header stuff
#define LUMP_ENTITIES      0
#define LUMP_PLANES        1
#define LUMP_TEXTURES      2
#define LUMP_VERTICES      3
#define LUMP_VISIBILITY    4
#define LUMP_NODES         5
#define LUMP_TEXINFO       6
#define LUMP_FACES         7
#define LUMP_LIGHTING      8
#define LUMP_CLIPNODES     9
#define LUMP_LEAVES       10
#define LUMP_MARKSURFACES 11
#define LUMP_EDGES        12
#define LUMP_SURFEDGES    13
#define LUMP_MODELS       14
#define HEADER_LUMPS      15

// Half-life BSP Compiler constants
#define MAX_MAP_HULLS        4
#define MAX_MAP_MODELS       400
#define MAX_MAP_BRUSHES      4096
#define MAX_MAP_ENTITIES     1024
#define MAX_MAP_ENTSTRING    (128*1024)
#define MAX_MAP_PLANES       32767
#define MAX_MAP_NODES        32767
#define MAX_MAP_CLIPNODES    32767
#define MAX_MAP_LEAFS        8192
#define MAX_MAP_VERTS        65535
#define MAX_MAP_FACES        65535
#define MAX_MAP_MARKSURFACES 65535
#define MAX_MAP_TEXINFO      8192
#define MAX_MAP_EDGES        256000
#define MAX_MAP_SURFEDGES    512000
#define MAX_MAP_TEXTURES     512
#define MAX_MAP_MIPTEX       0x200000
#define MAX_MAP_LIGHTING     0x200000
#define MAX_MAP_VISIBILITY   0x200000
#define MAX_MAP_PORTALS      65536

// Lump
struct BSPLUMP
{
	int32_t nOffset; // File offset to data
	int32_t nLength; // Length of data
};

// Header
struct BSPHEADER
{
	int32_t nVersion;           // Must be 30 for a valid HL BSP file
	BSPLUMP lump[HEADER_LUMPS]; // Stores the directory of lumps
};

// Node
struct BSPNODE
{
	uint32_t	iPlane;				// Index into Planes lump
	int16_t		iChildren[2];       // If > 0, then indices into Nodes // otherwise bitwise inverse indices into Leafs
	int16_t		nMins[3], nMaxs[3]; // Defines bounding box
	uint16_t	firstFace, nFaces;	// Index and count into Faces
};

// Vector of 3 floats typical for position, normal etc.
struct VECTOR3D
{
	float x, y, z;

	VECTOR3D(float x0 = 0, float y0 = 0, float z0 = 0) {
		x = x0; y = y0; z = z0;
	}


};

#define PLANE_X		0			// Plane is perpendicular to given axis
#define PLANE_Y		1
#define PLANE_Z		2
#define PLANE_ANYX	3			// Non-axial plane is snapped to the nearest
#define PLANE_ANYY	4
#define PLANE_ANYZ	5

// Plane
struct BSPPLANE
{
	VECTOR3D	vNormal;	// The planes normal vector
	float		fDist;      // Plane equation is: vNormal * X = fDist
	int32_t		nType;		// Plane type, see #defines
};

// Edge
struct BSPEDGE
{
	uint16_t	iVertex[2];	// Indices into vertex array
};

// SurfEdge
typedef int32_t BSPSURFEDGE;

// Texture header
struct BSPTEXTUREHEADER
{
	uint32_t nMipTextures; // Number of BSPMIPTEX structures
};

// Texture ID
typedef int32_t BSPMIPTEXOFFSET;

#define MAXTEXTURENAME	16
#define MIPLEVELS		4

// Texture
struct BSPMIPTEX
{
	char		szName[MAXTEXTURENAME];  // Name of texture
	uint32_t	nWidth, nHeight;		// Extends of the texture
	uint32_t	nOffsets[MIPLEVELS];	// Offsets to texture mipmaps BSPMIPTEX;
};

// TextureInfo
struct BSPTEXTUREINFO
{
	VECTOR3D	vS;
	float		fSShift;    // Texture shift in s direction
	VECTOR3D	vT;
	float		fTShift;    // Texture shift in t direction
	uint32_t	iMiptex;	// Index into textures array
	uint32_t	nFlags;		// Texture flags, seem to always be 0
};

// Face
struct BSPFACE
{
	uint16_t	iPlane;				// Plane the face is parallel to
	uint16_t	nPlaneSide;			// Set if different normals orientation
	uint32_t	iFirstEdge;			// Index of the first surfedge
	uint16_t	nEdges;				// Number of consecutive surfedges
	uint16_t	iTextureInfo;		// Index of the texture info structure
	uint8_t		nStyles[4];			// Specify lighting styles
	uint32_t	nLightmapOffset;	// Offsets into the raw lightmap data
}; 

// Model
// Note: The first model is worldspawn also called as "bmodel" is the visible world excluding any entities
// All other models are referenced by entities
// https://developer.valvesoftware.com/wiki/Brush_entity
#define MAX_MAP_HULLS 4

struct BSPMODEL {
	float nMins[3], nMaxs[3];          // Defines bounding box
	VECTOR3D vOrigin;                  // Coordinates to move the // coordinate system
	int32_t iHeadnodes[MAX_MAP_HULLS]; // Index into nodes array
	int32_t nVisLeafs;                 // ???
	int32_t iFirstFace, nFaces;        // Index and count into faces

	void printInfo() {
		printf("** BSPMODEL **\n");
		printf("Bounding box : (%f,%f,%f) -> (%f,%f,%f)\n",
			nMins[0], nMins[1], nMins[2],
			nMaxs[0], nMaxs[1], nMaxs[2]);
		printf("origin : (%f,%f,%f)\n", vOrigin.x, vOrigin.y, vOrigin.z);
		printf("faces : %d\n", nFaces);
	}
};

struct BSPLEAF
{
	int32_t nContents;                         // Contents enumeration
	int32_t nVisOffset;                        // Offset into the visibility lump
	int16_t nMins[3], nMaxs[3];                // Defines bounding box
	uint16_t iFirstMarkSurface, nMarkSurfaces; // Index and count into marksurfaces array
	uint8_t nAmbientLevels[4];                 // Ambient sound levels
};