#pragma once

#include "declarations.hpp"


struct TextureHandler
{
private:
   std::vector<GLuint> _textures;
   FilePath _textures_folder_path;
public:
   TextureHandler(FilePath path)
   {
      _textures_folder_path = path;
   }

   ~TextureHandler()
   {
      for (auto & tex: _textures)
      {
         glDeleteTextures(1, &tex);
      }
   }

   GLuint
   load(std::string name)
   {
      auto image = loadImage(_textures_folder_path + name);
      GLuint tex;
      glGenTextures(1, &tex);
       
      glBindTexture(GL_TEXTURE_2D, tex);
      glTexImage2D(GL_TEXTURE_2D,
                   0,
                   GL_RGBA,
                   image->getWidth(),
                   image->getHeight(),
                   0,
                   GL_RGBA,
                   GL_FLOAT,
                   image->getPixels());
       
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glBindTexture(GL_TEXTURE_2D, 0);

      _textures.push_back(tex);
      return tex;
   }

};
