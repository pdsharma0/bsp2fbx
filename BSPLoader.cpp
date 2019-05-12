#pragma once

#include <fstream>
#include <cassert>
#include "BSPLoader.h"

// -----------------------------------------------------------------
BSPLoader::BSPLoader(const char* fileName) {

	// Open BSP file
	m_FileStream.open(fileName, std::ios::binary);
	if (!m_FileStream.is_open()) {
		printf("Unable to load file: %s", fileName);
		exit(-1);
	}

	printf("BSP file: %s opened successfully!\n", fileName);

	// Get header version
	m_FileStream.read((char*)&m_Header, sizeof(BSPHEADER));
	printf("Version: %d\n", m_Header.nVersion);

	m_Vertices = nullptr;
	m_Planes = nullptr;
	m_Edges = nullptr;
	m_SurfEdges = nullptr;
	m_TextureOffsets = nullptr;
	m_TextureInfos = nullptr;
	m_Faces = nullptr;
	m_Nodes = nullptr;
}

// -----------------------------------------------------------------
BSPLoader::~BSPLoader() {

	m_FileStream.close();

	if (m_Vertices)
		delete[] m_Vertices;

	if (m_Planes)
		delete[] m_Planes;

	if (m_Edges)
		delete[] m_Edges;

	if (m_SurfEdges)
		delete[] m_SurfEdges;

	if (m_TextureOffsets)
		delete[] m_TextureOffsets;

	if (m_TextureInfos)
		delete[] m_TextureInfos;

	if (m_Faces)
		delete[] m_Faces;

	if (m_Nodes)
		delete[] m_Nodes;
}

// -----------------------------------------------------------------
void BSPLoader::ReadNodes() {

	// Get Node data offset and size from its LUMP
	int32_t nodeDataOffset = m_Header.lump[LUMP_NODES].nOffset;
	int32_t nodeDataSize = m_Header.lump[LUMP_NODES].nLength;

	unsigned nNodes = nodeDataSize / sizeof(BSPNODE);

	// Allocate memory for Node array
	m_Nodes = new BSPNODE[nNodes];

	// Read Node array from file
	m_FileStream.seekg(nodeDataOffset, std::ios::beg);
	m_FileStream.read((char*)m_Nodes, nodeDataSize);

	// Print out all the nodes for now doing a simple array traversal
	// Ideally we should be doing the hiearchial traversal 
	for (unsigned i = 0; i < nNodes; i++) {
		BSPNODE& node = m_Nodes[i];

		// Get Child0 data
		int32_t child0_index = node.iChildren[0];
		const char* child0_type = "node";
		if (child0_index < 0) {
			child0_type = "leaf";
			child0_index = ~child0_index;
		}
			
		// Get child1 data
		int32_t child1_index = node.iChildren[1];
		const char* child1_type = "node";
		if (child1_index < 0) {
			child1_type = "leaf";
			child1_index = ~child1_index;
		}

		float bboxLengthX = node.nMaxs[0] - node.nMins[0];
		if (bboxLengthX < 0) {
			printf("[WARNING] Xmin: %d, Xmax: %d\n", node.nMins[0], node.nMaxs[0]);
		}

		float bboxLengthY = node.nMaxs[1] - node.nMins[1];
		if (bboxLengthY < 0) {
			printf("[WARNING] Ymin: %d, Ymax: %d\n", node.nMins[1], node.nMaxs[1]);
		}

		float bboxLengthZ = node.nMaxs[2] - node.nMins[2];
		if (bboxLengthZ < 0) {
			printf("[WARNING] Zmin: %d, Zmax: %d\n", node.nMins[2], node.nMaxs[2]);
		}

		float bboxVolume = bboxLengthX * bboxLengthY * bboxLengthZ;
		
		printf("node: %d\tleft: %s(%d)\t\t\tright: %s(%d)\t\t\tvolume: %f\n", i, child0_type, child0_index, child1_type, child1_index, bboxVolume);
	}
}

// -----------------------------------------------------------------
void BSPLoader::ReadVertices()
{
	// Get Vertex data offset and size from its LUMP
	int32_t vertexDataOffset = m_Header.lump[LUMP_VERTICES].nOffset;
	int32_t vertexDataSize = m_Header.lump[LUMP_VERTICES].nLength;

	m_nVertices = vertexDataSize / sizeof(VECTOR3D);

	// Allocate vertices memory
	m_Vertices = new VECTOR3D[m_nVertices];

	// Read Node array from file
	m_FileStream.seekg(vertexDataOffset, std::ios::beg);
	m_FileStream.read((char*)m_Vertices, vertexDataSize);

	// Print all vertices
	printf("Number of vertices : %u\n", m_nVertices);
	/*for (unsigned i = 0; i < nVertices; i++) {
		printf("Vertex %u: %f,%f,%f\n", i, m_Vertices[i].x, m_Vertices[i].y, m_Vertices[i].z);
	}*/
}

