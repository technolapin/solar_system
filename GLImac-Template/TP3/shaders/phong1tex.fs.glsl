#version 300 es
precision mediump float;

in vec3 vNormale;
in vec2 vUV;
in vec3 fPos;
in vec3 fVPos;

out vec3 fFragColor;

vec3 phong(vec3 diffuse,
           vec3 ambiant,
           vec3 specular,
           float spec_exponent,
           vec3 light_intensity,
           vec3 light_direction)
{

   float diff = max(dot(vNormale, -light_direction), 0.0);
   vec3 amb = ambiant*light_intensity;

   vec3 camera_dir = normalize(-fVPos);
   vec3 half_vec = (camera_dir + normalize(-light_direction))/2.;

   float spec = pow(max(0., dot(half_vec, vNormale)),
                    spec_exponent);
   
   float lol = dot(half_vec, vNormale);
   return half_vec;
   return diff*light_intensity*diffuse+ spec*specular;
}




struct Light
{
   vec3 dir;
   vec3 col;
};

uniform sampler2D uTexture;
uniform mat2x3[10] uDirLights;
uniform mat2x3[10] uPtsLights;

void main()
{
   vec3 color = vec3(0., 0., 0);

   for (int i = 0; i < 1; ++i)
   {
      vec3 ldir = uDirLights[i][0];
      vec3 lcol = uDirLights[i][1];

//      ldir = vec3(1, 1, 1);
  //    lcol = vec3(0.2, 0.2, 0.2);
      color += phong(texture(uTexture, vUV).rgb,
                     vec3(1.0),
                     vec3(1.0),
                     1.00,
                     lcol,
                     ldir);
         
      
   }
//   fFragColor = texture(uTexture, vUV).rgb;
   fFragColor = color;
  // fFragColor = uDirLights[0][1];
}

