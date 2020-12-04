struct FreeflyCamera
{
   glm::vec3 position;

   float m_fPhi;
   float m_fTheta;   
   
   glm::vec3 forward;
   glm::vec3 up;
   glm::vec3 side;

   FreeflyCamera()
   {
      position = glm::vec3(0.0f);
      m_fPhi = glm::pi<float>();
      m_fTheta = 0.0f;
      computeDirectionVectors();
   }
   void
   computeDirectionVectors()
   {
      auto planar_vec = glm::vec3(glm::sin(m_fPhi), 0.0, glm::cos(m_fPhi));
      auto y = glm::vec3(0.0f, 1.0f, 0.0f);
      side = glm::vec3(glm::cos(m_fPhi), 0.0f, glm::sin(m_fPhi));
      forward = glm::cos(m_fTheta)*planar_vec + glm::sin(m_fTheta)*y;
      up = glm::cross(forward, side);
   }

   void
   moveLeft(float t)
   {
      position += side*t;
   }

   void
   moveFront(float t)
   {
      position += forward*t;
   }

   void
   moveUp(float t)
   {
      position += up*t;
   }

   int
   rotation_sign()
   {
      if (glm::cos(m_fTheta) < 0.0)
      {
         return -1;
      }
      else
      {
         return 1;
      }
   }


   void
   rotateLeft(float degrees) 
   {
      m_fPhi =  glm::mod<float>(m_fPhi + rotation_sign()*degrees*3.14/360.0, 2.0*glm::pi<float>());
   }

   void rotateUp(float degrees)
   {
      m_fTheta = glm::mod<float>(m_fTheta + degrees*3.14f/360.0f, 2.0*glm::pi<float>());
   }


   
   
   
   
   glm::mat4
   getViewMatrix() const
   {
      auto V  = glm::lookAt(position, forward, up);
      return V;
   }


};

