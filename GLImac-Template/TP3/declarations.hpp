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


const GLuint VERTEX_ATTR_POSITION = 0;
const GLuint VERTEX_ATTR_NORMAL = 1;
const GLuint VERTEX_ATTR_UV = 2;


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
