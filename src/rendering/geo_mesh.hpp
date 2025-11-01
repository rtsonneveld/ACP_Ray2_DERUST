#pragma once
#include "mesh.hpp"
#include <ACP_Ray2.h>
#include <memory>
#include <unordered_map>
#include "shader.hpp"
#include <functional>

class GeometricObjectMesh {
public:
  GeometricObjectMesh(GEO_tdstGeometricObject* po);
  static std::shared_ptr<GeometricObjectMesh> get(GEO_tdstGeometricObject* po);
  static void clearCache();
  void draw(Shader* shader, short zoneType = -1);
private:

  struct MeshDrawInfo {
    Mesh mesh;
    unsigned short collisionFlags;
    //unsigned short zoneType; // BEWARE: based on GMT_C_w..., not ZDX_C_ucType...!
  };

  std::vector<MeshDrawInfo> meshes;
  GEO_tdstGeometricObject* geomObj;

  static std::unordered_map<GEO_tdstGeometricObject*, std::shared_ptr<GeometricObjectMesh>> cache;

  void setTextureBasedOnFlagsAndType(Shader* shader, MeshDrawInfo info, short zoneType);
};
