

struct AnyLight
{
   glm::vec3 direction;
   glm::vec3 intensity;

   glm::mat2x3 into_mat()
   {
      return glm::mat2x3(direction, intensity);
   }
    
    static std::vector<GLfloat>
    parse_lights_dir(std::vector<AnyLight> lights, glm::mat4 v_mat)
    {
//      std::cout << "TINKERING LIGHT\n";
        std::vector<GLfloat> data;
      for (auto & light: lights)
      {
         auto pos = glm::vec4(light.direction, 0.0);
         auto view_pos = v_mat*pos;
         AnyLight placed = {glm::vec3(view_pos), light.intensity};
         auto floats = glm::value_ptr(placed.into_mat());
         for (auto i = 0; i < 6; ++i)
         {
            //std::cout << floats[i] << " ";
            data.push_back(floats[i]);
         }         
         
         //std::cout << std::endl;
      }
      return data;
      
   }

    static std::vector<GLfloat>
    parse_lights_pts(std::vector<AnyLight> lights, glm::mat4 v_mat)
    {
//      std::cout << "TINKERING LIGHT\n";
        std::vector<GLfloat> data;
        for (auto & light: lights)
        {
            auto pos = glm::vec4(light.direction, 1.0);
            auto view_pos = v_mat*pos;
            AnyLight placed = {glm::vec3(view_pos), light.intensity};
         auto floats = glm::value_ptr(placed.into_mat());
         for (auto i = 0; i < 6; ++i)
         {
            //std::cout << floats[i] << " ";
            data.push_back(floats[i]);
         }         
         
         //std::cout << std::endl;
      }
      return data;
      
   }


};

enum LightType
{
   Directional,
   Ponctual
};

struct Light
{
   LightType light_type;
   AnyLight light;
};

