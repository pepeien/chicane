#include "Runtime/Renderer/Camera.hpp"

constexpr float MAX_CAMERA_SPAN_SPEED = 0.7f;

constexpr float MIN_CAMERA_ZOOM_SPEED = 0.0f;
constexpr float MAX_CAMERA_ZOOM_SPEED = 4.0f;

namespace Chicane
{
    Camera::Camera()
    : m_position({ 0.0f, 0.0f, 0.0f }),
      m_focalPoint({ 0.0f, 0.0f, 0.0f }),
      m_yaw(0.0f),
      m_pitch(0.0f),
      m_viewportWidth(1280),
      m_viewportHeight(720),
      m_fov(45.0f),
      m_aspectRatio(1.77f),
      m_nearClip(0.01f),
      m_farClip(1000.0f),
      m_moveDistance(0.7f),
      m_matrixUBO({}),
      m_vectorUBO({})
    {}

    void Camera::panTo(const glm::vec2& inTarget)
    {
        glm::vec2 panSpeed = getPanSpeed();

		m_focalPoint += -getRight() * inTarget.x * panSpeed.x * m_moveDistance;
		m_focalPoint += getUp() * inTarget.y * panSpeed.y * m_moveDistance;

        updateUBOs();
    }

    void Camera::rotateTo(const glm::vec2& inTarget)
    {
        float rotationSpeed = getRotationSpeed();
        float yawSign       = getUp().z < 0 ? -1.0f : 1.0f;

		m_yaw   += yawSign * inTarget.x * rotationSpeed;
		m_pitch += inTarget.y * rotationSpeed;

        updateUBOs();
    }

    void Camera::zoomTo(float inTarget)
    {
        m_moveDistance -= inTarget * getZoomSpeed();

		if (m_moveDistance < 1.0f)
		{
			m_focalPoint  += getUp();
			m_moveDistance = 1.0f;
		}

        updateUBOs();
    }

    glm::vec3 Camera::getPosition()
    {
        return m_position;
    }

    void Camera::setViewportResolution(uint32_t inWidth, uint32_t inHeight)
    {
        m_viewportWidth  = inWidth;
        m_viewportHeight = inHeight;

        setAspectRatio(
            static_cast<float>(m_viewportWidth / m_viewportHeight)
        );
    }

    glm::quat Camera::getOrientation()
    {
        return glm::quat(glm::vec3(-m_pitch, -m_yaw, 0.0f));
    }

    glm::vec3 Camera::getForward()
    {
        return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 Camera::getRight()
    {
        return glm::rotate(getOrientation(), glm::vec3(0.0f, -1.0f, 0.0f));
    }

    glm::vec3 Camera::getUp()
    {
        return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    VectorUBO Camera::getVectorUBO()
    {
        return m_vectorUBO;
    }

    MatrixUBO Camera::getMatrixUBO()
    {
        return m_matrixUBO;
    }

    glm::vec2 Camera::getPanSpeed()
    {
        glm::vec2 result = glm::vec2(0.0f);

        float x = std::min(m_viewportWidth / 1000.0f, MAX_CAMERA_SPAN_SPEED);
		float y = std::min(m_viewportHeight / 1000.0f, MAX_CAMERA_SPAN_SPEED);

        result.x = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;
        result.y = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return result;
    }

    float Camera::getRotationSpeed()
    {
        return 0.01f;
    }

    float Camera::getZoomSpeed()
    {
        float distance = m_moveDistance * 0.2f;
		distance       = std::max(distance, MIN_CAMERA_ZOOM_SPEED);

		float result = distance * distance;
		result       = std::min(result, MAX_CAMERA_ZOOM_SPEED);

		return result;
    }

    void Camera::setAspectRatio(float inAspectRatio)
    {
        m_aspectRatio = inAspectRatio;

        updateUBOs();
    }

    glm::mat4 Camera::getView()
    {
		glm::quat orientation = getOrientation();

		glm::mat4 result = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(orientation);
		result           = glm::inverse(result);

        return result;
    }

    glm::mat4 Camera::getProjection()
    {
        glm::mat4 result = glm::perspective(
            glm::radians(m_fov),
            m_aspectRatio,
            m_nearClip,
            m_farClip
        );
        // Normalize OpenGL's to coordinate system Vulkan
        result[1][1] *= -1;

        return result;
    }

    void Camera::updateUBOs()
    {
        m_position = m_focalPoint - getForward() * m_moveDistance;

        m_vectorUBO.forward = getForward();
        m_vectorUBO.right   = getRight();
        m_vectorUBO.up      = getUp();

        m_matrixUBO.view           = getView();
        m_matrixUBO.projection     = getProjection();
        m_matrixUBO.viewProjection = m_matrixUBO.projection * m_matrixUBO.view;
    }
}