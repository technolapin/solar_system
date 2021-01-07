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
            return std::get<1>(children[child_index]);
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

       std::vector<Tree<Object, Ellipse> *> selected;
       
       
       World(Tree<Object, Ellipse> & tr, PgrmHandle cpgrm):
            tree(tr),
            curves_pgrm(cpgrm)
        {
           selected  = {&tree};
        }

       void
       select_root()
       {
          if (selected.size() > 1)
          {
             selected.pop_back();
          }
       }

       void
       select_child()
       {
          if (selected.back()->children.size() > 0)
          {
             selected.push_back(& std::get<1>(selected.back()->children[0]));
          }
       }

       void
       select_prev_adelphe()
       {
          if (selected.size() > 1)
          {
             auto & mother = **std::prev(std::prev(selected.end()));
             auto i = 0;
             for (auto & pair: mother.children)
             {
                auto & child = std::get<1>(pair);
                if (&child == selected.back())
                {
                   selected.back() = & std::get<1>(mother.children[(i-1) % mother.children.size()]);
                   return;
                }
                i++;
             }
          }
       }
       
       void
       select_next_adelphe()
       {
          if (selected.size() > 1)
          {
             auto & mother = **std::prev(std::prev(selected.end()));
             auto i = 0;
             for (auto & pair: mother.children)
             {
                auto & child = std::get<1>(pair);
                if (&child == selected.back())
                {
                   selected.back() = & std::get<1>(mother.children[(i+1) % mother.children.size()]);
                   return;
                }
                i++;
             }
          }
       }

       glm::vec3
       get_selected_pos()
       {
          auto & obj = selected.back()->_node_val;
          return glm::vec3(obj.trans * glm::vec4(0, 0, 0, 1));
       }
       
        std::optional<Object *>
        select(glm::mat4 V, glm::mat4 P, glm::vec2 cursor_pos)
        {
            auto ray = cast_ray(V, P, cursor_pos);

            std::optional<Object *> closest = std::nullopt;
            float min_dist = std::numeric_limits<float>::max();
            auto i = 0;
            
//            std::cout << "RAY = " << ray << std::endl;
            auto camera_pos = glm::vec3(V * glm::vec4(0, 0, 0, 1));
            for(auto & objellipse: tree)
            {
               
                auto & obj = objellipse._node_val;
                auto pos = glm::vec3((obj.trans) * glm::vec4(0, 0, 0, 1)) - camera_pos;
                //std::cout << "DEDUCED POS=" << pos << std::endl;
                //std::cout << "RAD = " << obj.sphere_radius << std::endl;
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

                update_pos(child_tree, transform*trans_child);

            }
        }
        

        
        Scene
        make_scene(bool partial_render)
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

            auto rendered = &tree;
            if (partial_render)
            {
               rendered = selected.back();
            }

            for (auto & node: *rendered)
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
