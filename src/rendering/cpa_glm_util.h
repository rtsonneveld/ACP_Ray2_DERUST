#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ACP_Ray2.h>

// Conversion from legacy vector to glm
inline glm::vec3 ToGLMVec(const MTH3D_tdstVector& v)
{
  return glm::vec3(v.x, v.y, v.z);
}

// Conversion from glm to legacy vector
inline MTH3D_tdstVector FromGLMVec(const glm::vec3& v)
{
  return MTH3D_tdstVector{ v.x, v.y, v.z };
}

// Conversion from legacy 3x3 matrix to glm::mat3
inline glm::mat3 ToGLMMat3(const MTH3D_tdstMatrix& m)
{
  return glm::mat3(
    glm::vec3(m.stCol_0.x, m.stCol_0.z, -m.stCol_0.y),
    glm::vec3(m.stCol_1.x, m.stCol_1.z, -m.stCol_1.y),
    glm::vec3(m.stCol_2.x, m.stCol_2.z, -m.stCol_2.y)
  );
}

// Conversion from glm::mat3 to legacy matrix
inline MTH3D_tdstMatrix FromGLMMat3(const glm::mat3& m)
{
  return MTH3D_tdstMatrix{
      FromGLMVec(m[0]),
      FromGLMVec(m[1]),
      FromGLMVec(m[2])
  };
}

// Convert POS_tdstCompletePosition to glm::mat4
inline glm::mat4 ToGLMMat4(const POS_tdstCompletePosition& pos)
{
  glm::mat3 rot = ToGLMMat3(pos.stRotationMatrix);
  glm::vec3 trans = ToGLMVec(pos.stTranslationVector);

  glm::mat4 result(1.0f);
  result[0] = glm::vec4(rot[0], 0.0f);
  result[1] = glm::vec4(rot[1], 0.0f);
  result[2] = glm::vec4(rot[2], 0.0f);
  result[3] = glm::vec4(trans, 1.0f);

  return result;
}

// Convert glm::mat4 to POS_tdstCompletePosition
inline POS_tdstCompletePosition FromGLMMat4(const glm::mat4& m)
{
  POS_tdstCompletePosition pos{};
  pos.eType = MTH_C_Type_CompleteMatrix;
  pos.stTranslationVector = FromGLMVec(glm::vec3(m[3]));

  glm::mat3 rot;
  rot[0] = glm::vec3(m[0]);
  rot[1] = glm::vec3(m[1]);
  rot[2] = glm::vec3(m[2]);

  pos.stRotationMatrix = FromGLMMat3(rot);
  pos.stTransformMatrix = FromGLMMat3(rot); // Assuming same usage
  return pos;
}