// -----------------------------------------------------------------
void BSPLoader::ReadPlanes()
{
	// Get Vertex data offset and size from its LUMP
	int32_t planeDataOffset = m_Header.lump[LUMP_PLANES].nOffset;
	int32_t planeDataSize = m_Header.lump[LUMP_PLANES].nLength;

	m_nPlanes = planeDataSize / sizeof(BSPPLANE);

	// Allocate vertices memory
	m_Planes = new BSPPLANE[m_nPlanes];

	// Read Node array from file
	m_FileStream.seekg(planeDataOffset, std::ios::beg);
	m_FileStream.read((char*)m_Planes, planeDataSize);

	// Print all planes
	printf("Number of planes : %u\n", m_nPlanes);
	/*for (unsigned i = 0; i < nPlanes; i++) {
		printf("Plane %u: Normal=%f,%f,%f\n", i, m_Planes[i].vNormal.x, m_Planes[i].vNormal.y, m_Planes[i].vNormal.z);
	}*/
}

// -----------------------------------------------------------------
void BSPLoader::ReadEdges()
{
	// Get Vertex data offset and size from its LUMP
	int32_t edgeDataOffset = m_Header.lump[LUMP_EDGES].nOffset;
	int32_t edgeDataSize = m_Header.lump[LUMP_EDGES].nLength;

	m_nEdges = edgeDataSize / sizeof(BSPEDGE);

	// Allocate vertices memory
	m_Edges = new BSPEDGE[m_nEdges];

	// Read Node array from file
	m_FileStream.seekg(edgeDataOffset, std::ios::beg);
	m_FileStream.read((char*)m_Edges, edgeDataSize);

	// Print all edges
	printf("Number of edges : %u\n", m_nEdges);
	/*for (unsigned i = 0; i < nEdges; i++) {

		// Get vertex IDs
		unsigned v0_Id = m_Edges[i].iVertex[0];
		unsigned v1_Id = m_Edges[i].iVertex[1];

		// Edges index into vertices array
		assert(m_Vertices);

		// Get vertirces
		VECTOR3D v0 = m_Vertices[v0_Id];
		VECTOR3D v1 = m_Vertices[v1_Id];

		printf("Edge %u: V0=(%f,%f,%f) V1=(%f,%f,%f)\n", i, v0.x, v0.y, v0.z, v1.x, v1.y, v1.z);
	}*/
}

// -----------------------------------------------------------------
void BSPLoader::ReadSurfEdges()
{
	// Get Vertex data offset and size from its LUMP
	int32_t surfEdgeDataOffset = m_Header.lump[LUMP_SURFEDGES].nOffset;
	int32_t surfEdgeDataSize = m_Header.lump[LUMP_SURFEDGES].nLength;

	m_nSurfEdges = surfEdgeDataSize / sizeof(BSPSURFEDGE);

	// Allocate vertices memory
	m_SurfEdges = new BSPSURFEDGE[m_nSurfEdges];

	// Read Node array from file
	m_FileStream.seekg(surfEdgeDataOffset, std::ios::beg);
	m_FileStream.read((char*)m_SurfEdges, surfEdgeDataSize);

	// Print all surfedges
	printf("Number of surface edges : %u\n", m_nSurfEdges);
}

// -----------------------------------------------------------------
void BSPLoader::ReadTextures()
{
	// Get Vertex data offset and size from its LUMP
	int32_t textureDataOffset = m_Header.lump[LUMP_TEXTURES].nOffset;
	int32_t textureDataSize = m_Header.lump[LUMP_TEXTURES].nLength;

	// First, read the texture header
	BSPTEXTUREHEADER textureHeader;

	// texture header only
	m_FileStream.seekg(textureDataOffset, std::ios::beg);
	m_FileStream.read((char*)&textureHeader, sizeof(BSPTEXTUREHEADER));

	//printf("Number of texture offsets : %u\n", textureHeader.nMipTextures);

	m_nTextures = textureHeader.nMipTextures;

	// Read texture offsets
	m_TextureOffsets = new BSPMIPTEXOFFSET[m_nTextures];
	m_Textures = new BSPMIPTEX[m_nTextures];

	// texture header only
	m_FileStream.read((char*)m_TextureOffsets, sizeof(BSPMIPTEXOFFSET) * m_nTextures);

	// Read texture MIPOFFSETS
	for (unsigned i = 0; i < m_nTextures; i++) {
		//printf("Texture offset : %u :: %u\n", i, m_TextureOffsets[i]);
		m_FileStream.seekg(textureDataOffset + m_TextureOffsets[i], std::ios::beg);
		m_FileStream.read((char*)&m_Textures[i], sizeof(BSPMIPTEX));
	}

	// Print textures
	printf("Number of textures : %u\n", m_nTextures);
	/*for (unsigned i = 0; i < m_nTextures; i++) {
		BSPMIPTEX tex = m_Textures[i];
		printf("Texture %u : Name=%s Size=%ux%u Offsets=%u,%u,%u,%u\n",
			i, tex.szName, tex.nWidth, tex.nHeight,
			tex.nOffsets[0], tex.nOffsets[1], tex.nOffsets[2], tex.nOffsets[3]);
	}*/
}

