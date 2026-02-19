#pragma once

#include <unordered_set>
#include <glm/glm.hpp>

class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 look, glm::vec3 up, int width, int height, float m_fov = 1.f, float movementSpeed = 2.f, float rotationSpeed = 0.2f);
    ~Camera();

    [[nodiscard]] glm::mat4 GetViewMatrix() const;
    [[nodiscard]] glm::mat4 GetProjectionMatrix() const;

    void Move(const std::unordered_set<int>& keysPressed, double deltaTime);
    void Rotate(double yRot, double xRot);
    void Resize(int width, int height);

private:
    void CalculateViewMatrix();
    void CalculateProjectionMatrix();

    glm::vec3 m_position;
    glm::vec3 m_look;
    glm::vec3 m_up;

    float m_aspectRatio;
    glm::ivec2 m_screenSize;
    float m_fov;

    float m_movementSpeed;
    float m_rotationSpeed;

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
};