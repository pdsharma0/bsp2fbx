#include "BSP2FBX.h"
#include "fbxsdk/fileio/fbxiosettings.h"
#include "fbxsdk/fileio/fbxexporter.h"
#include "fbxsdk/scene/geometry/fbxmesh.h"
#include "fbxsdk/scene/geometry/fbxlayer.h"
#include <stdio.h>

//---------------------------------------------------------------------
BSP2FBX::BSP2FBX()
{
	m_bspLoader = nullptr;
	m_fbxManager = FbxManager::Create();
	m_fbxScene = nullptr;
}

//---------------------------------------------------------------------
BSP2FBX::~BSP2FBX()
{
	UnloadBSPFile();
}

//---------------------------------------------------------------------
void BSP2FBX::LoadBSPFile(const char * bspFile)
{
	m_bspFileName = bspFile;
	m_bspLoader = new BSPLoader(m_bspFileName.c_str());
	m_bspLoader->ReadVertices();
	m_bspLoader->ReadPlanes();
	m_bspLoader->ReadEdges();
	m_bspLoader->ReadSurfEdges();
	m_bspLoader->ReadTexInfo();
	m_bspLoader->ReadTextures();
	m_bspLoader->ReadFaces();
	m_bspLoader->ReadModels();
	m_bspLoader->ReadEntities();
	//m_bspLoader->ReadLeaves();
}

// Converts a Right Handed Coordinate system to Left Handed and vice-versa
// Source engine uses a right-handed coordinate system where 
// X is forward, Y is Left and Z is up
// Where as Unreal engine uses a left-handed coordinate system instead
// X is forward, Y is Right and Z is up
VECTOR3D SwitchHandedness(VECTOR3D v) {
	return VECTOR3D(v.x, v.z, v.y);
}

