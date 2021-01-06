
// the order is important
#include "globject.hpp"
#include "instance.hpp"
#include "textures_handler.hpp"

#include "lights.hpp"

#include "renderer.hpp"
#include "scene.cpp"


#include "cameras/trackball_camera.hpp"
#include "cameras/free_fly.hpp"

#include <optional>

#include <limits>
#include <math.h>



// Ray Casting functions I was working on
glm::vec3
cast_ray(glm::mat4 V, glm::mat4 P, glm::vec2 pointer_pos)
{

    auto ray_clip = glm::vec4(pointer_pos, -1.0, 1.0);

    auto ray_eye = glm::inverse(P) * ray_clip;
    ray_eye = glm::vec4(glm::vec2(ray_eye), -1.0, 0.0);

    auto ray_world = glm::normalize(glm::vec3(glm::inverse(V) * ray_eye));

    return ray_world;
    
}

bool
intersect_ray(glm::vec3 ray, glm::vec3 sphere_center, float sphere_radius)
{
   auto u = -glm::normalize(sphere_center);
    auto ortho = u - glm::dot(u, ray)*ray;
    auto dist = glm::l2Norm(ortho);
    // std::cout << "dist  " << dist << "    z =" << sphere_center[2] << std::endl;
    return dist <= sphere_radius && sphere_center[2] > 0.;
}


#include "logic.cpp"






