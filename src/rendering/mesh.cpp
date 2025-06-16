#include "mesh.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
# define M_PI  3.14159265358979323846  // pi

Mesh::Mesh(std::vector<float> vertices, std::vector<unsigned int> indices) {
  numVertices = indices.size();

  // Allocate per-vertex smooth normal array
  size_t vertexCount = vertices.size() / 3;
  std::vector<glm::vec3> smoothNormals(vertexCount, glm::vec3(0.0f));

  // Temporary storage for per-face normal
  std::vector<glm::vec3> faceNormals(indices.size() / 3);

  // Accumulate smooth normals and compute face normals
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

    faceNormals[i / 3] = faceNormal;

    smoothNormals[i0] += faceNormal;
    smoothNormals[i1] += faceNormal;
    smoothNormals[i2] += faceNormal;
  }

  // Normalize per-vertex smooth normals
  for (auto& n : smoothNormals) {
    n = glm::normalize(n);
  }

  // Prepare per-corner data
  std::vector<float> vertexData;
  vertexData.reserve(indices.size() * 9);

  for (size_t i = 0; i < indices.size(); ++i) {
    int idx = indices[i];
    int faceIdx = i / 3;

    // Vertex pos
    vertexData.push_back(vertices[idx * 3 + 0]);
    vertexData.push_back(vertices[idx * 3 + 1]);
    vertexData.push_back(vertices[idx * 3 + 2]);

    // Smooth normal
    vertexData.push_back(smoothNormals[idx].x);
    vertexData.push_back(smoothNormals[idx].y);
    vertexData.push_back(smoothNormals[idx].z);

    // Face normal
    glm::vec3 faceNormal = faceNormals[faceIdx];
    vertexData.push_back(faceNormal.x);
    vertexData.push_back(faceNormal.y);
    vertexData.push_back(faceNormal.z);
  }

  // OpenGL buffer setup
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

  // Position attribute (location = 0)
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);

  // Smooth normal attribute (location = 1)
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));

  // Face normal attribute (location = 2)
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
}

Mesh Mesh::createSphere(float radius, glm::vec3 offset, int n_stacks, int n_slices) {

  std::vector<float> vertices;
  std::vector<unsigned int> indices;

  // Add top vertex (with radius and offset)
  vertices.push_back(offset.x + 0.0f);
  vertices.push_back(offset.y + 0.0f);
  vertices.push_back(offset.z + radius);

  // Add vertices for each stack/slice (excluding poles)
  for (int i = 0; i < n_stacks - 1; i++) {
    double phi = M_PI * double(i + 1) / double(n_stacks);
    for (int j = 0; j < n_slices; j++) {
      double theta = 2.0 * M_PI * double(j) / double(n_slices);
      float x = radius * std::sin(phi) * std::cos(theta);
      float y = radius * std::sin(phi) * std::sin(theta);
      float z = radius * std::cos(phi);
      vertices.push_back(offset.x + x);
      vertices.push_back(offset.y + y);
      vertices.push_back(offset.z + z);
    }
  }

  // Add bottom vertex (with radius and offset)
  vertices.push_back(offset.x + 0.0f);
  vertices.push_back(offset.y + 0.0f);
  vertices.push_back(offset.z - radius);

  unsigned int top_index = 0;
  unsigned int bottom_index = static_cast<unsigned int>(vertices.size() / 3 - 1);

  // Top cap triangles (reverse order)
  for (int i = 0; i < n_slices; ++i) {
    unsigned int i0 = i + 1;
    unsigned int i1 = (i + 1) % n_slices + 1;
    indices.insert(indices.end(), { top_index, i0, i1 });
  }

  // Bottom cap triangles (reverse order)
  for (int i = 0; i < n_slices; ++i) {
    unsigned int i0 = i + n_slices * (n_stacks - 2) + 1;
    unsigned int i1 = (i + 1) % n_slices + n_slices * (n_stacks - 2) + 1;
    indices.insert(indices.end(), { bottom_index, i1, i0 });
  }

  // Quads (as two triangles per quad) (reverse order)
  for (int j = 0; j < n_stacks - 2; j++) {
    unsigned int j0 = j * n_slices + 1;
    unsigned int j1 = (j + 1) * n_slices + 1;

    for (int i = 0; i < n_slices; i++) {
      unsigned int i0 = j0 + i;
      unsigned int i1 = (j0 + (i + 1) % n_slices);
      unsigned int i2 = j1 + (i + 1) % n_slices;
      unsigned int i3 = j1 + i;

      // Triangle 1 (reverse)
      indices.insert(indices.end(), { i0, i2, i1 });
      // Triangle 2 (reverse)
      indices.insert(indices.end(), { i0, i3, i2 });
    }
  }

  return Mesh(vertices, indices);
}

Mesh::~Mesh()
{
}

void Mesh::draw(Shader * shader)
{
  //shader->setColor("uColor", color);

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, numVertices);
}
