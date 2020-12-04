#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>

#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>


using namespace glimac;

int main(int argc, char** argv) {
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
    Shader vs = loadShader(GL_VERTEX_SHADER, path_vs);
    Shader fs = loadShader(GL_FRAGMENT_SHADER, path_fs);

    if(!vs.compile()) {
       throw std::runtime_error("Compilation error for vertex shader (from file " + std::string(path_vs) + "): " + vs.getInfoLog());
    }

    if(!fs.compile()) {
       throw std::runtime_error("Compilation error for fragment shader (from file " + std::string(path_fs) + "): " + fs.getInfoLog());
    }
    Program program;
    program.attachShader(vs);
    program.attachShader(fs);

    if (argc > 3)
    {
       auto path_gs = applicationPath.dirPath() + "shaders" + argv[3];
       Shader gs = loadShader(GL_GEOMETRY_SHADER, path_gs);
       if(!gs.compile()) {
          throw std::runtime_error(
             "Compilation error for geometric shader (from file "
             + std::string(path_gs)
             + "): " + gs.getInfoLog());
       }

       program.attachShader(gs);

       if(!program.link()) {
          throw std::runtime_error("Link error (for files " + path_vs.str() + " and " + path_fs.str() + " and "+ path_gs.str()+"): " + program.getInfoLog());
       }
    }
    else
    {
       if(!program.link()) {
          throw std::runtime_error("Link error (for files " + path_vs.str() + " and " + path_fs.str() + "): " + program.getInfoLog());
       }

    }

                                  
    program.use();

    

    


    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);


    GLfloat vertices[] = {
       -0.5f, -0.5f, 1.f, 0.f, 0.f, // premier sommet
       0.5f, -0.5f, 0.f, 1.f, 0.f, // deuxième sommet
       0.0f, 0.5f, 0.f, 0.f, 1.f // troisième sommet
    };
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3*5, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_COL = 1;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_COL);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), 0);
    glVertexAttribPointer(VERTEX_ATTR_COL, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (const GLvoid*) (2*sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(vao);


    
// Application loop:
    bool done = false;
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

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);


           

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    return EXIT_SUCCESS;
}
