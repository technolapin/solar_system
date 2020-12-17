#include "scene.hpp"


void
Scene::print() const
{
   std::cout << "Scene {\n";
   for (auto & couple: this->instances)
   {
      std::cout << "   ";
      auto & mesh = couple.first.mesh;
      std::cout << mesh;
      std::cout << " : [";
      for (auto & inst: couple.second)
      {
         std::cout << inst;
      }
      std::cout << "]" << std::endl;
   }
   std::cout << "}\n";
}
   
   
void
Scene::with(const GLRenderable & rdr)
{
   this->instances.insert({rdr, std::vector<Instance>()});
}

void
Scene::add(const GLObject & mesh,
           const PgrmHandle & pgrm,
           const Instance & inst)
{
   GLRenderable rdr = {mesh, pgrm};
   this->with(rdr);
   auto & vec = this->instances[rdr];
   vec.push_back(inst);
}

void Renderer::render(Scene scene,
                      PgrmHandle pgrm,
                      glm::mat4 v_mat,
                      glm::mat4 p_mat)
{
   

   ////////////////////////////////
   for (auto & couple: scene.instances)
   {
      auto & mesh = couple.first.mesh;
      auto & pgrm = couple.first.pgrm;

      setup(pgrm);

      // get lights uniforms
      GLuint uDirLights = this->uniform_location(pgrm, "uDirLights");
      GLuint uPtsLights = this->uniform_location(pgrm, "uPtsLights");
      GLuint uDirLightsCount = this->uniform_location(pgrm, "uDirLightsCount");
      GLuint uPtsLightsCount = this->uniform_location(pgrm, "uPtsLightsCount");
      //   GLuint testloc = this->uniform_location(pgrm, "test");

      auto lights_dir = AnyLight::parse_lights_dir(scene.lights_dir, v_mat);
      auto lights_pts = AnyLight::parse_lights_pts(scene.lights_pts, v_mat);
      // GLfloat test[6] = {1., 1., 1., 1., 1., 1.};
      glUniformMatrix2x3fv(uDirLights,
                           glm::min((size_t) 10, scene.lights_dir.size()),
//                        10,
                           false,
                           lights_dir.data());
      glUniformMatrix2x3fv(uPtsLights,
                           glm::min((size_t) 10, scene.lights_pts.size()),
                           false,
                           lights_pts.data());
      
      glUniform1i(uDirLightsCount, scene.lights_dir.size());
      glUniform1i(uPtsLightsCount, scene.lights_pts.size());
      /*
      for (auto x: lights_pts)
      {
          std::cout << x << " ";
          
      }std::cout <<std::endl;
      */

      
      
      for (auto & inst: couple.second)
      {
          auto M = inst.model_mat;
          auto MV = v_mat * M;
          auto MVP = p_mat * MV;
         auto N = glm::transpose(glm::inverse(MV));

         glUniformMatrix4fv(_uM,
                            1, false,
                            glm::value_ptr(M));
         glUniformMatrix4fv(_uMVP,
                            1, false,
                            glm::value_ptr(MVP));
         glUniformMatrix4fv(_uMV,
                            1, false,
                            glm::value_ptr(MV));
         glUniformMatrix4fv(_uNormal,
                            1, false,
                            glm::value_ptr(N));

         mesh.draw();

      }

      finish(pgrm);


   } 
}


