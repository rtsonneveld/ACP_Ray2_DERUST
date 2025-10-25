#include "geo_mesh.hpp"
#include "cpa_glm_util.hpp"

// Cache
std::unordered_map<GEO_tdstGeometricObject*, std::shared_ptr<GeometricObjectMesh>> GeometricObjectMesh::cache;

void GeometricObjectMesh::clearCache() {
  cache.clear();
}

std::shared_ptr<GeometricObjectMesh> GeometricObjectMesh::get(GEO_tdstGeometricObject* po) {
  auto it = cache.find(po);
  if (it != cache.end()) {
    return it->second;
  }

  auto newMesh = std::make_shared<GeometricObjectMesh>(po);
  cache[po] = newMesh;
  return newMesh;
}

GeometricObjectMesh::GeometricObjectMesh(GEO_tdstGeometricObject* geomObj) {

  for (int i = 0;i < geomObj->xNbElements;i++) {
    void* element = geomObj->d_hListOfElements[i];

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    GEO_tdstElementIndexedTriangles* elemTris;
    GEO_tdstElementSpheres* elemSpheres;
    
    unsigned short colMat = 0;

    switch (geomObj->d_xListOfElementsTypes[i]) {
      case GEO_C_xElementNULL: break;

      case GEO_C_xElementIndexedTriangles:
        elemTris = (GEO_tdstElementIndexedTriangles*)element;
        colMat = elemTris->hMaterial != nullptr && elemTris->hMaterial->hCollideMaterial != nullptr && 
          (int)elemTris->hMaterial->hCollideMaterial != 0xFFFFFFFF ?
                    elemTris->hMaterial->hCollideMaterial->xIdentifier : 0;
      
        for (int j = 0; j < geomObj->xNbPoints;j++) {
          auto point = geomObj->d_stListOfPoints[j];
          vertices.push_back(point.x);
          vertices.push_back(point.y);
          vertices.push_back(point.z);
        }

        for (int idxFace = 0; idxFace < elemTris->xNbFaces;idxFace++) {

          for (int idxPoint = 0; idxPoint < 3;idxPoint++) {
            int idxInPointsList = elemTris->d_stListOfFacesTripled[idxFace].a3_xIndex[idxPoint];

            indices.push_back(idxInPointsList);
          }
        }

        meshes.push_back(Mesh(vertices, indices, 1.0f, colMat));
        break;
      case GEO_C_xElementSpheres:
        elemSpheres = (GEO_tdstElementSpheres*)element;
        for (int sphereIdx = 0;sphereIdx < elemSpheres->xNbSpheres; sphereIdx++) {
            
          auto sphere = elemSpheres->d_stListOfSpheres[sphereIdx];
          
          auto offset = geomObj->d_stListOfPoints[sphere.xCenterPoint];
          auto radius = sphere.xRadius;

          meshes.push_back(Mesh::createSphere(radius, ToGLMVec(offset)));
        }
        break;
    }
  }
}

void GeometricObjectMesh::draw(Shader * shader) {
  for (Mesh& mesh : meshes) {
    mesh.draw(shader);
  }
}
