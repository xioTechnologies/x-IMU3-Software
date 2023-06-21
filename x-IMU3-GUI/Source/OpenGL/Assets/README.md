# How to generate .obj and .mtl files from a STEP file

1. Import STEP file (with colour) into [FreeCAD](https://www.freecad.org/). Disable: Edit > Preferences > Import-Export > STEP > Enable STEP compound merge
2. Export gITF file from FreeCAD
3. Import gITF file into [Blender](https://www.blender.org/)
4. Fix orientation and origin in Blender
5. Export Wavefront (.obj) from Blender. Transform: Y Forward, Z Up
