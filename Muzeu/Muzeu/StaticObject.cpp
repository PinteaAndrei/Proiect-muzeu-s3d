#include "StaticObject.h"

StaticObject::StaticObject(Model model, const int width, const int height, const glm::vec3& position)
{
    objectModel = model;
    startPosition = position;
    Set(width, height, position);
}

StaticObject& StaticObject::operator=(const StaticObject& otherObject)
{
    objectModel = otherObject.objectModel;
    startPosition = otherObject.startPosition;
    Set(otherObject.width, otherObject.height, otherObject.position);
    return *this;
}

void StaticObject::Set(const int width, const int height, const glm::vec3& position)
{
    this->isPerspective = true;
    this->yaw = YAW;

    this->FoVy = FOV;
    this->width = width;
    this->height = height;
    this->zNear = zNEAR;
    this->zFar = zFAR;

    this->worldUp = glm::vec3(0, 1, 0);
    this->position = position;

    lastX = width / 2.0f;
    lastY = height / 2.0f;

    UpdateObjectVectors();
}

const glm::vec3 StaticObject::GetPosition() const
{
    return position;
}

const glm::mat4 StaticObject::GetProjectionMatrix() const
{
    glm::mat4 Proj = glm::mat4(1);
    if (isPerspective)
    {
        float aspectRatio = ((float)(width)) / height;
        Proj = glm::perspective(glm::radians(FoVy), aspectRatio, zNear, zFar);
    }
    else
    {
        float scaleFactor = 2000.f;
        Proj = glm::ortho<float>(
            -width / scaleFactor, width / scaleFactor,
            -height / scaleFactor, height / scaleFactor, -zFar, zFar);
    }
    return Proj;
}



Model& StaticObject::GetModel()
{
    return objectModel;
}

float StaticObject::GetRotation()
{
    return rotation;
}

void StaticObject::SetRotation(float newRotation)
{
    rotation = newRotation;
}

glm::vec3 StaticObject::GetForward()
{
    return forward;
}

float StaticObject::GetYaw()
{
    return yaw;
}

void StaticObject::UpdateObjectVectors()
{
    // Calculate the new forward vector
    this->forward.x = cos(glm::radians(yaw));
    this->forward.z = sin(glm::radians(yaw));
    this->forward = glm::normalize(this->forward);

   

    right = glm::normalize(glm::cross(forward, worldUp)); 
}
