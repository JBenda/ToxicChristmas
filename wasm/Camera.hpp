#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    Camera(const glm::vec2& _bottomLeft, const glm::vec2& _topRight, glm::vec2 _pos = glm::vec2(0,0)) : m_pos{_pos, 0} {
        m_projection = glm::ortho<float>(_bottomLeft.x, _topRight.x, _topRight.y, _bottomLeft.y, -1, 1);
        m_projectionMoved = glm::translate(m_projection, m_pos);
    }
    const float* GetProjectionMatrix() const { return glm::value_ptr(m_projectionMoved); }
    void SetPosition(const glm::vec2& _pos) {
        m_pos = glm::vec3(_pos, 0);
        m_projectionMoved = glm::translate(m_projection, m_pos);
    }
private:
    glm::mat4 m_projectionMoved; /**< projection Matrix moved to Position*/
    glm::mat4 m_projection; /**< projction Matrix */
    glm::vec3 m_pos;
};
