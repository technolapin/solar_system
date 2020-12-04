#include <glimac/SDLWindowManager.hpp>
#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/glm.hpp>

using namespace glimac;


struct Vertex2DColor
{
   glm::vec2 position;
   glm::vec3 color;

   Vertex2DColor() = default;
   Vertex2DColor(glm::vec2 pos, glm::vec3 col)
   {
      position = pos;
      color = col;
   }
};


int main(int argc, char** argv) {
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
    Program program = loadProgram(applicationPath.dirPath() + "shaders/triangle.vs.glsl",
                                  applicationPath.dirPath() + "shaders/triangle.fs.glsl");
    program.use();

    



    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);


    int nb_sides = 40;
    float radius = 0.5;
    
    auto delta_angle = 2.*glm::pi<float>()/( (float) nb_sides);
    std::vector<Vertex2DColor> vertices;

    std::vector<uint32_t> indices;
    
    vertices.push_back(Vertex2DColor(glm::vec2(0., 0.), glm::vec3(1, 0, 1)));
    
    for (int i = 0; i < nb_sides; ++i)
    {
       auto angle1 = delta_angle*( (float) i);
       auto angle2 = delta_angle*( (float) (i+1));
       auto x1 = radius * glm::cos(angle1);
       auto y1 = radius * glm::sin(angle1);
       vertices.push_back(Vertex2DColor(glm::vec2(x1, y1), glm::vec3(0, 0, 1)));
       int indice_1 = i+1;
       int indice_2 = (i+1) % nb_sides +1;
       indices.push_back(0);
       indices.push_back(indice_1);
       indices.push_back(indice_2);
    }

    std::cout << "VBO " << vertices.size() << std::endl; 
    std::cout << "IBO " << indices.size() << std::endl; 
       
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2DColor) * indices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint ibo;
    glGenBuffers(1, &ibo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    
    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    
    const GLuint VERTEX_ATTR_POSITION = 3;
    const GLuint VERTEX_ATTR_COL = 8;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_COL);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(VERTEX_ATTR_POSITION,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex2DColor),
                          (const GLvoid*) offsetof(Vertex2DColor, position));
    
    glVertexAttribPointer(VERTEX_ATTR_COL,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex2DColor),
                          (const GLvoid*) offsetof(Vertex2DColor, color));
    
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

        glBindVertexArray(vao);

        glDrawElements(GL_TRIANGLES, nb_sides*3, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
           

        // Update the display
        windowManager.swapBuffers();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);

    return EXIT_SUCCESS;
}
