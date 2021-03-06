#pragma once
struct Renderable
{
private:
   std::shared_ptr<ProgramHolder> _pgrm;

public:
   std::vector<GLValue> uniforms_values;
   Renderable(std::shared_ptr<ProgramHolder> pgrm,
              std::vector<GLValue> values)
   {
      _pgrm = pgrm;
      uniforms_values = values;
   }

   
   void
   setup() const
   {
      _pgrm->use();
      auto texture_indice = 0;
      for (int i = 0; i <  _pgrm->uniforms.size(); ++i)
      {
         auto & uni = _pgrm->uniforms[i];
         uni.setup(uniforms_values[i], &texture_indice);
      }
   }

   bool operator==(const Renderable &other) const
   {
      return _pgrm == other._pgrm;
   }

   
};


struct RenderableBuilder
{
private:
   std::shared_ptr<ProgramHolder> _pgrm;

public:
   std::vector<GLValue> uniforms_values;
   RenderableBuilder(std::shared_ptr<ProgramHolder>& pgrm)
   {
      _pgrm = pgrm;
   }
   template <class GLT>
   RenderableBuilder &
   with(GLType t, GLT val)
   {
      // todo: assert on type
      GLValue gl_val(t, val);
      uniforms_values.push_back(gl_val);
      return *this;
   }

   bool
   is_valid()
   {
      auto size = uniforms_values.size();
      if (uniforms_values.size() != _pgrm->uniforms.size())
      {
         return false;
      }
      else
      {
         for (auto i = 0; i < size; ++i)
         {
            if (uniforms_values[i].type != _pgrm->uniforms[i].type)
            {
               return false;
            }
         }
         return true;
      }   
   }

   Renderable
   build()
   {
      assert(is_valid());

      return {
         _pgrm, uniforms_values
      };
      
   }

   
   
};
