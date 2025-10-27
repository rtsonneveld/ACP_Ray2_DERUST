#pragma once

namespace Primitives {
  namespace Cube {
    const inline float vertices[] = {
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,  -0.5f, 0.5f,-0.5f,
        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,  -0.5f, 0.5f, 0.5f
    };

    const inline unsigned int indices[] = {
      // (z+)
      4,5,6, 6,7,4,
      // (z-)
      2,1,0, 0,3,2,
      // (y-)
      0,1,5, 5,4,0,
      // (y+)
      2,3,7, 7,6,2,
      // (x-)
      7,3,0, 0,4,7,
      // (x+)
      1,2,6, 6,5,1
    };
  }
}