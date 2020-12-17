
// the order is important
#include "globject.hpp"
#include "instance.hpp"
#include "textures_handler.hpp"

#include "lights.hpp"

#include "renderer.hpp"
#include "scene.cpp"


#include "cameras/trackball_camera.hpp"
#include "cameras/free_fly.hpp"


#include "primitives.cpp"


GLObject
make_sphere(GLfloat radius,
            GLsizei discLat,
            GLsizei discLong)
{
    std::vector<Vertex> sphere;

    auto sph_obj = Primitives::Sphere(radius, discLat, discLong);
    auto n = sph_obj.getVertexCount();
    auto ptr = sph_obj.getDataPointer();
    std::cout << "POLO 1.5!" << std::endl;
    for (auto i = 0; i < n; ++i)
    {
       sphere.push_back(ptr[i]);
    }
    std::cout << "POLO 2!" << std::endl;
    return GLObject(sphere);
}



int main(int argc, char** argv) {
    // Initialize SDL and open a window
    SDLWindowManager windowManager(1000, 1000, "GLImac");

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
    auto path_vs = applicationPath.dirPath() + "shaders" + "3D.vs.glsl";
    auto path_fs_mono = applicationPath.dirPath() + "shaders" + "phong1texfullmaps.fs.glsl";
    auto path_fs_duo = applicationPath.dirPath() + "shaders" + "phong2tex.fs.glsl";
//    auto path_fs_duo = applicationPath.dirPath() + "shaders" + "bitexture.fs.glsl";
    auto program = loadProgram(path_vs, path_fs_mono);    
    program.use();
    
    TextureHandler textures(
       applicationPath.dirPath()
       + ".."
       + ".."
       + "GLImac-Template"
       + "assets"
       + "textures"
       );
    
    auto earth_tex = textures.load("EarthMap.jpg");
    auto cloud_tex = textures.load("CloudMap.jpg");
    auto moon_tex = textures.load("MoonMap.jpg"); 
    auto teto_tex = textures.load("tetoaria.png");
    Renderer renderer;

    auto earth_renderer = renderer.add_bitex(path_vs,
                                             path_fs_duo,
                                             earth_tex,
                                             cloud_tex);
    auto moon_renderer = renderer.add_monotex(path_vs,
                                              path_fs_mono,
                                              moon_tex);
    

    auto M = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -5.0f));
    auto V = glm::mat4(1.0f);
    auto P = glm::perspective(
       glm::radians(70.f),
       1.0f,
       0.1f,
       100.0f
       );


       
    std::vector<GLObject> meshes;
    meshes.push_back(make_sphere(1.0, 30, 30));
    
    

    glEnable(GL_DEPTH_TEST);
    
    for (auto & obj: meshes)
    {
       std::cout << obj << std::endl;
    }



    std::vector<glm::vec3> axes;
    std::vector<glm::vec3> pos_init;

    for(int i = 0; i < 32; ++i)
    {
       auto axe = glm::sphericalRand(1.0f);
       axes.push_back(axe);
       auto p = glm::sphericalRand(1.0f);
       auto p_ortho = glm::normalize(p - glm::dot(p, axe)*axe);
       
       pos_init.push_back(p_ortho);
    }

    // stuff
    float speed = 0.2;
    float speed_max = 30.0;
    
    // mouse stuff
    float mouse_amplitude = 1.0f;
    float mouse_speed = 1.0f;
    auto mouse_pos = windowManager.getMousePosition();
    auto mouse_angle = glm::vec2(glm::atan(mouse_pos[0]/mouse_amplitude),
                                 glm::atan(mouse_pos[1]/mouse_amplitude));
    
    // Trackball camera
    
    TrackballCamera camera;
    camera.set_center(glm::vec3(0.0f, 0.0f, -5.0f));
    camera.moveFront(5.0);

    float friction = 0.99;
    float sensibility = 10000.0;
    auto rotation_speed = glm::vec2(0.0f, 0.0f);
    bool was_mouse_pressed = false;
    

    //////// Freefly camera
    /*
    float speed = 1.0;
    
    FreeflyCamera camera;
    */
    
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

        if (windowManager.isKeyPressed(SDLK_ESCAPE))
        {
           done = true;
        }
        
        /// events and such
        auto new_mouse_pos = windowManager.getMousePosition();
        auto new_mouse_angle = glm::vec2(glm::atan(new_mouse_pos[0]/mouse_amplitude),
                                         glm::atan(new_mouse_pos[1]/mouse_amplitude));
        auto mouse_angle_delta = new_mouse_angle - mouse_angle;
        auto mouse_delta = new_mouse_pos - mouse_pos;
        if ((new_mouse_pos[0] == 0.0f && new_mouse_pos[1] == 0.0f)
            || (mouse_pos[0] == 0.0f && mouse_pos[0] == 0.0f))
        {
           mouse_delta = glm::vec2(0.0f);
        }
        mouse_pos = new_mouse_pos;
        mouse_angle = new_mouse_angle;

        //////////// Trackball camera
        if (windowManager.isMouseButtonPressed(SDL_BUTTON_LEFT))
        {
           rotation_speed *= friction/2.0;
           auto angles = sensibility*mouse_angle_delta;
           rotation_speed[0] += angles[0]*camera.rotation_sign();
           rotation_speed[1] += angles[1];
           rotation_speed[0] = glm::min<float>(speed_max, glm::max<float>(-speed_max, rotation_speed[0]));
           rotation_speed[1] = glm::min<float>(speed_max, glm::max<float>(-speed_max, rotation_speed[1]));
           
           camera.rotateLeft(angles[0]);
           camera.rotateUp(angles[1]);
           was_mouse_pressed = true;
        }
        else
        {
           camera.rotateLeft(rotation_speed[0]*camera.rotation_sign());
           camera.rotateUp(rotation_speed[1]);
           was_mouse_pressed = false;
        }
        rotation_speed *= friction;
        if (windowManager.isKeyPressed(SDLK_z))
        {
           camera.moveFront(speed);
        }
        else if (windowManager.isKeyPressed(SDLK_s))
        {
           camera.moveFront(-speed);
        }

        /////////// freefly camera
        /*
        if (windowManager.isKeyPressed(SDLK_z))
        {
           camera.moveFront(speed);
        }
        else if (windowManager.isKeyPressed(SDLK_s))
        {
           camera.moveFront(-speed);
        }
        else if (windowManager.isKeyPressed(SDLK_d))
        {
           camera.moveLeft(-speed);
        }
        else if (windowManager.isKeyPressed(SDLK_q))
        {
           camera.moveLeft(speed);
        }

        camera.rotateLeft(mouse_delta[0]*mouse_speed);
        camera.rotateUp(mouse_delta[1]*mouse_speed);
        */
        // *********************************
        // * HERE SHOULD COME THE RENDERING CODE
        // *********************************


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        Scene scene;
        scene.lights_pts.push_back({
                glm::vec3(0., 0., -5.),
                glm::vec3(0.7, 0.6, 0.9)*1.0f
            });
        scene.lights_dir.push_back({ 
                glm::vec3(1.0, 1.0, 9.0),
                glm::vec3(0.9, 0.7, 0.7),
            });
        
        {
           auto center = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -5.0f));
    
           Instance planet_inst = {center};
           scene.add(meshes[0], earth_renderer, planet_inst);

           for (auto i = 0; i < axes.size(); ++i)
           {
              auto rot_vec = axes[i];
              auto pos0 = pos_init[i];
              pos0 = glm::vec3(2.0*pos0[0], 2.0*pos0[1], 2.0*pos0[2]);
              glm::mat4 moon_mat = glm::translate(glm::mat4(1), glm::vec3(0, 0, -5));
              moon_mat = glm::rotate(moon_mat, windowManager.getTime(), rot_vec);
              moon_mat = glm::translate(moon_mat, pos0);
              moon_mat = glm::scale(moon_mat, glm::vec3(0.2, 0.2, 0.2));
              Instance moon = {moon_mat};
    

              scene.add(meshes[0], moon_renderer, moon);
           }
        }
        
        renderer.render(scene, earth_renderer, camera.getViewMatrix(), P);
        
        // Update the display
        windowManager.swapBuffers();
    }

    for (auto & obj: meshes)
    {
       obj.free_gpu();
    }
    
       
    return EXIT_SUCCESS;
}

