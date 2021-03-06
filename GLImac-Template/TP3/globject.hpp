#pragma once

#include "declarations.hpp"

struct GLObject
{
private:
   GLuint _vao;
   GLuint _vbo;
   GLuint _size;
public:
    GLObject()
    {
    }
    
   GLObject(std::vector<ShapeVertex> & obj)
   {
      _size = obj.size();
      
      glGenBuffers(1, &_vbo);
      glGenVertexArrays(1, &_vao);

      glBindBuffer(GL_ARRAY_BUFFER, _vbo);
      glBindVertexArray(_vao);

      
      glBufferData(GL_ARRAY_BUFFER,
                   sizeof(ShapeVertex) * obj.size(),
                   obj.data(),
                   GL_STATIC_DRAW);

      glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
      glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
      glEnableVertexAttribArray(VERTEX_ATTR_UV);

      glVertexAttribPointer
         (
            VERTEX_ATTR_POSITION,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(ShapeVertex),
            (const GLvoid*) offsetof(ShapeVertex, position)
            );
      glVertexAttribPointer
         (
            VERTEX_ATTR_NORMAL,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(ShapeVertex),
            (const GLvoid*) offsetof(ShapeVertex, normal)
            );
      glVertexAttribPointer
         (
            VERTEX_ATTR_UV,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(ShapeVertex),
            (const GLvoid*) offsetof(ShapeVertex, texCoords)
            );
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
   }

   void free_gpu()
   {
      glDeleteVertexArrays(1, &_vao);
      glDeleteBuffers(1, &_vbo);
   }

   

   void
   draw(GLenum mode) const
   {
      glBindVertexArray(_vao);
      glDrawArrays(mode, 0, _size);
      glBindVertexArray(0);
   }

   GLuint
   vbo() const
   {
      return _vbo;
   }
   GLuint
   vao() const
   {
      return _vao;
   }
   GLuint
   size() const
   {
      return _size;
   }
   bool operator==(const GLObject &other) const
   {
      return this->vbo() == other.vbo()
         && this->vao() == other.vao()
         && this->size() == other.size();
   }
};


std::ostream & operator << (std::ostream &out, const GLObject & obj)
{
   out << "Object [vbo:" << obj.vbo()
       << ", vao:" << obj.vao()
       << ", size:" << obj.size() << "]";
   return out;
}
