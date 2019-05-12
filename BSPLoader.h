/*
	This file defines the BSPLoader class which loads a BSP file
	into suitable data-structures.
*/

#pragma once

// ----- Includes -----
#include <string>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "BSPDefines.h"
#include "BSPEntities.h"

using namespace std;

// ======================================================================
// BSPLoader defines a set of functions to read data from a BSP map file
// ======================================================================
class BSPLoader
{
public:
	// Constructor
	BSPLoader(const char* bspFileName);
	
	// Destructor
	~BSPLoader();

	// --------- Class interface ---------;

	// Reads the BSP node hierarchy and returns pointer to array in root
	void ReadNodes();

	// Read Vertices lump
	void ReadVertices();

	// Read Planes
	void ReadPlanes();

	// Read Edges 
	// Edge -> Vertices
	void ReadEdges();

	// Read SurfEdges
	// SurfEdge -> Edge
	void ReadSurfEdges();

	// Read textures
	void ReadTextures();

	// Read TexInfo lump
	// TexInfo -> texture
	void ReadTexInfo();

	// Read Faces
	// Face -> Plane, TexInfo, Edges
	void ReadFaces();
	
	// Create entity objects from their attribute mappings
	void ProcessEntity(map<string, string>& attributes);

	// Extract an entity attributes from its raw string
	void ProcessEntityStr(string& entityStr);

	// Read entities from BSP
	void ReadEntities();

	// Read Models from BSP
	void ReadModels();

	// Read Leaves from BSP
	void ReadLeaves();

	// --------- Class data ---------

	std::ifstream		m_FileStream;		// BSP file read handle	
	BSPHEADER			m_Header;			// Stores version and lump information

	unsigned			m_nVertices;		// Number of Vertices
	VECTOR3D*			m_Vertices;			// Array of Vertices

	unsigned			m_nPlanes;
	BSPPLANE*			m_Planes;			// Array of Planes

	unsigned			m_nEdges;
	BSPEDGE*			m_Edges;			// Array of Edges

	unsigned			m_nSurfEdges;		
	BSPSURFEDGE*		m_SurfEdges;		// Array of SurfEdges

	unsigned			m_nTextures;
	BSPMIPTEXOFFSET*	m_TextureOffsets;	// Array of Texture Offsets
	BSPMIPTEX*			m_Textures;			// Array of Textures

	unsigned			m_nTextureInfos;
	BSPTEXTUREINFO*		m_TextureInfos;		// Array of TextureInfos

	unsigned			m_nFaces;
	BSPFACE*			m_Faces;			// Array of Faces

	unsigned			m_nModels;
	BSPMODEL*           m_Models;			// Array of Models

	unsigned			m_nNodes;
	BSPNODE*			m_Nodes;			// Array of Nodes

	unsigned			m_nLeaves;
	BSPLEAF*			m_Leaves;			// Array of Leaves

	unsigned			m_nEntities;		// Total number of entities in BSP file
	char*				m_Entities;			// Entity string

	entity_worldspawn				m_worldspawn;		// A BSP has a single worldspawn entity
	vector<entity_funcwall>			m_funcwalls;		// List of func_wall entities
	vector<entity_funcbreakable>	m_funcbreakables;	// List of func_breakable entities
};