#pragma once

#include "declarations.hpp"
#include "renderer.hpp"
struct GLRenderable
{
   GLObject mesh;
   PgrmHandle pgrm;

   bool operator==(const GLRenderable &other) const
   {
      return mesh == other.mesh && pgrm == other.pgrm;
   }

};
struct hash_fn
{
   std::size_t operator() (const GLRenderable &rdr) const
   {
      std::size_t h = (size_t) &rdr;
      return h;
   }
};


struct Scene
{
public:
   std::unordered_map<GLRenderable, std::vector<Instance>, hash_fn> instances;

   void
   print() const;
   
   Scene() = default;
   
   void with(const GLRenderable & rdr);

   void
   add(const GLObject & mesh, const PgrmHandle & rend, const Instance & inst);
/*
   void
   render(GLint uMVP, GLint uMV, GLint uNormal,
          glm::mat4 & v_mat, glm::mat4 &  p_mat) const;
*/
};

