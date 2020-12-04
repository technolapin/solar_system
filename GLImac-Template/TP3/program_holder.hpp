#pragma once

#include "glvars.hpp"

struct ProgramHolder
{
//private:
public:
   Program _pgrm;
   std::vector<Uniform> uniforms;

   ProgramHolder(FilePath path_vs, FilePath path_fs);

   void
   add_uniform(GLType gl_type, std::string name);

   // does not work for now (move problem because one implicit constructor is missing)
   ProgramHolder &
   with_uniform(GLType gl_type, std::string name);

   void use() const;

   ProgramHolder(const ProgramHolder& other)
   {
      // necessary because the copy constructor of glimac::Program is private
      memcpy(&_pgrm, &other._pgrm, sizeof(glimac::Program));
      uniforms = other.uniforms;
   }
};
