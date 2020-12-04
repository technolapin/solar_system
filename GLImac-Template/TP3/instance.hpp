#pragma once

#include "declarations.hpp"



struct Instance
{
   glm::mat4 model_mat;
//   GLuint texture;
};

std::ostream & operator << (std::ostream &out, const Instance & inst)
{
   out << "Instance{" << (size_t)  &inst << "}";
   return out;
}


