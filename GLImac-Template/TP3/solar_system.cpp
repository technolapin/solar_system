
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
    auto ortho = sphere_center - glm::dot(sphere_center, ray)*ray;
    auto dist = glm::l2Norm(ortho);
    // std::cout << "dist  " << dist << "    z =" << sphere_center[2] << std::endl;
    return dist <= sphere_radius && sphere_center[2] > 0.;
}






namespace Logic
{
    template<class NodeType, class EdgeType>
    struct Tree
    {
    private:
        // we can avoid to work with pointers since the vector is already a pointer
 
    public: 
        NodeType _node_val;
        std::vector<std::pair<EdgeType, Tree>> children;


        struct TreeIterator
        {
        private:
//            Tree<NodeType, EdgeType> * _treeptr;
            // vector elements are trees and the index of their selected child
            std::vector<std::pair<Tree<NodeType, EdgeType> *, int >> branch;

        public:

            TreeIterator(Tree<NodeType, EdgeType> * treeptr)
            {
                branch = {std::make_pair(treeptr, 0)};
            }

            TreeIterator()
            {
                branch = {};
            }

            


            void
            next()
            {
                if (branch.size() == 0)
                {
                    return;
                }
            
                auto & head = *branch.back().first;
                auto child_index = branch.back().second;
            
                if (!head.is_leaf() && child_index < head.children_count())
                {
                    branch.push_back(std::make_pair(&head[child_index], 0));
                }
                else
                {
                    branch.pop_back();
                    if (branch.size() == 0) return;
                    branch.back().second++;
                    next();
                }
            }
            
            TreeIterator &
            operator++()
            {
                next();
                return *this;
            }
            Tree &
            operator*()
            {
                return *std::get<Tree*>(branch.back());
            }

            
            // sufficient for use in for loops
            bool
            operator!=(const TreeIterator & other) const
            {
                return other.branch.size() != branch.size();
            }
        
        
        }; 

        
        Tree(NodeType val):
            _node_val(val)
        {
//            _node_val = val;
        }
        
        NodeType &
        value()
        {
            return  _node_val;
        }
            

        Tree &
        add_child(Tree tree, EdgeType ed)
        {
            children.push_back(std::make_pair(ed, tree));
            return std::get<Tree>(children.back());
        }

        // search for a node matching the given predicate
        std::optional<Tree &>
        find(std::function< bool(NodeType &) >& lambda) const
        {
            for (auto & tree: *this)
            {
                if (lambda(tree.value()))
                {
                    return std::make_optional(tree);
                }
            } 
            return std::nullopt;

            /*
            if (lambda(_node_val))
            {
                return std::make_optional(*this);
            }
            else
            {
                for (auto & child: children)
                {
                    auto found = child.second.find(lambda);
                    if (found.has_value())
                    {
                        return found;
                    }
                }

                // empty optional if nothing found
                return std::nullopt;
            }
            */
        }

        bool
        is_leaf()
        {
            return children.size() == 0;
        }

        size_t
        children_count()
        {
            return children.size();
        }

        Tree &
        operator[](size_t child_index)
        {
            return std::get<Tree>(children[child_index]);
        }

        TreeIterator begin()
        {
            return TreeIterator(this);
        }
        TreeIterator end()
        {
            return TreeIterator();
        }
/*
        TreeIterator begin() const
        {
            return TreeIterator(this);
        }
        TreeIterator end() const
        {
            return TreeIterator();
        }
*/
    };


    


    
// note: there is no need for the center
    struct Ellipse
    {
        glm::vec3 forward;
        glm::vec3 up;
        glm::vec3 side;

        GLObject mesh;
        

        float peri;
        float apo;

        Ellipse(glm::vec3 fwd,
                glm::vec3 pup,
                float per,
                float ap)
        {
            forward = glm::normalize(fwd);
            up = glm::normalize(pup);
            side = glm::normalize(glm::cross(fwd, up));
            peri = per;
            apo = ap;
            mesh = ellipse_mesh(0.01f);
        }

        GLObject
        ellipse_mesh(float step)
        {
            std::vector<ShapeVertex> vertexes;
            for (float theta = 0.; theta < M_PI*2.0f; theta = theta + step)
            {
                auto point = pos_from_angle(theta);
                ShapeVertex vertex;
                vertex.position = point;
                vertexes.push_back(vertex);
            }
            vertexes.push_back(vertexes[0]);

            return GLObject(vertexes);            
        }

        
        float
        distance_from_center(float theta) const
        {
            float K = apo + peri;
            float delta = apo - peri;
            
            float a = (K*K - delta*delta)
                /(2.0f*(K + delta * glm::cos(theta)));
            return a;
        }
        
