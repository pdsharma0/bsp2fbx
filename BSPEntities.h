#pragma once

/*
	This file contains GoldSrc engine objects like entities, materials, vectors 
*/

#include <stdint.h>
#include <vector>
#include <string>
#include "BSPDefines.h"

using namespace std;

struct Color {
	uint8_t r, g, b;
};

// Sounds and Gibs
enum eMaterial {
	GLASS,
	WOOD,
	METAL,
	FLESH,
	CINDER_BLOCK,
	CEILING_TILE,
	COMPUTER,
	UNBREAKABLE_GLASS,
	ROCKS,
	NONE
};

// ------------- Various Entity types in GoldSrc engine -------------

// https://developer.valvesoftware.com/wiki/Worldspawn
struct entity_worldspawn {
	vector<string>	wads;		// List of WAD files
	string			skyname;	// Skybox string
	BSPMODEL*		model;		// Model which stores the world's geometry

	// Constructor
	entity_worldspawn() {
		skyname = "";
		model = 0;
	}

	void printInfo() {
		printf("**** Entity : worldspawn ****\n");
		printf("* wads :");
		for (auto i : wads) {
			printf(" (%s)", i.c_str());
		}
		printf(" *\n");
		printf("* sky : %s*\n", skyname.c_str());
		model->printInfo();
	}
};

// https://developer.valvesoftware.com/wiki/Func_wall
struct entity_funcwall {
	BSPMODEL*		model;
	Color			rendercolor;
	uint8_t			renderamt;

	void printInfo() {
		printf("**** Entity : func_wall ****\n");
		printf("* model : %p *\n", (void*)model);
		model->printInfo();
		/*printf("* color : %u,%u,%u,%u *\n",
			rendercolor.r, rendercolor.g, rendercolor.b,
			renderamt);*/
	}
};

// https://developer.valvesoftware.com/wiki/Func_breakable
struct entity_funcbreakable {
	BSPMODEL*		model;
	string          gibmodel;
	unsigned        explodemagnitude;
	unsigned		health;
	Color			rendercolor;
	eMaterial		material;
	string			targetname;

	void printInfo() {
		printf("**** Entity : func_breakable ****\n");
		printf("* model : %p *\n", (void*)model);
		printf("* material : %d *\n", material);
		model->printInfo();
	}
};
