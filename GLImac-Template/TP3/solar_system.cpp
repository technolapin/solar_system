
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
    };


    


    
// note: there is no need for the center
    struct Ellipse
    {
        glm::vec3 center;
        glm::vec3 forward;
        glm::vec3 up;
        glm::vec3 side;

        float peri;
        float apo;

        Ellipse(glm::vec3 cent,
                glm::vec3 fwd,
                glm::vec3 pup,
                float per,
                float ap)
        {
            center = cent;
            forward = glm::normalize(fwd);
            up = glm::normalize(pup);
            side = glm::normalize(glm::cross(fwd, up));
            peri = per;
            apo = ap;
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
        
        glm::mat4
        transform_from_angle(float theta) const
        {
            
            float a = distance_from_center(theta);
            
            auto u = forward*glm::cos<float>(theta) + side * glm::sin<float>(theta);
            auto pos = center + a*u;
            //std::cout << a <<"  " << pos << std::endl;
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
        {}

        Object
        with_geometry(float sc)
        {
            scale = sc;
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

        World(Tree<Object, Ellipse> & tr):
            tree(tr)
        {
//            tree = tr;
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
        populate_scene(Scene & scene,
                       const Tree<Object, Ellipse> & node,
                       const glm::mat4 & transform)
        {
            auto & obj = node._node_val;

            auto geo_trans = obj.shape_transform();
            scene.add(obj.mesh, obj.pgrm, {transform*geo_trans});

            for (auto & child: node.children)
            {
                auto & child_tree = std::get<1>(child);
                auto & obj = child_tree._node_val;
                auto & el = std::get<Ellipse>(child);
                auto & theta = obj.theta;

                
                auto trans_child = el.transform_from_angle(theta);

                // TODO: check multiplication order
                populate_scene(scene, child_tree, transform*trans_child);

            }
        }
        
        Scene
        make_scene() const
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
            
            populate_scene(scene, tree, glm::mat4(1));
            
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



int main(int argc, char** argv)
{
    
    {
        Logic::Tree<int, float> tree(0);
        Logic::Tree<int, float> a(1);
        Logic::Tree<int, float> b(2);
        Logic::Tree<int, float> c(3);
        Logic::Tree<int, float> d(4);
        Logic::Tree<int, float> e(5);
        Logic::Tree<int, float> f(6);

        tree.add_child(a, 0.).add_child(d, 3.).add_child(e, 4.).add_child(f, 5.);
        tree.add_child(b, 1.);
        tree.add_child(c, 2.);

        for (auto x: tree)
        {
            std::cout << x.value() << " ";
        } std::cout << std::endl;
    }
    
    

    
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
    auto path_fs_mono = applicationPath.dirPath() + "shaders" + "phong1tex.fs.glsl";
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


        Logic::Ellipse el1(glm::vec3(0),
                           glm::vec3(1, 0, 0),
                           glm::vec3(0, 1, 0),
                           8.0f,
                           10.0f);
        Logic::Ellipse el2(glm::vec3(0),
                           glm::vec3(1, 0, 0),
                           glm::vec3(0, 1, 0),
                           3.0f,
                           4.0f);
        Logic::Ellipse el3(glm::vec3(0),
                           glm::vec3(1, 0, 0),
                           glm::vec3(0, 0, 1),
                           1.0f,
                           1.0f);
        Logic::Ellipse el4(glm::vec3(0),
                           glm::vec3(1, 0, 0),
                           glm::vec3(0, 1, 0),
                           0.3f,
                           0.2f);

        auto ob0 = Logic::Object(meshes[0], earth_renderer);
        auto obmoon = Logic::Object(meshes[0], moon_renderer);
        auto ob1 = obmoon
            .with_geometry(1.0f/4.0f);
        auto ob2 = obmoon.with_geometry(1.0f/16.0f);
        auto ob3 = obmoon.with_geometry(1.0f/64.0f);

        /*
 //        Logic::Object ob0 = {1.0f, 0.0f, glm::vec3(0), 0.0f, 1.0f,  earth_renderer, meshes[0], 1.0f};
        Logic::Object ob1 = {1.0f, 0.0f, glm::vec3(0), 0.0f, 0.25f,  moon_renderer, meshes[0], 1.0f/4.0f};
        Logic::Object ob2 = {1.0f, 0.0f, glm::vec3(0), 0.0f, 0.0625f,  moon_renderer, meshes[0], 1.0f/16.0f};

        Logic::Object ob3 = {1.0f, 0.0f, glm::vec3(0), 0.0f, 0.0625f/4.0f,  moon_renderer, meshes[0], 1.0f/64.0f};
        */

        Logic::Tree<Logic::Object, Logic::Ellipse> tree(ob0);

        tree.add_child(Logic::Tree<Logic::Object, Logic::Ellipse>(ob1), el1);
        tree.add_child(Logic::Tree<Logic::Object, Logic::Ellipse>(ob1), el2)
            .add_child(Logic::Tree<Logic::Object, Logic::Ellipse>(ob2), el3)
            .add_child(Logic::Tree<Logic::Object, Logic::Ellipse>(ob3), el4);

        
        Logic::World world(tree);

        world.lights.push_back({LightType::Directional, {glm::vec3(1), glm::vec3(0.7)}});
    


    Logic::Ellipse el(glm::vec3(0, 0, -5),
               glm::vec3(1, 0, 0),
               glm::vec3(0, 1, 0),
               1.0f,
               20.0f);
    
    float theta = 0.0f;

    bool running = true;
    
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

        
        if (running) world.tick(0.03f);

  
        auto test_scene =  world.make_scene();
        renderer.render(test_scene, earth_renderer, camera.getViewMatrix(), P);
        
        // Update the display
        windowManager.swapBuffers();
    }

    for (auto & obj: meshes)
    {
       obj.free_gpu();
    }
    
       
    return EXIT_SUCCESS;
}

