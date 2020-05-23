#include "Camera.hpp"

namespace Renderer {
	void Camera::UpdateCameraVectors() {
		// Calculate new front vector
		glm::vec3 front;
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_Front = glm::normalize(front);

		// Also re-calculate the Right and Up vector

		// Normalize the vectors, because their length gets closer to 0
		// the more you look up or down which results in slower movement.
		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}
	
	Camera::Camera(
		glm::vec3 position, 
		glm::vec3 up, 
		glm::vec3 front,
		float yaw,
		float pitch
	) : 
		m_Yaw(yaw), m_Pitch(pitch) {
		
		m_Position = position;
		m_Up = m_WorldUp = up;
		m_Front = front;

		UpdateCameraVectors();
	}

	Camera::Camera(
		float posX, 
		float posY, 
		float posZ, 
		float upX, 
		float upY, 
		float upZ, 
		float yaw, 
		float pitch
	) : 
		m_Pitch(pitch), 
		m_Yaw(yaw) {

		m_Position = glm::vec3(posX, posY, posZ);
		m_Up = m_WorldUp = glm::vec3(upX, upY, upZ);
		m_Front = glm::vec3(0.0f, 0.0f, -1.0f);

		UpdateCameraVectors();
	}

	void Camera::ProcessKeyboard(Camera_Movemet direction, float deltaTime) {
		float velocity = m_Speed * deltaTime;

		if (direction == Camera_Movemet::FORWARD) {
			m_Position += m_Front * velocity;
		}

		if (direction == Camera_Movemet::BACKWARD) {
			m_Position -= m_Front * velocity;
		}

		if (direction == Camera_Movemet::LEFT) {
			m_Position -= m_Right * velocity;
		}

		if (direction == Camera_Movemet::RIGHT) {
			m_Position += m_Right * velocity;
		}

		if (direction == Camera_Movemet::UP) {
			m_Position += m_WorldUp * velocity;
		}

		if (direction == Camera_Movemet::DOWN) {
			m_Position -= m_WorldUp * velocity;
		}

		UpdateCameraVectors();
	}

	void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
		xOffset *= m_Sensitivity;
		yOffset *= m_Sensitivity;

		m_Yaw += xOffset;
		m_Pitch += yOffset;

		if (constrainPitch) {
			if (m_Pitch > 89.0f) {
				m_Pitch = 89.0f;
			}

			if (m_Pitch < -89.0f) {
				m_Pitch = -89.0f;
			}
		}

		UpdateCameraVectors();
	}
}