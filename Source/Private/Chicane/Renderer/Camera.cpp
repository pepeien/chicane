#include "Chicane/Renderer/Camera.hpp"

constexpr float MAX_CAMERA_SPAN_SPEED = 0.7f;

constexpr float MIN_CAMERA_ZOOM_SPEED = 0.0f;
constexpr float MAX_CAMERA_ZOOM_SPEED = 4.0f;

namespace Chicane
{
    Camera::Camera()
        : m_transform({}),
        m_focalPoint({ 0.0f, 0.0f, 0.0f }),
        m_viewportWidth(1280),
        m_viewportHeight(720),
        m_fov(45.0f),
        m_aspectRatio(1.77f),
        m_nearClip(0.2f),
        m_farClip(50000.0f),
        m_moveDistance(0.7f),
        m_UBO({})
    {}

    void Camera::setViewport(std::uint32_t inWidth, std::uint32_t inHeight)
    {
        m_viewportWidth  = inWidth;
        m_viewportHeight = inHeight;
        m_aspectRatio    = 0;

        if (inWidth == 0 || inHeight == 0)
        {
            return;
        }

        setAspectRatio(
            static_cast<float>(m_viewportWidth / m_viewportHeight)
        );
    }

    void Camera::setViewport(const Vec<std::uint32_t>::Two& inViewportResolution)
    {
        setViewport(
            inViewportResolution.x,
            inViewportResolution.y
        );
    }

    Vec<float>::Three Camera::getPosition()
    {
        return m_transform.translation;
    }

    void Camera::setPosition(const Vec<float>::Three& inPosition)
    {
        m_transform.translation = inPosition;
    }

    Vec<float>::Three Camera::getRotation()
    {
        return m_transform.rotation;
    }

    void Camera::setRotation(const Vec<float>::Three& inRotation)
    {
        m_transform.rotation = inRotation;

        addPitch(inRotation.x);
        addRoll(inRotation.y);
        addYaw(inRotation.z);
    }

    void Camera::pan(float inX, float inY)
    {
        Vec<float>::Two panSpeed = getPanSpeed();

		m_focalPoint += -getRight() * inX * panSpeed.x * m_moveDistance;
		m_focalPoint += getUp() * inY * panSpeed.y * m_moveDistance;
    }

    void Camera::pan(const Vec<float>::Two& inDelta)
    {
        pan(
            inDelta.x,
            inDelta.y
        );
    }

    void Camera::zoom(float inDelta)
    {
        m_moveDistance -= inDelta * getZoomSpeed();

		if (m_moveDistance < 1.0f)
		{
			m_focalPoint += getForward();
			m_moveDistance = 1.0f;
		}
    }

    void Camera::addPitch(float inPitch)
    {
		m_transform.rotation.x += inPitch * getRotationSpeed();
    }

    void Camera::addRoll(float inRoll)
    {
        m_transform.rotation.y += inRoll * getRotationSpeed();
    }

    void Camera::addYaw(float inYaw)
    {
        float yawSign = getUp().y < 0 ? -1.0f : 1.0f;

		m_transform.rotation.z += inYaw * getRotationSpeed();
    }

    Quat<float>::Default Camera::getOrientation()
    {
        return Quat<float>::Default(m_transform.rotation * -1.0f);
    }

    Vec<float>::Three Camera::getForward()
    {
        return glm::rotate(getOrientation(), CAMERA_FORWARD_DIRECTION);
    }

    Vec<float>::Three Camera::getRight()
    {
        return glm::rotate(getOrientation(), CAMERA_RIGHT_DIRECTION);
    }

    Vec<float>::Three Camera::getUp()
    {
        return glm::rotate(getOrientation(), CAMERA_UP_DIRECTION);
    }

    UBO Camera::getUBO()
    {
        return m_UBO;
    }

    void Camera::updateUBO()
    {
        m_UBO.view           = getView();
        m_UBO.projection     = getProjection();
        m_UBO.viewProjection = m_UBO.projection * m_UBO.view;

        m_UBO.forward = Vec<float>::Four(getForward(), 0.0f);
        m_UBO.right   = Vec<float>::Four(getRight(), 0.0f);
        m_UBO.up      = Vec<float>::Four(getUp(), 0.0f);
    }

    Vec<float>::Two Camera::getPanSpeed()
    {
		return Vec<float>::Two(0.01f);
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
    }

    Mat<float>::Four Camera::getView()
    {
		Mat<float>::Four result = glm::lookAt(
            m_transform.translation,
            m_focalPoint,
            getUp()
        );
        result = glm::inverse(result);

        return result;
    }

    Mat<float>::Four Camera::getProjection()
    {
        Mat<float>::Four result = glm::perspective(
            glm::radians(m_fov),
            m_aspectRatio,
            m_nearClip,
            m_farClip
        );
        // Normalize OpenGL's to coordinate system Vulkan
        result[1][1] *= -1;

        return result;
    }
}