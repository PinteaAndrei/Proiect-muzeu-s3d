#pragma once

#include <glew.h>
#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>
#include <glfw3.h>

#include "Model.h"
class StaticObject
{
private:
    const float zNEAR = 0.1f;
    const float zFAR = 500.f;
    const float YAW = -90.0f;
    const float FOV = 45.0f;
    glm::vec3 startPosition;
    Model objectModel;

public:
    StaticObject() = default;
    StaticObject(Model model, const int width, const int height, const glm::vec3& position);

    StaticObject& operator=(const StaticObject& othervehicle);

    void Set(const int width, const int height, const glm::vec3& position);

    const glm::vec3 GetPosition() const;

    const glm::mat4 GetProjectionMatrix() const;

    Model& GetModel();

    float GetRotation();

    void SetRotation(float newRotation);

    glm::vec3 GetForward();

    float GetYaw();

private:
    void UpdateObjectVectors();

protected:


    // Perspective properties
    float zNear;
    float zFar;
    float FoVy;
    int width;
    int height;
    bool isPerspective;

    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 right;
    float rotation = 1.0f;
    glm::vec3 worldUp;

 
    float yaw;

    float lastX = 0.f, lastY = 0.f;
};

