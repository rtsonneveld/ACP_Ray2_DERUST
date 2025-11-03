#pragma once

namespace Primitives {
  namespace Octahedron {
    // 6 vertices of an octahedron
    const inline float vertices[] = {
      // X-axis points
       0.5f,  0.0f,  0.0f,   // 0
      -0.5f,  0.0f,  0.0f,   // 1
      // Y-axis points
       0.0f,  0.5f,  0.0f,   // 2
       0.0f, -0.5f,  0.0f,   // 3
       // Z-axis points
        0.0f,  0.0f,  0.5f,   // 4
        0.0f,  0.0f, -0.5f    // 5
    };

    // 8 triangular faces (each defined by 3 vertex indices)
    const inline unsigned int indices[] = {
      // Top pyramid (toward +Z)
      0, 2, 4,
      2, 1, 4,
      1, 3, 4,
      3, 0, 4,

      // Bottom pyramid (toward -Z)
      2, 0, 5,
      1, 2, 5,
      3, 1, 5,
      0, 3, 5
    };
  }
}