// -----------------------------------------------------------------
void BSPLoader::ReadTexInfo()
{
	// Get TexInfo offset and size from its LUMP
	int32_t texInfoDataOffset = m_Header.lump[LUMP_TEXINFO].nOffset;
	int32_t texInfoDataSize = m_Header.lump[LUMP_TEXINFO].nLength;

	m_nTextureInfos = texInfoDataSize / sizeof(BSPTEXTUREINFO);

	// Allocate texture infos
	m_TextureInfos = new BSPTEXTUREINFO[m_nTextureInfos];

	// Read Node array from file
	m_FileStream.seekg(texInfoDataOffset, std::ios::beg);
	m_FileStream.read((char*)m_TextureInfos, texInfoDataSize);

	printf("Number of TexInfos : %u\n", m_nTextureInfos);
}

// -----------------------------------------------------------------
void BSPLoader::ReadFaces() {
	// Get Vertex data offset and size from its LUMP
	int32_t faceDataOffset = m_Header.lump[LUMP_FACES].nOffset;
	int32_t faceDataSize = m_Header.lump[LUMP_FACES].nLength;

	m_nFaces = faceDataSize / sizeof(BSPFACE);

	// Allocate vertices memory
	m_Faces = new BSPFACE[m_nFaces];

	// Read Node array from file
	m_FileStream.seekg(faceDataOffset, std::ios::beg);
	m_FileStream.read((char*)m_Faces, faceDataSize);

	printf("Number of Faces : %u\n", m_nFaces);
}

// -----------------------------------------------------------------
void BSPLoader::ProcessEntity(map<string, string>& attributes) {
	// Entities are defined according to their classnames
	if (attributes.find("classname") != attributes.end()) {
		std::string classname = attributes["classname"];
		//printf("Classname : %s\n", classname.c_str());

		// ------------ worldspawn ------------
		if (classname == "worldspawn") {
			m_worldspawn.model = &m_Models[0];
			//printf("Entity : worldspawn Model=0.\n");
		}
		// ------------ func_wall ------------
		else if (classname == "func_wall") {
			//printf("Entity : func_wall!\n");
			entity_funcwall afuncwall;
			// Get the model string
			string modelStr = attributes["model"];
			// model strings are of format : *N where N is the model I
			int modelIdx = stoi(modelStr.substr(1));
			afuncwall.model = &m_Models[modelIdx];
			// Add the func_wall into our list of funcwalls
			m_funcwalls.push_back(afuncwall);
			//printf("Entity : func_wall Model=%d.\n", modelIdx);

		}
		// ------------ func_breakable ------------
		else if (classname == "func_breakable") {
			entity_funcbreakable afuncbreakable;
			// Get the model string
			std::string modelStr = attributes["model"];
			// model strings are of format : *N where N is the model I
			int modelIdx = stoi(modelStr.substr(1));
			afuncbreakable.model = &m_Models[modelIdx];
			// Add the func_breakable into our list of funcbreakables
			m_funcbreakables.push_back(afuncbreakable);
			//printf("Entity : func_breakable Model=%d.\n", modelIdx);
		}
		else {
			//printf("[WARNING] Entity isn't supported (yet)!\n");
		}

	}
}

