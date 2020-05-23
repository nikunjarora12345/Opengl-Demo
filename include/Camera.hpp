#pragma once

#include <vector>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Renderer {
	enum class Camera_Movemet {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	class Camera {
	private:
		float m_Fov = 45.0f;
		float m_Speed = 6.0f;
		float m_Sensitivity = 0.25f;
		float m_Yaw;
		float m_Pitch;

		glm::vec3 m_Position;
		glm::vec3 m_Up;
		glm::vec3 m_Front;
		glm::vec3 m_Right;
		glm::vec3 m_WorldUp;

		void UpdateCameraVectors();

	public:
		Camera(
			glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
			float yaw = -90.0f,
			float pitch = 0.0f
		);

		Camera(
			float posX,
			float posY,
			float posZ,
			float upX,
			float upY,
			float upZ,
			float yaw,
			float pitch
		);

		void ProcessKeyboard(Camera_Movemet direction, float deltaTime);
		void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

		inline glm::mat4 GetViewMatrix() { return glm::lookAt(m_Position, m_Position + m_Front, m_Up); }

		inline float GetSpeed() { return m_Speed; }
		inline float GetSensitivity() { return m_Sensitivity; }
		inline float GetFov() { return m_Fov; }
		inline float GetYaw() { return m_Yaw; }
		inline float GetPitch() { return m_Pitch; }
		inline glm::vec3 GetPosition() { return m_Position; }
		inline glm::vec3 GetUp() { return m_Up; }
		inline glm::vec3 GetFront() { return m_Front; }

		inline void SetSpeed(float value) { m_Speed = value; }
		inline void SetSensitivity(float value) { m_Sensitivity = value; }
		inline void SetFov(float value) { m_Fov = value; }
		inline void SetYaw(float value) { m_Yaw = value; }
		inline void SetPitch(float value) { m_Pitch = value; }
		inline void SetPosition(glm::vec3 value) { m_Position = value; }
		inline void SetUp(glm::vec3 value) { m_Up = value; }
		inline void SetFront(glm::vec3 value) { m_Front = value; }
	};
}

