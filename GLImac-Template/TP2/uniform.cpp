#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/Image.hpp>
#include <vector>
#include <array>
#include <math.h>
using namespace glimac;

struct Vertex2DUV
{
   glm::vec2 pos;
   glm::vec2 uv;

   Vertex2DUV() = default;
   Vertex2DUV(glm::vec2 p_pos, glm::vec2 p_uv)
   {
      pos = p_pos;
      uv = p_uv;
   }
   
};




struct Mat3
{
   std::array<float, 9> data;

   Mat3()
   {
      data = {0.};
   }
   Mat3(float a, float b, float c, float d, float e, float f, float g, float h, float i)
   {
      data = {a, b, c, d, e, f, g, h, i};
   }
   Mat3(std::array<float, 9> p_data)
   {
      data = p_data;
   }
   Mat3(const Mat3 & mat)
   {
      std::copy(mat.data.begin(), mat.data.end(), data.begin());
   }

   float get(const int i, const int j) const
   {
      return this->data[i+j*3];
   }
   void set(int i, int j, float val)
   {
      this->data[i+j*3] = val;
   }


   Mat3 operator+(const Mat3 & other)
   {
      Mat3 out;
      for (int i = 0; i < 9; ++i)
      {
         out.data[i] = this->data[i] + other.data[i];
      }
      return out;
   }

   Mat3 operator*(const Mat3 & other) const
   {
      Mat3 out;
      for (int i = 0; i < 3; ++i)
      {
         for (int j = 0; j < 3; ++j)
         {
            float sum = 0.0f;
            for (int k = 0; k < 3; ++k)
            {
               sum += other.get(i, k)*this->get(k, j);
            }
            out.set(i, j, sum);
         }
      }
      return out;
   }

   glm::mat3 to_glm() const
   {
      // the order is different in glm matrixes
      glm::mat3 out(
         this->data[0], 
         this->data[3],
         this->data[6],

         this->data[1],
         this->data[4],
         this->data[7],

         this->data[2],
         this->data[5],
         this->data[8]
         );

      return out;
   }

   static Mat3 rotate(const float angle)
   {
      return Mat3(
         cos(angle) , sin(angle), 0.,
         -sin(angle), cos(angle), 0.,
         0.         ,         0., 1.
         );
   }
   static Mat3 translate(const float x, const float y)
   {
      return Mat3(
         1., 0., x ,
         0., 1., y ,
         0., 0., 1.
         );
   }
   static Mat3 scale(const float x, const float y)
   {
      return Mat3(
         x , 0., 0.,
         0., y , 0.,
         0., 0., 1.
         );
   }

   
};


glm::mat3 rotate(const float angle)
{
   return Mat3::rotate(angle).to_glm();
}


std::ostream & operator << (std::ostream &out, Mat3 & mat)
{
   for (int j = 0; j < 3; ++j)
   {
      for (int i = 0; i < 3; ++i)
      {
         out << std::setprecision(1) << std::setw(7);
         out << mat.get(i, j);
      }
      out << std::endl;
   }
   return out;
}


