#pragma once

namespace Shaders {

  namespace WOIT_FullScreenPresent {
    inline const char* Fragment = R""""(

#version 430 core

layout(location = 0) uniform sampler2D accum_tex;
layout(location = 1) uniform sampler2D reveal_tex;
out vec4 frag_col;
in vec2 v_uv;
void main() 
{
  float revealage = texture( reveal_tex, v_uv ).r;
  vec4 accum = texture( accum_tex, v_uv );  

  if (revealage == 1.0) { discard; }
  vec3 avg_col = accum.rgb / clamp( accum.a, 1e-3, 5e4 );
  frag_col = vec4( avg_col, revealage );
}

)"""";


    inline const char* Vertex = R""""(

      #version 330 core
      layout( location = 0 ) in vec3 pos;

      out vec2 v_uv;
      void main()
      {
        v_uv = 0.5 * (pos.xy + 1.0);
        gl_Position = vec4( pos, 1.0 );
      }

  )"""";

  }

}