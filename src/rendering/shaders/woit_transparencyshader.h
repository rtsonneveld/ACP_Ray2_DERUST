#pragma once

namespace Shaders {

  namespace WOIT_TransparencyShader {
    inline const char* Fragment = R""""(

#version 430 core

      in vec4 v_col;
      in vec2 v_uv;
      layout( location = 0 ) out vec4 accum;
      layout( location = 1 ) out float revelage;
      void main()
      {
        float dist = sqrt( v_uv.x * v_uv.x + v_uv.y * v_uv.y );
        vec4 out_col = v_col;
        out_col.a *= (1.0 - smoothstep(0.85, 1.0, dist) );

        float csz = abs(1.0 / gl_FragCoord.w);

#if 1
        // Equation 9
        float factor = 1.0/200.0; 
        float z = factor * csz;
        float weight = clamp( (0.03 / (1e-5 + pow(z, 4.0) ) ), 1e-4, 3e3 );
#else
        // Exponential function with coefficents fit to better resemple fixed order.
        // Sensitve to changes in depth
        float a = 58.3765228;
        float b = 1.45434782;
        float c = 0.00630901288;
        float fz = a * exp(-b*csz) + c;
        float weight = clamp(fz, 1e-2, 3e3);
#endif
        accum = vec4( out_col.rgb * out_col.a, out_col.a ) * weight;
        revelage = out_col.a;
      } 

)"""";


    inline const char* Vertex = R""""(

      #version 330 core
      layout( location = 0 ) in vec3 pos;
      layout( location = 1 ) in vec4 col;
      layout( location = 2 ) in vec2 uv;

      layout( location = 0 ) uniform mat4 mvp;
      layout( location = 1 ) uniform mat4 view;
      out vec4 v_col;
      out vec2 v_uv;
      void main()
      {
        v_col = col;
        v_uv = uv;
        gl_Position = mvp * vec4( pos, 1.0 );
      }

  )"""";

  }

}