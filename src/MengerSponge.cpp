#include "MengerSponge.h"

#include <glm/glm.hpp>

MengerSponge::MengerSponge(float size, int& layerCount):
    m_size(size),
    m_layerCount(layerCount)
{
    TryRegenerate();
}

MengerSponge::~MengerSponge()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void MengerSponge::Draw()
{
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

    glDrawElements(GL_TRIANGLES, m_elementCount, GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void MengerSponge::TryRegenerate()
{
    if(m_layerCount == m_prevLayerCount){
        return;
    }

    m_prevLayerCount = m_layerCount;

    if(m_vao != 0){
        glDeleteVertexArrays(1, &m_vao);
    }
    if(m_vbo != 0){
        glDeleteBuffers(1, &m_vbo);
    }
    if(m_ebo != 0){
        glDeleteBuffers(1, &m_ebo);
    }

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    GenerateMengerSponge(vertices, indices, glm::vec3(0, 0, 0), m_size / 2.f, m_layerCount);

    m_elementCount = indices.size();

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MengerSponge::GenerateMengerSponge(std::vector<glm::vec3>& vertices,
        std::vector<unsigned int>& indices,
        glm::vec3 center,
        float radius,
        int level)
{
    if(level == 0){
        return;
    }
    else if(level == 1){
        GenerateCube(vertices, indices, center, radius);
        return;
    }

    float currentRadius = radius / 3.f; // 3 cubes in each dimension

    for(int x = -1; x <= 1; x++){
        for(int y = -1; y <= 1; y++){
            for(int z = -1; z <= 1; z++){
                int numZero = (x == 0) + (y == 0) + (z == 0);
                // If in center, we have x = y = z = 0
                // If in center of face, two of x, y, z are 0
                // Only add solid if at most one of x, y, z is zero

                if(numZero <= 1){
                    glm::vec3 currentCenter = center + glm::vec3(x, y, z) * currentRadius * 2.f;

                    GenerateMengerSponge(vertices, indices, currentCenter, currentRadius, level - 1);
                }
            }
        }
    }
}

void MengerSponge::GenerateCube(std::vector<glm::vec3>& vertices, 
        std::vector<unsigned int>& indices, 
        glm::vec3 center, 
        float radius)
{
    unsigned int startIndex = vertices.size();

    std::vector<glm::vec3> cubeVertices = {
        {-radius, -radius, radius}, {radius, -radius, radius}, {radius, radius, radius}, {-radius, radius, radius},
        {-radius, -radius, -radius}, {radius, -radius, -radius}, {radius, radius, -radius}, {-radius, radius, -radius}
    };

    for(const auto& cubeVertex : cubeVertices){
        vertices.push_back(cubeVertex + center);
    }

    std::vector<unsigned int> cubeIndices = {
        0, 1, 2,  2, 3, 0, // Front
        1, 5, 6,  6, 2, 1, // Right
        7, 6, 5,  5, 4, 7, // Back
        4, 0, 3,  3, 7, 4, // Left
        4, 5, 1,  1, 0, 4, // Bottom
        3, 2, 6,  6, 7, 3  // Top
    };

    for(const auto& cubeIndex : cubeIndices){
        indices.push_back(cubeIndex + startIndex);
    }
}