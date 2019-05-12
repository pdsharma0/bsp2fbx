#pragma once

/*
	The BSP2FBX class uses BSPLoader to read models from a BSP and 
	generates a corresponding FBX file using the Autodesk's FBX SDK.
	Each BSPMODEL (defined in BSPDefines.h) is converted to a FbxMesh object.
*/

#include "fbxsdk.h"
#include "BSPLoader.h"
#include <string>

using namespace std;

class BSP2FBX {
public:
	// Constructor
	BSP2FBX();

	// Destructor
	~BSP2FBX();

	// Read a BSP file contents using the BSPLoader
	void LoadBSPFile(const char* bspFile);

	// Create a FBxMesh using a BSPMODEL's geometry
	FbxMesh* CreateFbxMesh(BSPMODEL* model);

	// Dump the FBX (binary) file
	void GenerateFBX();

	// Unload a currently loaded BSP data if any
	void UnloadBSPFile();

private:

	string		m_bspFileName;
	BSPLoader*	m_bspLoader;

	// ---- FBX stuff -----
	FbxManager*			m_fbxManager;
	FbxScene*			m_fbxScene;
	vector<FbxMesh*>	m_fbxMeshes;
};