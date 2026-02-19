#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include "glfw/glfw3.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <ostream>

Camera::Camera(glm::vec3 position, glm::vec3 look, glm::vec3 up, int width, int height, float fov, float movementSpeed, float rotationSpeed):
    m_position(position),
    m_look(look),
    m_up(up),
    m_aspectRatio((float)width/(float)height),
    m_screenSize(glm::ivec2(width, height)),
    m_fov(fov),
    m_movementSpeed(movementSpeed),
    m_rotationSpeed(rotationSpeed),
    m_viewMatrix(glm::mat4(1.f)),
    m_projectionMatrix(glm::mat4(1.f))
{
    CalculateViewMatrix();
    CalculateProjectionMatrix();
}

Camera::~Camera()
{
}

glm::mat4 Camera::GetViewMatrix() const
{
    return m_viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return m_projectionMatrix;
}

void Camera::Move(const std::unordered_set<int>& keysPressed, double deltaTime)
{
    glm::vec3 right = glm::normalize(glm::cross(m_look, m_up));
    glm::vec3 localUp = glm::normalize(glm::cross(right, m_look));

    glm::vec3 moveDirection = glm::vec3(0.f);
    if (keysPressed.count(GLFW_KEY_W) > 0)
    {
        moveDirection += m_look;
    }
    if (keysPressed.count(GLFW_KEY_S) > 0)
    {
        moveDirection -= m_look;
    }
    if (keysPressed.count(GLFW_KEY_D) > 0)
    {
        moveDirection += right;
    }
    if (keysPressed.count(GLFW_KEY_A) > 0)
    {
        moveDirection -= right;
    }
    if (keysPressed.count(GLFW_KEY_SPACE) > 0)
    {
        moveDirection += localUp;
    }
    if (keysPressed.count(GLFW_KEY_LEFT_SHIFT) > 0)
    {
        moveDirection -= localUp;
    }

    if (moveDirection != glm::vec3(0.f))
    {
        moveDirection = glm::normalize(moveDirection) * m_movementSpeed * (float)deltaTime;

        m_position += moveDirection;
        CalculateViewMatrix();
    }
}

void Camera::Rotate(double yRot, double xRot)
{
    glm::mat4 rotationMatrix = glm::mat4(1.0f);

    float yRotationAmount = glm::radians((float)yRot) * m_rotationSpeed;
    float xRotationAmount = glm::radians((float)xRot) * m_rotationSpeed;

    glm::vec3 xRotationAxis = glm::normalize(glm::cross(m_look, m_up));

    rotationMatrix = glm::rotate(rotationMatrix, yRotationAmount, m_up);
    rotationMatrix = glm::rotate(rotationMatrix, xRotationAmount, xRotationAxis);

    m_look = rotationMatrix * glm::vec4(m_look, 0);

    CalculateViewMatrix();
}

void Camera::Resize(int width, int height)
{
    m_aspectRatio = (float)width / (float)height;
    m_screenSize = glm::ivec2(width, height);
    CalculateProjectionMatrix();
}

void Camera::CalculateViewMatrix()
{
    m_viewMatrix = glm::lookAt(m_position, m_position + m_look, m_up);
}

void Camera::CalculateProjectionMatrix()
{
    m_projectionMatrix = glm::perspective(m_fov, m_aspectRatio, 0.1f, 100.f);
}