
#include "declarations.hpp"
#include "program_holder.hpp"
#include "renderable.hpp"

ProgramHolder::ProgramHolder(FilePath path_vs, FilePath path_fs)
{
   _pgrm = loadProgram(path_vs, path_fs);
}

void
ProgramHolder::add_uniform(GLType gl_type, std::string name)
{
   GLuint unif_ptr = glGetUniformLocation(_pgrm.getGLId(),
                                          name.c_str());
   uniforms.push_back(
      {
         gl_type,
         name,
         unif_ptr
      });
}
   
ProgramHolder &
ProgramHolder::with_uniform(GLType gl_type, std::string name)
{
   add_uniform(gl_type, name);
   return *this;
}   


void
ProgramHolder::use() const
{
   _pgrm.use();
}
