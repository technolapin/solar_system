#pragma once 
#include "declarations.hpp"


struct ProgramManager
{
protected:
   Program _pgrm;
public:
    GLenum draw_mode = GL_TRIANGLES;

    virtual void setup() = 0;
   virtual void finish() = 0;
    ProgramManager(std::string path_vs, std::string path_fs, GLenum mode = GL_TRIANGLES):
        _pgrm(loadProgram(path_vs, path_fs)),
        draw_mode(mode)
   {
   }

    GLuint
   getGLId()
   {
      return _pgrm.getGLId();
   }
};



struct MonotextureProgramManager: public ProgramManager
{
private:
   GLuint _texture;
public:
   MonotextureProgramManager(std::string path_vs,
                             std::string path_fs,
                             GLuint tex):
      ProgramManager(path_vs, path_fs)
   {
      _texture = tex;
   }
   
   void setup()
   {
      _pgrm.use();

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, _texture);
      auto uTexture = glGetUniformLocation(_pgrm.getGLId(), "uTexture"); 
      
      // we associate the texture n° 0 to the uniform      
      glUniform1i(uTexture, 0);
   }
   
   void finish()
   {
      // debind the used textures
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, 0);
   }
};

struct BitextureProgramManager: public ProgramManager
{
private:
   GLuint _texture1;
   GLuint _texture2;
public:
   BitextureProgramManager(std::string path_vs,
                           std::string path_fs,
                           GLuint tex1,
                           GLuint tex2):
      ProgramManager(path_vs, path_fs)
   {
      _texture1 = tex1;
      _texture2 = tex2;
   }
   
   void setup()
   {
      _pgrm.use();

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, _texture1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, _texture2);
      
      // we associate the texture n° 0 to the uniform      
      //glUniform1i(_texture1, 0);
      //glUniform1i(_texture2, 1);

      auto uTexture1 = glGetUniformLocation(_pgrm.getGLId(), "uTexture1"); 
      auto uTexture2 = glGetUniformLocation(_pgrm.getGLId(), "uTexture2");
     
      
      glUniform1i(uTexture1, 0);
      glUniform1i(uTexture2, 1);
   }
   
   void finish()
   {
      // debind the used textures
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, 0);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, 0);
   }
};


struct CurveProgramManager: public ProgramManager
{
private:
    float _thickness;
public:
    CurveProgramManager(std::string path_vs,
                        std::string path_fs,
                        float thickness):
        ProgramManager(path_vs, path_fs, GL_LINE_LOOP)
   {
       _thickness = thickness;
   }
   
   void setup()
   {
      _pgrm.use();
      
   }
   
   void finish()
   {
   }
};












struct PgrmHandle
{
   long unsigned int id;
   bool operator==(const PgrmHandle &other) const
   {
      return id == other.id;
  }

};


struct Renderer
{
private:
    std::vector<ProgramManager *> _pgrms;
    GLuint _uMVP;
    GLuint _uMV;
    GLuint _uM;
    GLuint _uNormal;    

public:
   Renderer() = default;
    ~Renderer()
    {
        for (auto ptr: _pgrms)
        {
            free(ptr);
        }
    }

   /*
   PgrmHandle
   add(ProgramManager * pgrm)
   {
      _pgrms.push_back(pgrm);
      return {_pgrms.size() - 1};
   }
   */
   PgrmHandle
   add_monotex(std::string path_vs,
               std::string path_fs,
               GLuint tex)
   {
      _pgrms.push_back(new MonotextureProgramManager(path_vs, path_fs, tex));
      return {_pgrms.size() -1};
   }
   PgrmHandle
   add_bitex(std::string path_vs,
             std::string path_fs,
             GLuint tex1,
             GLuint tex2)
   {
      _pgrms.push_back(new BitextureProgramManager(path_vs, path_fs, tex1, tex2));
      return {_pgrms.size() -1};
   }

   PgrmHandle
   add_curve(std::string path_vs,
             std::string path_fs,
             GLfloat thickness)
   {
      _pgrms.push_back(new CurveProgramManager(path_vs, path_fs, thickness));
      return {_pgrms.size() -1};
   }

    
    GLenum setup(PgrmHandle index)
   {
      _uMVP = glGetUniformLocation(_pgrms[index.id]->getGLId(), "uMVPMatrix");
      _uMV = glGetUniformLocation(_pgrms[index.id]->getGLId(), "uMVMatrix");
      _uM = glGetUniformLocation(_pgrms[index.id]->getGLId(), "uMMatrix");
      _uNormal = glGetUniformLocation(_pgrms[index.id]->getGLId(), "uNormalMatrix");

      _pgrms[index.id]->setup();
      return _pgrms[index.id]->draw_mode;
   }
   void finish(PgrmHandle index)
   {
      _pgrms[index.id]->finish();
   }
   
   void render(Scene scene, PgrmHandle pgrm, glm::mat4 V, glm::mat4 P);
   
   GLuint uniform_location(PgrmHandle index, const char * name)
   {
      return glGetUniformLocation(_pgrms[index.id]->getGLId(), name);
   }
};