int main(int argc, char** argv) {

   Mat3 A;
   A.set(0, 0, 1.);
   A.set(1, 1, 1.);
   A.set(2, 2, 1.);
   
   auto B = A+A;

   auto C = Mat3(
            1., 1., 1.,
            0., 0., 0.,
            0., 0., 0.
         );
   auto D = Mat3(
            1., 0., 0.,
            1., 0., 0.,
            1., 0., 0.
         );
   auto E = C*D;
   auto F = rotate(3.14/2.);
   std::cout << A << std::endl;
   std::cout << B << std::endl;
   std::cout << E << std::endl;
   std::cout << F << std::endl;
   
   if (argc < 3)
   {
      std::cout << "Need at least 2 arguments!" << std::endl;
      return 1;
   }
    // Initialize SDL and open a window
    SDLWindowManager windowManager(100, 100, "GLImac");

    // Initialize glew for OpenGL3+ support
    GLenum glewInitError = glewInit();
    if(GLEW_OK != glewInitError) {
        std::cerr << glewGetErrorString(glewInitError) << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "OpenGL Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLEW Version : " << glewGetString(GLEW_VERSION) << std::endl;

    /*********************************
     * HERE SHOULD COME THE INITIALIZATION CODE
     *********************************/
    FilePath applicationPath(argv[0]);
    auto path_vs = applicationPath.dirPath() + "shaders" + argv[1];
    auto path_fs = applicationPath.dirPath() + "shaders" + argv[2];

    auto program = loadProgram(path_vs, path_fs);

    program.use();

    GLuint uModelMatrix = glGetUniformLocation(program.getGLId(), "uModelMatrix");
    GLuint uColor = glGetUniformLocation(program.getGLId(), "uColor");
    GLuint uTexture = glGetUniformLocation(program.getGLId(), "uTexture");

    std::cout << "uModelMatrix location: " << uModelMatrix << std::endl;
    std::cout << "uColor location: " << uColor << std::endl;


    auto path_triforce = applicationPath.dirPath() + ".."+".." +"GLImac-Template"+ "assets" + "textures"+"triforce.png";

    std::cout << path_triforce << std::endl;
    auto triforce_image = loadImage(path_triforce);
    std::cout << "image is nullptr: " << (triforce_image == nullptr) << std::endl;

    GLuint triforce;
    glGenTextures(1, &triforce);

    glBindTexture(GL_TEXTURE_2D, triforce);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 triforce_image->getWidth(),
                 triforce_image->getHeight(),
                 0,
                 GL_RGBA,
                 GL_FLOAT,
                 triforce_image->getPixels()
       );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
 
       
    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    

    std::vector<Vertex2DUV> vertices;

    vertices.push_back(Vertex2DUV(glm::vec2(-1., -1), glm::vec2(0., 1.)));
    vertices.push_back(Vertex2DUV(glm::vec2( 1., -1), glm::vec2(0.5, 0.)));
    vertices.push_back(Vertex2DUV(glm::vec2( 0.,  1), glm::vec2(1., 1.)));

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(Vertex2DUV) * 3,
                 vertices.data(),
                 GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_UV = 1;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_UV);

    glVertexAttribPointer(
       VERTEX_ATTR_POSITION,
       2,
       GL_FLOAT,
       GL_FALSE,
       sizeof(Vertex2DUV),
       (const GLvoid*) offsetof(Vertex2DUV, pos)
       );

    glVertexAttribPointer(
       VERTEX_ATTR_UV,
       2,
       GL_FLOAT,
       GL_FALSE,
       sizeof(Vertex2DUV),
       (const GLvoid*) offsetof(Vertex2DUV, uv)
       );

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindVertexArray(0);




    
    // Application loop:
    bool done = false;

    float angle = 0.0f;

    
    while(!done) {
        // Event loop:
        SDL_Event e;
        while(windowManager.pollEvent(e)) {
            if(e.type == SDL_QUIT) {
                done = true; // Leave the loop after this iteration
            }
        }
        /*********************************
         * HERE SHOULD COME THE RENDERING CODE
         *********************************/
        //glUniform1f(uTime, angle);
        angle += 0.01;
        auto mat = Mat3::rotate(angle)*Mat3::scale(0.2,0.5);

        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);


        glUniform3fv(uColor,
                     1,
                     glm::value_ptr(glm::vec3(1., 0., 1.)));

        
        std::vector<Mat3> instances;
        instances.push_back(Mat3::rotate(angle)*Mat3::translate(-0.5, -0.5)*mat);
        instances.push_back(Mat3::rotate(angle)*Mat3::translate(0.5, -0.5)*mat);
        instances.push_back(Mat3::rotate(angle)*Mat3::translate(-0.5, 0.5)*mat);
        instances.push_back(Mat3::rotate(angle)*Mat3::translate(0.5, 0.5)*mat);


        glBindTexture(GL_TEXTURE_2D, triforce);
        glUniform1i(uTexture, 0);

        
        for (auto & mat: instances)
        {
           glUniformMatrix3fv(uModelMatrix,
                              1, false,
                              glm::value_ptr(mat.to_glm()));
           glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(0);

        // Update the display
        windowManager.swapBuffers();
    }
    glDeleteBuffers(1, &vbo);
    //glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &triforce);

    return EXIT_SUCCESS;
}

