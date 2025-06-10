#pragma once
#include "mesh.h"
#include <ACP_Ray2.h>
#include <memory>
#include <unordered_map>

class GeometricObjectMesh {
public:
  GeometricObjectMesh(GEO_tdstGeometricObject* po);
  static std::shared_ptr<GeometricObjectMesh> get(GEO_tdstGeometricObject* po);
  void draw();
private:
  std::vector<Mesh> meshes;

  static std::unordered_map<GEO_tdstGeometricObject*, std::shared_ptr<GeometricObjectMesh>> cache;
};
