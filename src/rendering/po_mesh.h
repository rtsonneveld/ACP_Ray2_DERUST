#pragma once
#include "mesh.h"
#include <ACP_Ray2.h>
#include <memory>
#include <unordered_map>

class PhysicalObjectMesh {
public:
  PhysicalObjectMesh(PO_tdstPhysicalObject* po);
  static std::shared_ptr<PhysicalObjectMesh> get(PO_tdstPhysicalObject* po);
  void draw();
private:
  std::vector<Mesh> meshes;

  static std::unordered_map<PO_tdstPhysicalObject*, std::shared_ptr<PhysicalObjectMesh>> cache;
};
