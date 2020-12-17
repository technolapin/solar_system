#pragma once

#include <glimac/SDLWindowManager.hpp>
#include <glimac/Sphere.hpp>
#include <GL/glew.h>
#include <iostream>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <unordered_map>
#include <glimac/Image.hpp>
#include <memory> // shared ptrs
#include <assert.h>
#include <string>
#include <algorithm>
#include <string.h>
using namespace glimac;

struct CompleteVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 texCoords;
};


using Vertex = CompleteVertex;

const GLuint VERTEX_ATTR_POSITION = 0;
const GLuint VERTEX_ATTR_NORMAL = 1;
const GLuint VERTEX_ATTR_TANGENT = 2;
const GLuint VERTEX_ATTR_BITANGENT = 3;
const GLuint VERTEX_ATTR_UV = 4;



struct GLObject;
struct Scene;
struct Instance;
struct TextureHandler;
//enum GLType{};
//struct GLValue;
//struct Uniform;
//struct ProgramHolder;
struct Renderable;
struct RenderableBuilder;
struct AnyLight;
struct Renderer;
struct ProgramManager;
struct MonotextureProgramManager;
struct BitextureProgramManager;
struct PgrmHandle;
