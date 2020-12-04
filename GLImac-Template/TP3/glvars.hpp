#pragma once
enum GLType
{
   Mat4,
   Sampler2D
};

struct GLValue
{
   GLType type;
   union
   {
      GLuint val_int;
      GLfloat val_float;
      const GLuint * val_intp;
      const GLfloat * val_floatp;
   };


   GLValue(GLType t, glm::mat4 &val)
   {
      type = t;
      val_floatp = glm::value_ptr(val);
   }

   GLValue(GLType t, GLuint &val)
   {
      type = t;
      val_int = val;
   }
   
};

struct Uniform
{
   GLType type;
   std::string name;
   GLuint pointer;

   void
   setup(const GLValue & val, int * tex_indice) const
   {
      switch (type)
         {
            case GLType::Mat4:
               glUniformMatrix4fv(this->pointer,
                                  1, false,
                                  val.val_floatp);
               break;
               
            case GLType::Sampler2D:
               glUniform1i(this->pointer, val.val_int);

               glActiveTexture(GL_TEXTURE0+*tex_indice);
               glBindTexture(GL_TEXTURE_2D, val.val_int);
               
               (*tex_indice)++;
               
               break;
               
            default:
               std::cout << "GLType not implemented!\n";
         }
   }
};
