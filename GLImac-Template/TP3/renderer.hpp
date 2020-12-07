#pragma once 
//#include "scene.hpp"
#include "declarations.hpp"


struct ProgramManager
{
protected:
   Program _pgrm;
public:
   virtual void setup() = 0;
   virtual void finish() = 0;
   ProgramManager(std::string path_vs, std::string path_fs)
   {
      _pgrm = loadProgram(path_vs, path_fs);
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
      
      // we associate the texture n° 0 to the uniform      
      glUniform1i(_texture, 0);
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
      glUniform1i(_texture1, 0);
      glUniform1i(_texture2, 1);
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
   GLuint _uNormal;


public:
   Renderer() = default;

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

   void setup(PgrmHandle index)
   {
      _uMVP = glGetUniformLocation(_pgrms[index.id]->getGLId(), "uMVPMatrix");
      _uMV = glGetUniformLocation(_pgrms[index.id]->getGLId(), "uMVMatrix");
      _uNormal = glGetUniformLocation(_pgrms[index.id]->getGLId(), "uNormalMatrix");

      _pgrms[index.id]->setup();
   }
   void finish(PgrmHandle index)
   {
      _pgrms[index.id]->finish();
   }
   
   void render(Scene scene, PgrmHandle pgrm, glm::mat4 V, glm::mat4 P);
     
};