        glm::vec3
        pos_from_angle(float theta) const
        {
            float a = distance_from_center(theta);
            auto u = forward*glm::cos<float>(theta) + side * glm::sin<float>(theta);
            auto pos = a*u;
            return glm::vec3(pos);
        }

        glm::mat4
        transform_from_angle(float theta) const
        {
            auto pos = pos_from_angle(theta);
            return glm::translate(glm::mat4(1), glm::vec3(pos));
        }

        
        float
        speed(float theta) const
        {
            float a = distance_from_center(theta);

            return 1.0f/(a*a);
        }
        
    };

        
    struct Object
    {
        // physic data
        float mass;
        
        // spatial data
        float theta;
        glm::mat4 trans;

        // Geometric data
        glm::vec3 tilt;
        float spin;
        float scale;

        // rendering data
        PgrmHandle pgrm;
        GLObject mesh;

        //hitbox
        float sphere_radius;

        

        Object(GLObject ob, PgrmHandle pg):
            mass(1.0f),
            theta(0.0f),
            spin(0.0f),
            scale(1.0f),
            sphere_radius(-1.0f),
            mesh(ob),
            pgrm(pg)
        {
            
            
        }

        Object
        with_geometry(float sc)
        {
            scale = sc;
            sphere_radius = sc;
            return *this;
        }
        

        glm::mat4
        shape_transform() const
        {
            auto mat = glm::mat4(1);
            mat = glm::scale(mat, glm::vec3(scale));
            //mat = glm::rotate(mat, );
            return mat;
        }
        
                
    };

    

    
    struct World
    {
        Tree<Object, Ellipse> tree;
        std::vector<Light> lights;
        PgrmHandle curves_pgrm;

        World(Tree<Object, Ellipse> & tr, PgrmHandle cpgrm):
            tree(tr),
            curves_pgrm(cpgrm)
        {
//            tree = tr;
        }

        std::optional<Object *>
        select(glm::mat4 V, glm::mat4 P, glm::vec2 cursor_pos)
        {
            auto ray = cast_ray(V, P, cursor_pos);

            std::optional<Object *> closest = std::nullopt;
            float min_dist = std::numeric_limits<float>::max();
            auto i = 0;
            
            for(auto & objellipse: tree)
            {
                auto & obj = objellipse._node_val;
                auto pos = glm::vec3((P*V*obj.trans) * glm::vec4(0, 0, 0, 1));
                if (intersect_ray(ray, pos, obj.sphere_radius))
                {
                    std::cout << "INTERSECTING object of id " << i << std::endl;
                }
                ++i;
                
            }

            return closest;
            
            
        }
        

        void
        tick(float dt)
        {
            for (auto & node: tree)
            {
                for (auto & child: node.children)
                {
                    auto & theta = child.second._node_val.theta;
                    auto speed = child.first.speed(theta)*dt;
                    theta += speed;
                }
            }
        }
        
        static void
        update_pos( Tree<Object, Ellipse> & node,
                   const glm::mat4 & transform)
        {
            auto & obj = node._node_val;

            auto geo_trans = obj.shape_transform();
            obj.trans = transform*geo_trans;

            for (auto & child: node.children)
            {
                auto & child_tree = std::get<1>(child);
                auto & obj = child_tree._node_val;
                auto & el = std::get<Ellipse>(child);
                auto & theta = obj.theta;
                
                auto trans_child = el.transform_from_angle(theta);
                // TODO: check multiplication order
                update_pos(child_tree, transform*trans_child);

            }
        }
        

        
        Scene
        make_scene()
        {
            Scene scene;

            for (auto & light: lights)
            {
                switch (light.light_type)
                {
                    case LightType::Directional:
                        scene.lights_dir.push_back(light.light);
                        break;

                    case LightType::Ponctual:
                        scene.lights_pts.push_back(light.light);
                        break;

                    default:
                        std::cout << "Unrecognized light type: "
                                  << light.light_type
                                  << std::endl;
                }
            }
            

            
            update_pos(tree, glm::mat4(1));

            
            //auto & obj = tree._node_val;
            //scene.add(obj.mesh, obj.pgrm, {obj.trans});
            for (auto & node: tree)
            { 
                auto const & obj = node._node_val;
                scene.add(obj.mesh, obj.pgrm, {obj.trans});
                for (auto & child: node.children)
                {
                    auto & el = std::get<Ellipse>(child);

                    auto mesh = el.mesh;

                    glm::vec3 el_pos = glm::vec3(obj.trans * glm::vec4(0, 0, 0, 1));
                    auto el_trans = glm::translate(glm::mat4(1), el_pos);
                    
                    scene.add(mesh, curves_pgrm, {el_trans});
                }
                
            }
            
            return scene;

            
        }
    };
        

}
















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

