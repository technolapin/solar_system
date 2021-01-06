#include "primitives.hpp"

namespace Primitives
{
    
    void Sphere::build(GLfloat r, GLsizei discLat, GLsizei discLong) {
        // Equation paramétrique en (r, phi, theta) de la sphère 
        // avec r >= 0, -PI / 2 <= theta <= PI / 2, 0 <= phi <= 2PI
        //
        // x(r, phi, theta) = r sin(phi) cos(theta)
        // y(r, phi, theta) = r sin(theta)
        // z(r, phi, theta) = r cos(phi) cos(theta)
        //
        // Discrétisation:
        // dPhi = 2PI / discLat, dTheta = PI / discLong
        //
        // x(r, i, j) = r * sin(i * dPhi) * cos(-PI / 2 + j * dTheta)
        // y(r, i, j) = r * sin(-PI / 2 + j * dTheta)
        // z(r, i, j) = r * cos(i * dPhi) * cos(-PI / 2 + j * dTheta)

        GLfloat rcpLat = 1.f / discLat, rcpLong = 1.f / discLong;
        GLfloat dPhi = 2 * glm::pi<float>() * rcpLat, dTheta = glm::pi<float>() * rcpLong;
    
        std::vector<Vertex> data;
    
        // Construit l'ensemble des vertex
        for(GLsizei j = 0; j <= discLong; ++j) {
            GLfloat cosTheta = cos(-glm::pi<float>() / 2 + j * dTheta);
            GLfloat sinTheta = sin(-glm::pi<float>() / 2 + j * dTheta);
        
            for(GLsizei i = 0; i <= discLat; ++i) {
                Vertex vertex;
            
                vertex.texCoords.x = i * rcpLat;
                vertex.texCoords.y = 1.f - j * rcpLong;

                vertex.normal.x = sin(i * dPhi) * cosTheta;
                vertex.normal.y = sinTheta;
                vertex.normal.z = cos(i * dPhi) * cosTheta;
            
                vertex.position = r * vertex.normal;

                data.push_back(vertex);
            }
        }

        m_nVertexCount = discLat * discLong * 6;
    
        GLuint idx = 0;
        // Construit les vertex finaux en regroupant les données en triangles:
        // Pour une longitude donnée, les deux triangles formant une face sont de la forme:
        // (i, i + 1, i + discLat + 1), (i, i + discLat + 1, i + discLat)
        // avec i sur la bande correspondant à la longitude
        for(GLsizei j = 0; j < discLong; ++j) {
            GLsizei offset = j * (discLat + 1);
            for(GLsizei i = 0; i < discLat; ++i) {
                m_Vertices.push_back(data[offset + i]);
                m_Vertices.push_back(data[offset + (i + 1)]);
                m_Vertices.push_back(data[offset + discLat + 1 + (i + 1)]);
                m_Vertices.push_back(data[offset + i]);
                m_Vertices.push_back(data[offset + discLat + 1 + (i + 1)]);
                m_Vertices.push_back(data[offset + i + discLat + 1]);
            }
        }

        for (auto i = 0; i < m_Vertices.size()/3; ++i)
        {
            auto & v1 = m_Vertices[i*3];
            auto & v2 = m_Vertices[i*3+1];
            auto & v3 = m_Vertices[i*3+2];

            glm::vec3 edge1 = v2.position - v1.position;
            glm::vec3 edge2 = v3.position - v1.position;
            glm::vec2 deltaUV1 = v2.texCoords - v1.texCoords; 
            glm::vec2 deltaUV2 = v3.texCoords - v1.texCoords;
            float f = 1.0f / (deltaUV1.x*deltaUV2.y - deltaUV2.x*deltaUV1.y);

            glm::vec3 tangent1 = glm::vec3(1.0f);
            glm::vec3 bitangent1 = glm::vec3(1.0f);
            
            tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

            bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

            v1.tangent = tangent1;
            v2.tangent = tangent1;
            v3.tangent = tangent1;
            v1.bitangent = bitangent1;
            v2.bitangent = bitangent1;
            v3.bitangent = bitangent1;
            
            
        }
        

        
        // Attention ! dans cette implantation on duplique beaucoup de sommets. Une meilleur stratégie est de passer
        // par un Index Buffer Object, que nous verrons dans les prochains TDs
    }

}
