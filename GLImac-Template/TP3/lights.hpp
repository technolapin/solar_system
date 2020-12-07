

struct AnyLight
{
   glm::vec3 direction;
   glm::vec3 intensity;
};

enum LightType
{
   Directional,
   Ponctual
};

struct Light
{
   LightType light_type;
   AnyLight light;
};

