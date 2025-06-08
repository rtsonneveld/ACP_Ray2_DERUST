#include "po_mesh.h"

/*
typedef struct GEO_tdstElementIndexedTriangles
{
  GMT_tdstGameMaterial *hMaterial;

  ACP_tdxIndex xNbFaces; = num_triangles
  ACP_tdxIndex xNbElementUV = num_uvs;
  GEO_tdstTripledIndex *d_stListOfFacesTripled; = off_triangles // 1 entry = 3 shorts. Max: geometricObject.num_vertices
  GEO_tdstTripledIndex *d_stListOfFacesTripledIndexUV; = off_mapping_uvs // 1 entry = 3 shorts. Max: num_weights
  MTH3D_tdstVector *d_stListOfFacesNormals; = off_normals // 1 entry = 3 floats
  GLI_tdst2DUVValues *d_stListOfElementUV; // 1 entry = 2 floats

  ACP_tdxIndex *d_stListOfIndexUsedByThisElement; // off_vertex_indices → not needed
  ACP_tdxIndex xNbOfIndexUsed; // num_vertex_indices → not needed

  ACP_tdxIndex xIndexOfParallelBox;
}
GEO_tdstElementIndexedTriangles;
*/


// Cache
std::unordered_map<PO_tdstPhysicalObject*, std::shared_ptr<PhysicalObjectMesh>> PhysicalObjectMesh::cache;

std::shared_ptr<PhysicalObjectMesh> PhysicalObjectMesh::get(PO_tdstPhysicalObject* po) {
  auto it = cache.find(po);
  if (it != cache.end()) {
    return it->second;
  }

  auto newMesh = std::make_shared<PhysicalObjectMesh>(po);
  cache[po] = newMesh;
  return newMesh;
}

PhysicalObjectMesh::PhysicalObjectMesh(PO_tdstPhysicalObject* po) {

  int nbLod = po->hVisualSet->xNbLodDefinitions;

  if (nbLod <= 0) return;
  // Only use the first LOD for now
  auto geomObj = po->hVisualSet->d_p_stLodDefinitions[0];

  for (int i = 0;i < geomObj->xNbElements;i++) {
    void* element = geomObj->d_hListOfElements[i];

    std::vector<float> vertices;
    std::vector<int> indices;

    GEO_tdstElementIndexedTriangles* elemTris;

    switch (geomObj->d_xListOfElementsTypes[i]) {
      case GEO_C_xElementNULL: break;

      case GEO_C_xElementIndexedTriangles:
        elemTris = (GEO_tdstElementIndexedTriangles*)element;
        
        int curIndex = 0;

        for (int idxFace = 0; idxFace < elemTris->xNbFaces;idxFace++) {

          for (int idxPoint = 0; idxPoint < 3;idxPoint++) {
            int idxInPointsList = elemTris->d_stListOfFacesTripled[idxFace].a3_xIndex[idxPoint];
            
            auto point = geomObj->d_stListOfPoints[idxInPointsList];
            vertices.push_back(point.x);
            vertices.push_back(point.y);
            vertices.push_back(point.z);

            indices.push_back(curIndex);
            curIndex++;
          }
        }

        meshes.push_back(Mesh(vertices, indices));
      break;
    }
  }
}

void PhysicalObjectMesh::draw() {
  for (Mesh& mesh : meshes) {
    mesh.draw();
  }
}
