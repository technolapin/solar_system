struct TrackballCamera
{
   glm::vec3 center;
   
   float m_fDistance;
   float m_fAngleX;
   float m_fAngleY;

   TrackballCamera()
   {
      center = glm::vec3(0.0f);
      m_fDistance = 0.0f;
      m_fAngleX = 0.0f;
      m_fAngleY = 0.0f;
   }

   void set_center(glm::vec3 ctr)
   {
      center = ctr;
   }
   
   void moveFront(float delta)
   {
      m_fDistance += delta;
   }

   int
   rotation_sign()
   {
      if (glm::cos(m_fAngleY) < 0.0)
      {
         return -1;
      }
      else
      {
         return 1;
      }
        
   }
   
   void rotateLeft(float degrees)
   {
      m_fAngleX =  glm::mod<float>(m_fAngleX + rotation_sign()*degrees*3.14/360.0, 2.0*glm::pi<float>());
   }

   void rotateUp(float degrees)
   {
      m_fAngleY = glm::mod<float>(m_fAngleY + degrees*3.14f/360.0f, 2.0*glm::pi<float>());
   }

   
   glm::mat4
   getViewMatrix() const
   {
      auto x = glm::vec3(1.0f, 0.0f, 0.0f);
      auto y = glm::vec3(0.0f, 1.0f, 0.0f);
      auto z = glm::vec3(0.0f, 0.0f, 1.0f);

      auto V = glm::translate(glm::mat4(1.0f), -z*m_fDistance);
      V = glm::rotate(V, m_fAngleX, y);
      V = glm::rotate(V, m_fAngleY, glm::vec3(glm::cos(m_fAngleX), 0, glm::sin(m_fAngleX))); 
      V = glm::translate(V, -center);

      return V;
   }


};

