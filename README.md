# bsp2fbx

### Introduction

This is a utility which converts a GoldSrc engine BSP map file to industry standard FBX format. This basically includes all original Half-Life and Counter-Strike maps. FBX today is used across all major renderers and game engines so it makes sense to revive old-school content in this particular format. Some example renders from Blender and Unreal Engine 4:

<img src="https://github.com/pdsharma0/bsp2fbx/blob/master/images/blender/RPR_aztec_11.png?raw=true" width="280"> <img src="https://github.com/pdsharma0/bsp2fbx/blob/master/images/blender/RPR_dust_2.png?raw=true" width="280"> <img src="https://github.com/pdsharma0/bsp2fbx/blob/master/images/blender/RPR_inferno_1.png?raw=true" width="280">

<img src="https://github.com/pdsharma0/bsp2fbx/blob/master/images/ue4/cstrike_italy.png?raw=true" width="280"> <img src="https://github.com/pdsharma0/bsp2fbx/blob/master/images/ue4/hl_bootcamp.png?raw=true" width="280"> <img src="https://github.com/pdsharma0/bsp2fbx/blob/master/images/ue4/cstrike_office.png?raw=true" width="280">

### Usage

To generate an FBX file from a BSP file:

```
bsp2fbx.exe xyz.bsp
```

This will create a xyz.fbx in the same folder where the BSP file is. Note that only **GoldSrc v30** BSP files are currently supported so Quake2 and Source Engine BSP files for example will probably result in an error.

Now to get the bsp2fbx.exe, either download a [release](https://github.com/pdsharma0/bsp2fbx/releases) or compile the bsp2fbx.sln file. In both cases you'll first need the Autodesk's FBX SDK which can be downloaded from here : https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2019-0. This SDK contains a libfbxsdk.dll which needs to be in your PATH environment variable before running the executable.

Compiling the solution file requires the environment variable FBX_SDK to be set pointing to where you installed your SDK. For example, mine points to "C:\Program Files\Autodesk\FBX\FBX SDK\2019.0".

### BSP format

The *unofficial* [BSP v30 spec](http://hlbsp.sourceforge.net/index.php?content=bspdef) and [Quake2 BSP spec](http://www.flipcode.com/archives/Quake_2_BSP_File_Format.shtml) were used as a reference.

Only these BSP entities having visible geometries are currently loaded into the FBX scene:
* [**worldspawn**](https://developer.valvesoftware.com/wiki/Worldspawn) : Contains *brushes* which comprise most of the visible geometry in a map. Also contains other information like dependent WAD files and which skybox to use. 
* [**func_wall**](https://developer.valvesoftware.com/wiki/Func_wall) : These are brushes as well but just separated out from worldspawn for some reason.
* [**func_breakable**](https://developer.valvesoftware.com/wiki/Func_breakable) : These are special brushes which can break.

### FBX Scene description

The FBX file format is quite flexible and defines a [scene](https://help.autodesk.com/view/FBX/2017/ENU/?guid=__files_GUID_4F644045_380D_4B75_A2A3_D39DDE53BEDD_htm) as a hierarchy of [nodes](https://help.autodesk.com/view/FBX/2017/ENU/?guid=__files_GUID_02CF7021_71FF_4F31_A072_291EF86CCA5A_htm). Each node can have many attributes like a mesh, a light, a camera etc. 

The generated FBX contains a *visible_geometries* node under root. A node is created for every visible entity in the BSP file described above and added as a child to this node. Each such node in-turn has a mesh attribute which was generated from the BSPMODEL referenced by the entity.

### Importing FBX file

Polygon normals and tangents are exported in the FBX file but they don't have any smoothing applied so it might result in a few visible artifacts. It's advisable to generate normals while importing to fix this problem. This has been tested in both UE4 and Cryengine and is known to get rid of such artifacts.

### Future Work

* Export skybox, lights and materials as well.
* Support [Source Engine BSP format](http://bagthorpe.org/bob/cofrdrbob/bspformat.html)