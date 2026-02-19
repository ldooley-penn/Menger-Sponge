#pragma once

#include <vector>
#include <glad/glad.h>

#include "glm/vec3.hpp"

class MengerSponge {
public:
    MengerSponge(float size, int& layerCount);
    ~MengerSponge();

    void Draw();

    void TryRegenerate();

private:
    void GenerateMengerSponge(std::vector<glm::vec3>& vertices,
        std::vector<unsigned int>& indices,
        glm::vec3 center,
        float radius,
        int level);

    void GenerateCube(std::vector<glm::vec3>& vertices, 
        std::vector<unsigned int>& indices, 
        glm::vec3 center, 
        float radius);

    float m_size;
    int& m_layerCount;
    int m_prevLayerCount = 0;

    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;

    int m_elementCount = 0;
};