// -----------------------------------------------------------------
void BSPLoader::ProcessEntityStr(string& entityStr) {
	//printf("Entity : [%s]\n", entityStr.c_str());

	// Process every line
	// Add key : value pair to a map
	// Now based on 'classname' key, process that entity

	size_t size = entityStr.size();
	// Trim the entityStr between the start and end quotes
	size_t first_newline = entityStr.find_first_of('\"');
	size_t last_newline = entityStr.find_last_of('\"');
	string trimmedStr = entityStr.substr(first_newline, last_newline);
	
	map<string, string> entityAttributes;

	// Now iterate through each line of string
	size_t start = 0;
	size_t end = 0;
	size_t lineNum = 0;
	while (true) {
		// Find the next newline character
		end = trimmedStr.find('\n', start);
		if (end == std::string::npos)
			break;

		// Get line string
		std::string line = trimmedStr.substr(start, end - start);

		// There might be a \r before \n so need to trim it as well
		size_t end_trimmed = line.find_last_of('\"');
		std::string line_trimmed = line.substr(0, end_trimmed + 1);

		//printf("Line : %u :: [%s]\n", lineNum, line_trimmed.c_str());
	
		// A line is always of this format:
		// "key" "value"
		size_t spacePos = line_trimmed.find(' ');
		string key = line_trimmed.substr(1, spacePos - 2);
		string value = line_trimmed.substr(spacePos + 2, line_trimmed.size() - spacePos - 3);

		//printf("Line : %u :: Key=%s Pair=%s\n", lineNum, key.c_str(), value.c_str());
		entityAttributes[key] = value;

		start = end + 1;
		lineNum++;
	}

	/*for (auto i : entityAttributes) {
		printf("Entity attribute: %s :: %s\n", i.first.c_str(), i.second.c_str());
	}*/
	ProcessEntity(entityAttributes);
}

// -----------------------------------------------------------------
void BSPLoader::ReadEntities()
{
	// Get Vertex data offset and size from its LUMP
	int32_t entityDataOffset = m_Header.lump[LUMP_ENTITIES].nOffset;
	int32_t entityDataSize = m_Header.lump[LUMP_ENTITIES].nLength;

	// Allocate vertices memory
	char* entities = new char[entityDataSize];

	// Read Node array from file
	m_FileStream.seekg(entityDataOffset, std::ios::beg);
	m_FileStream.read(entities, entityDataSize);

	// Print all surfedges
	//printf("Entities : %s\n", entities);

	// C++ strings are simpler to use
	std::string entitiesStr(entities);
	delete[] entities;

	// Extract entities from the string

	// An entity's information is enclosed in between { and } brackets
	size_t start = 0;
	size_t end = 0;
	while (true) {
		// Find the next '{' between [start, ...]
		start = entitiesStr.find('{', start);
		// No more '{' present
		if (start == std::string::npos)
			break;
		// Find the next '}' after start
		end = entitiesStr.find('}', start + 1);
		// No more '}' present
		if (end == std::string::npos)
			break;
		// Extract entity's string between start and end
		std::string entityStr = entitiesStr.substr(start, end - start + 1);
		ProcessEntityStr(entityStr);
		start = end + 1;
	}

	// Print all processed entities
	/*m_worldspawn.printInfo();			// worldspawn
	for (auto i : m_funcwalls)			// func_walls
		i.printInfo();
	for (auto i : m_funcbreakables)		// func_breakables
		i.printInfo();*/
}

// -----------------------------------------------------------------
void BSPLoader::ReadModels()
{
	// Get Vertex data offset and size from its LUMP
	int32_t modelDataOffset = m_Header.lump[LUMP_MODELS].nOffset;
	int32_t modelDataSize = m_Header.lump[LUMP_MODELS].nLength;

	unsigned nModels = modelDataSize / sizeof(BSPMODEL);

	// Allocate vertices memory
	m_Models = new BSPMODEL[nModels];

	// Read Node array from file
	m_FileStream.seekg(modelDataOffset, std::ios::beg);
	m_FileStream.read((char*)m_Models, modelDataSize);

	// Print all surfedges
	printf("Number of Models : %u\n", nModels);
}

// -----------------------------------------------------------------
void BSPLoader::ReadLeaves() {

	// Get Leaves data offset and size from its LUMP
	int32_t leavesDataOffset = m_Header.lump[LUMP_LEAVES].nOffset;
	int32_t leavesDataSize = m_Header.lump[LUMP_MODELS].nLength;
	
	unsigned nLeaves = leavesDataSize / sizeof(BSPLEAF);

	// Allocate leaves memory
	m_Leaves = new BSPLEAF[nLeaves];

	// Read Node array from file
	m_FileStream.seekg(leavesDataOffset, std::ios::beg);
	m_FileStream.read((char*)m_Leaves, leavesDataSize);

	// Print all surfedges
	printf("Number of Leaves : %u\n", nLeaves);

	for (unsigned i = 0; i < nLeaves; i++) {
		printf("Leaf : %u Type: %d nFaces: %u\n", i, m_Leaves[i].nContents, m_Leaves[i].nMarkSurfaces);
	}
}
