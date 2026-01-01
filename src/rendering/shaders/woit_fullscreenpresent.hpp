#pragma once

namespace Shaders {

  namespace WOIT_FullScreenPresent {
    inline const char* Fragment = R""""(

#version 430 core

layout(location = 0) uniform sampler2D accum_tex;
layout(location = 1) uniform sampler2D reveal_tex;
layout(location = 2) uniform sampler2D opaque_tex;
out vec4 frag_col;
in vec2 v_uv;
void main() 
{
    // Sample the OIT buffers
    vec4 accum = texture(accum_tex, v_uv);
    float revealage = texture(reveal_tex, v_uv).r;

    // Sample the opaque scene
    vec3 opaque_col = texture(opaque_tex, v_uv).rgb;

    // If nothing transparent contributes, just use the opaque color
    if (revealage >= 1.0) {
        frag_col = vec4(opaque_col, 1.0);
        return;
    }

    // Compute average color of transparent fragments
    vec3 trans_col = accum.rgb / max(accum.a, 1e-3);

    // Composite transparent on top of opaque
    vec3 final_col = trans_col * (1.0 - revealage) + opaque_col * revealage;

    frag_col = vec4(final_col, 1.0);
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