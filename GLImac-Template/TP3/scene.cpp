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
Scene::add(const GLObject & mesh, const PgrmHandle & pgrm, const Instance & inst)
{
   GLRenderable rdr = {mesh, pgrm};
   this->with(rdr);
   auto & vec = this->instances[rdr];
   vec.push_back(inst);
}


void Renderer::render(Scene scene,  PgrmHandle pgrm, glm::mat4 v_mat, glm::mat4 p_mat)
{

   ////////////////////////////////
   for (auto & couple: scene.instances)
   {
      auto & mesh = couple.first.mesh;
      auto & pgrm = couple.first.pgrm;

      setup(pgrm);

      for (auto & inst: couple.second)
      {
         auto MV = v_mat * inst.model_mat;
         auto MVP = p_mat * MV;
         auto N = glm::transpose(glm::inverse(MV));

         
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

/*
void
Scene::render(GLint uMVP, GLint uMV, GLint uNormal,
              glm::mat4 & v_mat, glm::mat4 &  p_mat) const
{
   for (auto & couple: this->instances)
   {
      auto & mesh = couple.first.mesh;
      auto & pgrm = couple.first.pgrm;
      
      for (auto & inst: couple.second)
      {
         auto MV = v_mat * inst.model_mat;
         auto MVP = p_mat * MV;
         auto N = glm::transpose(glm::inverse(MV));

         
         glUniformMatrix4fv(uMVP,
                            1, false,
                            glm::value_ptr(MVP));
         glUniformMatrix4fv(uMV,
                            1, false,
                            glm::value_ptr(MV));
         glUniformMatrix4fv(uNormal,
                            1, false,
                            glm::value_ptr(N));

         mesh.draw();

      }
   }
}
*/