//---------------------------------------------------------------------
FbxMesh* BSP2FBX::CreateFbxMesh(BSPMODEL* model) {
		
	unsigned nPolygons = model->nFaces;					// FbxMesh initialization data structures
	vector<unsigned> nPolygonCPs;						// Every polygon is composed of an array of control points indices
	vector<VECTOR3D> cpPositions;						// Control point positions
	vector<VECTOR3D> cpNormals;							// Control point normals
	vector<VECTOR3D> cpTangents;						// Control point tangents

	// Go through all the faces of BSPMODEL
	for (unsigned faceId = model->iFirstFace; faceId < (model->iFirstFace + model->nFaces); faceId++) {

		//printf("----- face : %u -----\n", faceId);

		// A face is a n-sided-polygon defined by a series of "SurfEdges"
		// It's basically a closed loop of edges like this:
		// f : (e0, e1, e2, ..., eN-1) for N edges
		// f : (v0 -> v1, v1 -> v2, v2 -> v3, ... , vN-1 -> v0) for N vertices
		// so number of vertices aka Control Points is same as number of surfedges 
		BSPFACE* face = &(m_bspLoader->m_Faces[faceId]);

		// Get face's material
		BSPTEXTUREINFO texInfo = m_bspLoader->m_TextureInfos[face->iTextureInfo];
		BSPMIPTEX tex = m_bspLoader->m_Textures[texInfo.iMiptex];

		//	skyboxes are not to be added to our visible mesh
		if (!strcmp(tex.szName, "sky")) {
			// We're a polygon less now
			nPolygons--;
			continue;
		}

		// Number of control points is equal to the number of edges for a closed planar surface
		nPolygonCPs.push_back(face->nEdges);

		// Store each face's normal
		BSPPLANE* plane = &(m_bspLoader->m_Planes[face->iPlane]);
		VECTOR3D normal = plane->vNormal;
		// Reverse normal if specified
		if (face->nPlaneSide) {
			normal.x *= -1.0f;
			normal.y *= -1.0f;
			normal.z *= -1.0f;
		}
		//polygonNormals.push_back(normal);
		/*printf("Poly: %u Vertices: %u LightingStyle: %u,%u,%u,%u Texture: %s\n", 
			faceId, 
			face->nEdges, 
			face->nStyles[0], face->nStyles[1], face->nStyles[2], face->nStyles[3],
			tex.szName);*/

		// For each surfedge
		for (unsigned surfedgeId = face->iFirstEdge; surfedgeId < (face->iFirstEdge + face->nEdges); surfedgeId++) {

			// Get corresponding edgeId
			int edgeId = m_bspLoader->m_SurfEdges[surfedgeId];
			unsigned edgeId_abs = abs(edgeId);

			// Get vertex IDs of edge
			unsigned v0Id = m_bspLoader->m_Edges[edgeId_abs].iVertex[0];
			unsigned v1Id = m_bspLoader->m_Edges[edgeId_abs].iVertex[1];

			// Swap v0Id and v1Id if we have a negative edgeId
			if (edgeId < 0)
				swap(v0Id, v1Id);

			// Get vertices
			VECTOR3D v0 = m_bspLoader->m_Vertices[v0Id];
			VECTOR3D v1 = m_bspLoader->m_Vertices[v1Id];

			/*printf("SurfEdge: %u Edge: %d Edge_abs: %u, v0:%f,%f,%f v1:%f,%f,%f \n", surfedgeId, edgeId, edgeId_abs,
				v0.x, v0.y, v0.z, v1.x, v1.y, v1.z);*/

			// Every edge naturally defines a tangent as well
			VECTOR3D tangent;
			tangent.x = v0.x - v1.x;
			tangent.y = v0.y - v1.y;
			tangent.z = v0.z - v1.z;

			float tangent_length = sqrt(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z);
			tangent.x = tangent.x / tangent_length;
			tangent.y = tangent.y / tangent_length;
			tangent.z = tangent.z / tangent_length;

			/*printf("SurfEdge: %u Edge: %d Edge_abs: %u, v0:%f,%f,%f v1:%f,%f,%f Tangent: %f,%f,%f Normal: %f,%f,%f\n", 
				surfedgeId, edgeId, edgeId_abs,
				v0.x, v0.y, v0.z, 
				v1.x, v1.y, v1.z,
				tangent.x, tangent.y, tangent.z,
				normal.x, normal.y, normal.z);*/

			// Compute texture coordinates for this vertex
			// http://www.flipcode.com/archives/Quake_2_BSP_File_Format.shtml
			float u = (texInfo.vS.x * v0.x + texInfo.vS.y * v0.y + texInfo.vS.z * v0.z) + texInfo.fSShift;
			float v = (texInfo.vT.x * v0.x + texInfo.vT.y * v0.y + texInfo.vT.z * v0.z) + texInfo.fTShift;

			/*printf("Vertex: %f,%f,%f S:%f,%f,%f T: %f,%f,%f Offset: %f,%f UV: %f,%f\n", 
				v0.x, v0.y, v0.z, 
				texInfo.vS.x, texInfo.vS.y, texInfo.vS.z,
				texInfo.vT.x, texInfo.vT.y, texInfo.vT.z,
				texInfo.fSShift, texInfo.fTShift,
				u, v);*/

			// Add v0 to the list of control points
			cpPositions.push_back(SwitchHandedness(v0));
			cpNormals.push_back(SwitchHandedness(normal));
			cpTangents.push_back(SwitchHandedness(tangent));
		}
	}

	//printf("Creating FbxMesh\n");

	// Create a new FbxMesh
	FbxMesh* mesh = FbxMesh::Create(m_fbxScene, "mesh");

	// Create an array of global control points 
	// A polygon would just index into this array
	mesh->InitControlPoints(cpPositions.size());
	FbxVector4* cps = mesh->GetControlPoints();
	for (unsigned i = 0; i < cpPositions.size(); i++) {
		cps[i] = FbxVector4(cpPositions[i].x, cpPositions[i].y, cpPositions[i].z);
	}

	//printf("control points: %u\n", cpPositions.size());
	//printf("polygons: %u\n", nPolygons);

	mesh->ReservePolygonCount(nPolygons);
	// Global control point index
	unsigned cpId = 0;

	// Add a polygon to the mesh
	for (unsigned pId = 0; pId < nPolygons; pId++) {
		unsigned nCPs = nPolygonCPs[pId];
		mesh->BeginPolygon();
		for (unsigned i = 0; i < nCPs; i++) {
			mesh->AddPolygon(cpId++);
		}
		mesh->EndPolygon();
	}

	// Create a layer for normals
	FbxLayer* nLayer = mesh->GetLayer(0);
	if (nLayer == NULL) {
		mesh->CreateLayer();
		nLayer = mesh->GetLayer(0);
	}

	// Create a layer for tangents
	FbxLayer* tLayer = mesh->GetLayer(1);
	if (tLayer == NULL) {
		mesh->CreateLayer();
		tLayer = mesh->GetLayer(1);
	}

	// Create a normal layer.
	FbxLayerElementNormal* leNormal = FbxLayerElementNormal::Create(mesh, "normalLayer");
	FbxLayerElementTangent* leTangent = FbxLayerElementTangent::Create(mesh, "tangentLayer");

	// Set its mapping mode to map each normal vector to each polygon
	leNormal->SetMappingMode(FbxLayerElement::eByControlPoint);
	leTangent->SetMappingMode(FbxLayerElement::eByControlPoint);

	// Set the reference mode of so that the n'th element of the normal array maps to the n'th
	// element of the polygon array.
	leNormal->SetReferenceMode(FbxLayerElement::eDirect);
	leTangent->SetReferenceMode(FbxLayerElement::eDirect);

	// Add per control point normal
	for (auto cpn : cpNormals) {
		// Reverse all normals
		FbxVector4 n(cpn.x, cpn.y, cpn.z);
		leNormal->GetDirectArray().Add(n);
	}

	// Add per control point tangent
	for (auto cpt : cpTangents) {
		// Reverse all normals
		FbxVector4 t(cpt.x, cpt.y, cpt.z);
		leTangent->GetDirectArray().Add(t);
	}

	// Assign normals to layer
	nLayer->SetNormals(leNormal);
	tLayer->SetTangents(leTangent);

	return mesh;
}

