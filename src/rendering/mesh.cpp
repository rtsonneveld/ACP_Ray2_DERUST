#include "mesh.h"
#include <glad/glad.h>
//#include "cube_data.h"

#include "mesh.h"
#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
# define M_PI           3.14159265358979323846  /* pi */

Mesh::Mesh(std::vector<float> vertices, std::vector<unsigned int> indices) {
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

Mesh Mesh::createSphere(float radius, glm::vec3 offset, int n_stacks, int n_slices) {

  std::vector<float> vertices;
  std::vector<unsigned int> indices;

  // Add top vertex (with radius and offset)
  vertices.push_back(offset.x + 0.0f);
  vertices.push_back(offset.y + radius);
  vertices.push_back(offset.z + 0.0f);

  // Add vertices for each stack/slice (excluding poles)
  for (int i = 0; i < n_stacks - 1; i++)
  {
    double phi = M_PI * double(i + 1) / double(n_stacks);
    for (int j = 0; j < n_slices; j++)
    {
      double theta = 2.0 * M_PI * double(j) / double(n_slices);
      float x = radius * std::sin(phi) * std::cos(theta);
      float y = radius * std::cos(phi);
      float z = radius * std::sin(phi) * std::sin(theta);
      vertices.push_back(offset.x + x);
      vertices.push_back(offset.y + y);
      vertices.push_back(offset.z + z);
    }
  }

  // Add bottom vertex (with radius and offset)
  vertices.push_back(offset.x + 0.0f);
  vertices.push_back(offset.y - radius);
  vertices.push_back(offset.z + 0.0f);

  unsigned int top_index = 0;
  unsigned int bottom_index = static_cast<unsigned int>(vertices.size() / 3 - 1);

  // Top cap triangles
  for (int i = 0; i < n_slices; ++i)
  {
    unsigned int i0 = i + 1;
    unsigned int i1 = (i + 1) % n_slices + 1;
    indices.insert(indices.end(), { top_index, i1, i0 });
  }

  // Bottom cap triangles
  for (int i = 0; i < n_slices; ++i)
  {
    unsigned int i0 = i + n_slices * (n_stacks - 2) + 1;
    unsigned int i1 = (i + 1) % n_slices + n_slices * (n_stacks - 2) + 1;
    indices.insert(indices.end(), { bottom_index, i0, i1 });
  }

  // Quads (as two triangles per quad)
  for (int j = 0; j < n_stacks - 2; j++)
  {
    unsigned int j0 = j * n_slices + 1;
    unsigned int j1 = (j + 1) * n_slices + 1;

    for (int i = 0; i < n_slices; i++)
    {
      unsigned int i0 = j0 + i;
      unsigned int i1 = j0 + (i + 1) % n_slices;
      unsigned int i2 = j1 + (i + 1) % n_slices;
      unsigned int i3 = j1 + i;

      // Triangle 1
      indices.insert(indices.end(), { i0, i1, i2 });
      // Triangle 2
      indices.insert(indices.end(), { i0, i2, i3 });
    }
  }

  return Mesh(vertices, indices);
}


Mesh::~Mesh() {
}

void Mesh::draw() {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
}
