#include "Camera.h"
#include "../Application.h"

Camera::Camera()
{
    position = NULL;
    zoom = 1.0f;
    offsets = glm::vec2(0.0f, 0.0f);
    min = glm::vec2(0.0f, 0.0f);
    max = glm::vec2(0.0f, 0.0f);

    camera.x = camera.y = 0;
    camera.w = Application::width;
    camera.h = Application::height;
}

void Camera::Attach(glm::vec2* pos)
{
    if (pos != NULL)
    {
        position = pos;
    }
}

void Camera::SetZoom(float value)
{
    zoom = std::max(value, 1.0f);
}

void Camera::Update(double deltams)
{
    if (position == NULL)
    {
        return;
    }
    offsets = glm::vec2(position->x - (Application::width / 2.0f), position->y - (Application::height / 2.0f));
    offsets.x = std::max(min.x, offsets.x);
    offsets.y = std::max(min.y, offsets.y);
    offsets.x = std::min(max.x, offsets.x);
    offsets.y = std::min(max.y, offsets.y);

    camera.x = (int)offsets.x;
    camera.y = (int)offsets.y;
}

void Camera::SetBoundary(float left, float right, float up, float down)
{
    min = glm::vec2(left, up);
    max = glm::vec2(right-(float)camera.w, down-(float)camera.h);
}

float Camera::GetZoom()
{
    return zoom;
}