GLObject
make_sphere(GLfloat radius,
            GLsizei discLat,
            GLsizei discLong)
{
    std::vector<ShapeVertex> sphere;

    auto sph_obj = Sphere(radius, discLat, discLong);
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


enum CameraType
{
    FreeFly,
    Trackball
};

// manage the different type of cameras
// this is what we use in the end
struct CameraManager
{
    CameraType mode = CameraType::Trackball;

    TrackballCamera track_cam;;
    FreeflyCamera fly_cam;
    
    float     track_friction = 0.99;
    float     track_sensibility = 10000.0;
    glm::vec2 track_rotation_speed = glm::vec2(0);
    bool      track_was_mouse_pressed = false;

    float fly_speed = 1.0;

    
    float mouse_amplitude = 1.0f;
    float mouse_speed = 1.0f;
    glm::ivec2 mouse_pos = glm::vec2(0);
    glm::vec2 mouse_angle = glm::vec2(0.0f);

    // stuff
    float speed = 0.2;
    float speed_max = 30.0;

    float mode_switch_delay = 10;
    float mode_switch_cooldown = 0;

    void
    tick(SDLWindowManager & windowManager)
    {
        if (mode_switch_cooldown) mode_switch_cooldown--;
        
        if (!mode_switch_cooldown && windowManager.isKeyPressed(SDLK_SPACE))
        {
            if (mode == CameraType::Trackball)
                mode = CameraType::FreeFly;
            else
                mode = CameraType::Trackball;
            mode_switch_cooldown = mode_switch_delay;
        }
        
        /// mouse motion
        auto new_mouse_pos = windowManager.getMousePosition();
        auto new_mouse_angle = glm::vec2(glm::atan(new_mouse_pos[0]/mouse_amplitude),
                                         glm::atan(new_mouse_pos[1]/mouse_amplitude));
        auto mouse_angle_delta = new_mouse_angle - mouse_angle;
        auto mouse_delta = new_mouse_pos - mouse_pos;
        // out of the window, the mouse's position is (0,0)
        if ((new_mouse_pos[0] == 0.0f && new_mouse_pos[1] == 0.0f)
            || (mouse_pos[0] == 0.0f && mouse_pos[0] == 0.0f))
        {
            mouse_delta = glm::vec2(0.0f);
        }
        mouse_pos = new_mouse_pos;
        mouse_angle = new_mouse_angle;

        
        switch (mode)
        {
            case CameraType::Trackball:
            //////////// Trackball track_cam
            {
                if (windowManager.isMouseButtonPressed(SDL_BUTTON_LEFT))
                {
                    track_rotation_speed *= track_friction/2.0;
                    auto angles = track_sensibility*mouse_angle_delta;
                    track_rotation_speed[0] += angles[0]*track_cam.rotation_sign();
                    track_rotation_speed[1] += angles[1];
                    track_rotation_speed[0] = glm::min<float>(speed_max, glm::max<float>(-speed_max, track_rotation_speed[0]));
                    track_rotation_speed[1] = glm::min<float>(speed_max, glm::max<float>(-speed_max, track_rotation_speed[1]));
           
                    track_cam.rotateLeft(angles[0]);
                    track_cam.rotateUp(angles[1]);
                    track_was_mouse_pressed = true;
                }
                else
                {
                    track_cam.rotateLeft(track_rotation_speed[0]*track_cam.rotation_sign());
                    track_cam.rotateUp(track_rotation_speed[1]);
                    track_was_mouse_pressed = false;
                }
                track_rotation_speed *= track_friction;
                if (windowManager.isKeyPressed(SDLK_z))
                {
                    track_cam.moveFront(speed);
                }
                else if (windowManager.isKeyPressed(SDLK_s))
                {
                    track_cam.moveFront(-speed);
                }
            }
            break;

            default:
            /////////// freefly camera
            {
                if (windowManager.isKeyPressed(SDLK_z))
                {
                    fly_cam.moveFront(fly_speed);
                }
                else if (windowManager.isKeyPressed(SDLK_s))
                {
                    fly_cam.moveFront(-fly_speed);
                }
                else if (windowManager.isKeyPressed(SDLK_d))
                {
                    fly_cam.moveLeft(-fly_speed);
                }
                else if (windowManager.isKeyPressed(SDLK_q))
                {
                    fly_cam.moveLeft(fly_speed);
                }

                fly_cam.rotateLeft(mouse_delta[0]*mouse_speed);
                fly_cam.rotateUp(mouse_delta[1]*mouse_speed);
            }
        }
    }

    
    glm::mat4
    view_matrix()
    {
        switch(mode)
        {
            case CameraType::Trackball:
                return track_cam.getViewMatrix();
            default:
                return fly_cam.getViewMatrix();
        }
    }

    
};



int main(int argc, char** argv)
{
        
    int width = 1000;
    int height = 1000;
    // Initialize SDL and open a window
    SDLWindowManager windowManager(width, height, "GLImac");
    glm::vec2 dims((float) width, (float) height);
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
    auto path_fs_mono = applicationPath.dirPath() + "shaders" + "phong1tex.fs.glsl";
    auto path_fs_duo = applicationPath.dirPath() + "shaders" + "phong2tex.fs.glsl";

    auto path_vs_curve = applicationPath.dirPath() + "shaders" + "curve.vs.glsl";
    auto path_fs_curve = applicationPath.dirPath() + "shaders" + "curve.fs.glsl";

    
//    auto program = loadProgram(path_vs, path_fs_mono);    
    //  program.use();
    
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
    auto curve_renderer = renderer.add_curve(path_vs_curve,
                                             path_fs_curve,
                                             1.0f);
    

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


        Logic::Ellipse el1(glm::vec3(1, 0, 0),
                           glm::vec3(0, 1, 0),
                           8.0f,
                           10.0f);
        Logic::Ellipse el2(glm::vec3(1, 0, 0),
                           glm::vec3(0, 1, 0),
                           3.0f,
                           4.0f);
        Logic::Ellipse el3(glm::vec3(1, 0, 0),
                           glm::vec3(0, 0, 1),
                           1.0f,
                           1.0f);
        Logic::Ellipse el4(glm::vec3(1, 0, 0),
                           glm::vec3(0, 1, 0),
                           0.3f,
                           0.2f);

        auto ob0 = Logic::Object(meshes[0], earth_renderer).with_geometry(1.0f);
        auto obmoon = Logic::Object(meshes[0], moon_renderer);
        auto ob1 = obmoon
            .with_geometry(1.0f/4.0f);
        auto ob2 = obmoon.with_geometry(1.0f/16.0f);
        auto ob3 = obmoon.with_geometry(1.0f/64.0f);


        Logic::Tree<Logic::Object, Logic::Ellipse> tree(ob0);

        tree.add_child(Logic::Tree<Logic::Object, Logic::Ellipse>(ob1), el1);
        tree.add_child(Logic::Tree<Logic::Object, Logic::Ellipse>(ob1), el2)
            .add_child(Logic::Tree<Logic::Object, Logic::Ellipse>(ob2), el3)
            .add_child(Logic::Tree<Logic::Object, Logic::Ellipse>(ob3), el4);

        
        Logic::World world(tree, curve_renderer);

        world.lights.push_back({LightType::Directional, {glm::vec3(1), glm::vec3(0.7)}});
    



    bool running = true;

    CameraManager cam;
    
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

        if (windowManager.isKeyPressed(SDLK_RETURN))
        {
           running =  !running;
        }

        cam.tick(windowManager);

        // *********************************
        // * HERE SHOULD COME THE RENDERING CODE
        // *********************************


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        if (running) world.tick(0.03f);

  
        auto test_scene =  world.make_scene();
        auto V = cam.view_matrix();
        renderer.render(test_scene, earth_renderer, V, P);
        auto mouse_pos = windowManager.getMousePosition();
        glm::vec2 cursor_pos = glm::vec2(((float) (mouse_pos[0]-width/2))/ ((float) width),
                                         ((float) (mouse_pos[1]-height/2))/ ((float) height));
        //std::cout << "mouse pos: " << mouse_pos;
        //std::cout << "   cursor pos: " << cursor_pos << std::endl;
        world.select(V, P,  cursor_pos);
        // Update the display
        windowManager.swapBuffers();
    }

    
    for (auto & obj: meshes)
    {
       obj.free_gpu();
    }
    
       
    return EXIT_SUCCESS;
}

