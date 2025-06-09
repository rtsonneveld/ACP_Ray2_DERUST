#include "mesh.h"
#include <glad/glad.h>
//#include "cube_data.h"

#include "mesh.h"
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>

Mesh::Mesh(std::vector<float> vertices, std::vector<int> indices) {
  numIndices = indices.size();

  // Step 1: Prepare normal array (same number of vertices)
  size_t vertexCount = vertices.size() / 3;
  std::vector<glm::vec3> normals(vertexCount, glm::vec3(0.0f));

  // Step 2: Accumulate face normals
  for (size_t i = 0; i < indices.size(); i += 3) {
    int i0 = indices[i];
    int i1 = indices[i + 1];
    int i2 = indices[i + 2];

    glm::vec3 v0(vertices[i0 * 3 + 0], vertices[i0 * 3 + 1], vertices[i0 * 3 + 2]);
    glm::vec3 v1(vertices[i1 * 3 + 0], vertices[i1 * 3 + 1], vertices[i1 * 3 + 2]);
    glm::vec3 v2(vertices[i2 * 3 + 0], vertices[i2 * 3 + 1], vertices[i2 * 3 + 2]);

    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

    normals[i0] += faceNormal;
    normals[i1] += faceNormal;
    normals[i2] += faceNormal;
  }

  // Step 3: Normalize normals
  for (auto& n : normals) {
    n = glm::normalize(n);
  }

  // Step 4: Interleave position + normal into final VBO data
  std::vector<float> vertexData;
  vertexData.reserve(vertexCount * 6);

  for (size_t i = 0; i < vertexCount; ++i) {
    vertexData.push_back(vertices[i * 3 + 0]);
    vertexData.push_back(vertices[i * 3 + 1]);
    vertexData.push_back(vertices[i * 3 + 2]);

    vertexData.push_back(normals[i].x);
    vertexData.push_back(normals[i].y);
    vertexData.push_back(normals[i].z);
  }

  // Step 5: OpenGL buffer setup
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

  // Position attribute (location = 0)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // Normal attribute (location = 1)
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}


Mesh::~Mesh() {
}

void Mesh::draw() {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}