//---------------------------------------------------------------------
void BSP2FBX::GenerateFBX()
{
	// Create scene object
	m_fbxScene = FbxScene::Create(m_fbxManager, m_bspFileName.c_str());
	FbxNode* root = m_fbxScene->GetRootNode();

	// Create a visible geometry node containing all visible geometry in BSP
	FbxNode* node_visible_geometry = FbxNode::Create(m_fbxScene, "visible_geometry");
	printf("Creating FBX Node: visible_geometry\n");
	// Applying a mirror transform in X direction for all the nodes down this hierarchy
	node_visible_geometry->LclScaling.Set(FbxDouble3(-1, 1, 1));
	root->AddChild(node_visible_geometry);

	// ----- Visible Geometries in BSP  -----

	// --- worldspawn ---
	BSPMODEL* worldspawn_model = &(m_bspLoader->m_Models[0]);
	printf("Creating FBX Node: worldspawn\n");
	FbxMesh* worldspawn_mesh = CreateFbxMesh(worldspawn_model);
	FbxNode* node_worldspawn = FbxNode::Create(m_fbxScene, "worldspawn");
	node_worldspawn->SetNodeAttribute(worldspawn_mesh);
	node_visible_geometry->AddChild(node_worldspawn);

	// --- func_walls ---
	FbxNode* node_func_walls = FbxNode::Create(m_fbxScene, "func_walls");
	printf("Creating FBX Node: func_walls\n");
	node_visible_geometry->AddChild(node_func_walls);
	int index = 0;
	for (auto i : m_bspLoader->m_funcwalls) {
		// Create a sub-node per func_wall and add it to func_walls node
		string node_name = string("func_wall") + to_string(index++);
		FbxNode* node_func_wall = FbxNode::Create(m_fbxScene, node_name.c_str());
		printf("Creating FBX Node: %s\n", node_name.c_str());
		FbxMesh* mesh = CreateFbxMesh(i.model);
		node_func_wall->AddNodeAttribute(mesh);
		node_func_walls->AddChild(node_func_wall);
	}

	// --- func_breakables ---
	FbxNode* node_func_breakables = FbxNode::Create(m_fbxScene, "func_breakables");
	printf("Creating FBX Node: func_breakables\n");
	node_visible_geometry->AddChild(node_func_breakables);
	index = 0;
	for (auto i : m_bspLoader->m_funcbreakables) {
		// Create a sub-node per func_wall and add it to func_walls node
		string node_name = string("func_breakable") + to_string(index++);
		FbxNode* node_func_breakable = FbxNode::Create(m_fbxScene, node_name.c_str());
		printf("Creating FBX Node: %s\n", node_name.c_str());
		FbxMesh* mesh = CreateFbxMesh(i.model);
		node_func_breakable->AddNodeAttribute(mesh);
		node_func_breakables->AddChild(node_func_breakable);
	}

	// ----- Lights -----
	// Get lights to lighten up the world!

	// ----- Textures -----
	// Fill some color into our black and white world!

	// ----- Collision -----
	// Need to define collision geometry before we load our player

	// ----- Export to a FBX file -----
	FbxIOSettings* ios = FbxIOSettings::Create(m_fbxManager, IOSROOT);
	m_fbxManager->SetIOSettings(ios);
	FbxExporter* lExporter = FbxExporter::Create(m_fbxManager, "");
	string fbxFileName = m_bspFileName.substr(0, m_bspFileName.size() - 4) + string(".fbx");
	// Get the appropriate file format. Binary : FBX binary (*.fbx), ASCII : FBX ascii (*.fbx)
	int lFormat = m_fbxManager->GetIOPluginRegistry()->FindWriterIDByDescription("FBX binary (*.fbx)");
	bool lResult = lExporter->Initialize(fbxFileName.c_str(), lFormat, m_fbxManager->GetIOSettings());
	if (!lResult) {
		printf("Call to FbxExporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
	}
	lResult = lExporter->Export(m_fbxScene);
	if (!lResult) {
		printf("Call to FbxExporter::Export() failed.\n");
	}
	lExporter->Destroy();
	printf("*** Exporting to : %s ***\n", fbxFileName.c_str());
}

//---------------------------------------------------------------------
void BSP2FBX::UnloadBSPFile()
{
	if (m_bspLoader)
		delete[] m_bspLoader;
	m_bspLoader = nullptr;
}

//---------------------------------------------------------------------
int main(int argc, char** argv) {
	if (argc < 2) {
		printf("ERROR: No BSP file was provided as an argument.\n");
		exit(1);
	}

	const char* bspFileName = argv[1];
	printf("Loading BSP file : %s\n", bspFileName);

	BSP2FBX bsp2fbx;
	bsp2fbx.LoadBSPFile(bspFileName);
	bsp2fbx.GenerateFBX();
	return 0;
}