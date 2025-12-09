#include "geo_mesh.hpp"
#include "cpa_glm_util.hpp"
#include "textures.hpp"
#include <GLFW//glfw3.h>
#include <shared_mutex>

// Cache
std::unordered_map<GEO_tdstGeometricObject*, std::shared_ptr<GeometricObjectMesh>> GeometricObjectMesh::cache;

static std::shared_mutex cacheMutex;

void GeometricObjectMesh::clearCache() {

  std::unique_lock lock(cacheMutex);
  cache.clear();
}

std::shared_ptr<GeometricObjectMesh> GeometricObjectMesh::get(GEO_tdstGeometricObject* po) {

  std::shared_lock lock(cacheMutex);

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
    GEO_tdstElementAlignedBoxes* elemBoxes;

    unsigned short collisionFlags = 0;
    GMT_tdstCollideMaterial* collideMaterial = nullptr;

    switch (geomObj->d_xListOfElementsTypes[i]) {
      case GEO_C_xElementNULL: break;

      case GEO_C_xElementIndexedTriangles:
        elemTris = (GEO_tdstElementIndexedTriangles*)element;
      
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

        collideMaterial = elemTris->hMaterial != nullptr && elemTris->hMaterial->hCollideMaterial != nullptr &&
          (int)elemTris->hMaterial->hCollideMaterial != 0xFFFFFFFF ? elemTris->hMaterial->hCollideMaterial : nullptr;
        
        if (collideMaterial != nullptr) {
          collisionFlags = collideMaterial->xIdentifier;
        } else {
          collisionFlags = 0;
        }

        meshes.push_back({
          .mesh = Mesh(vertices, indices, 1.0f),
          .collisionFlags = collisionFlags,
        });

        break;

      case GEO_C_xElementSpheres:
        elemSpheres = (GEO_tdstElementSpheres*)element;
        for (int sphereIdx = 0;sphereIdx < elemSpheres->xNbSpheres; sphereIdx++) {
            
          GEO_tdstIndexedSphere Box = elemSpheres->d_stListOfSpheres[sphereIdx];
          
          auto offset = geomObj->d_stListOfPoints[Box.xCenterPoint];
          auto radius = Box.xRadius;

          collideMaterial = Box.hMaterial != nullptr && Box.hMaterial->hCollideMaterial != nullptr &&
            (int)Box.hMaterial->hCollideMaterial != 0xFFFFFFFF ? Box.hMaterial->hCollideMaterial : nullptr;

          if (collideMaterial != nullptr) {
            collisionFlags = collideMaterial->xIdentifier;
          } else {
            collisionFlags = 0;
          }

          meshes.push_back({
            .mesh = Mesh::createSphere(radius, ToGLMVec(offset), 8, 8),
            .collisionFlags = collisionFlags,
          });
        }
        break;

      case GEO_C_xElementAlignedBoxes:
        elemBoxes = (GEO_tdstElementAlignedBoxes*)element;
        for (int sphereIdx = 0;sphereIdx < elemBoxes->xNbAlignedBoxes; sphereIdx++) {

          GEO_tdstIndexedAlignedBox box = elemBoxes->d_stListOfAlignedBoxes[sphereIdx];

          auto minPoint = ToGLMVec(geomObj->d_stListOfPoints[box.xMinPoint]);
          auto maxPoint = ToGLMVec(geomObj->d_stListOfPoints[box.xMaxPoint]);

          collideMaterial = box.hMaterial != nullptr && box.hMaterial->hCollideMaterial != nullptr &&
            (int)box.hMaterial->hCollideMaterial != 0xFFFFFFFF ? box.hMaterial->hCollideMaterial : nullptr;

          if (collideMaterial != nullptr) {
            collisionFlags = collideMaterial->xIdentifier;
          }
          else {
            collisionFlags = 0;
          }

          auto center = minPoint + (maxPoint - minPoint) * 0.5f;
          auto size = (maxPoint - center) * 2.0f;

          meshes.push_back({
            .mesh = Mesh::createCube(size, center),
            .collisionFlags = collisionFlags,
            });
        }
        break;
    }
  }
}


void GeometricObjectMesh::setTextureBasedOnFlagsAndType(Shader* shader, MeshDrawInfo info, short zoneType) {

  if (zoneType < 0) {
    shader->setTex2D("tex1", Textures::ColDefault, 0);
    shader->setTex2D("tex2", 0, 1);
    shader->setBool("useSecondTexture", false);
    return;
  }

  std::vector<GLuint> textureTable; // zoneType switch

  GLuint defaultTexture = Textures::ColDefault; 

  switch (zoneType) {
    case ZDX_C_ucTypeZdm: textureTable = Textures::textureTableZDM; break;
    case ZDX_C_ucTypeZde: textureTable = Textures::textureTableZDE; defaultTexture = Textures::ColEvent; break;
    case ZDX_C_ucTypeZdr: textureTable = Textures::textureTableZDR; break;
    case ZDX_C_ucTypeZdd: textureTable = Textures::textureTableZDD; defaultTexture = Textures::ColZdd; break; // TODO: this doesn't work
    default: return;
  }

  std::vector<int> textures;

  for (int i = 0; i < 16; ++i) {
    if (info.collisionFlags & (1 << i)) {
      textures.push_back(textureTable[i]);
    }
  }

  size_t count = textures.size();

  if (count == 0) {
    shader->setTex2D("tex1", defaultTexture, 0);
    shader->setTex2D("tex2", 0, 1);
    shader->setBool("useSecondTexture", false);
    return;
  }
  else if (count == 1) {
    shader->setTex2D("tex1", textures[0], 0);
    shader->setTex2D("tex2", 0, 1);
    shader->setBool("useSecondTexture", false);
  }
  else if (count == 2) {
    shader->setTex2D("tex1", textures[0], 0);
    shader->setTex2D("tex2", textures[1], 1);
    shader->setBool("useSecondTexture", true);
  }
  else {

    // Cycle through textures every second (2 at a time)
    int cycle = static_cast<int>(glfwGetTime()) % count;
    int next = (cycle + 1) % count;

    shader->setTex2D("tex1", textures[cycle], 0);
    shader->setTex2D("tex2", textures[next], 1);
    shader->setBool("useSecondTexture", true);
  }
}



void GeometricObjectMesh::draw(Shader* shader, short zoneType) {
  for (MeshDrawInfo& mdi : meshes) {

    this->setTextureBasedOnFlagsAndType(shader, mdi, zoneType);
    mdi.mesh.draw(shader);
  }
}