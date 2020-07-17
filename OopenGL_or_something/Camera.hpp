#pragma once

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "glad.h"

enum CameraMovement {
	FORWARD,
	BACKWARDS,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5;
const float SENSITIVITY = 0.05f;
const float ZOOM = 70.0f;
const float BOOST_MULTIPLIER = 2.0f;

class Camera {
public:
	static glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	// Euler angles
	float Yaw;
	float Pitch;

	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
	       float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
	                                               MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
		Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 getViewMatrix() {
		return glm::lookAt(Position, Position + Front, Up);
	}

	void processKeyboard(CameraMovement movement, float deltaTime, bool shift) {
		float velocity = MovementSpeed * deltaTime;
		if (shift)
			velocity *= BOOST_MULTIPLIER;
		

		if (movement == FORWARD) {
			glm::vec3 d = Front * velocity;
			Position = Position + d;
		}

		if (movement == BACKWARDS) {
			glm::vec3 d = Front * velocity;
			Position = Position - d;
		}
			
		if (movement == RIGHT) {
			glm::vec3 d = Right * velocity;
			Position = Position + d;
		}
		
		if (movement == LEFT) {
			glm::vec3 d = Right * velocity;
			Position = Position - d;
		}

		if (movement == UP)
			Position += Up * velocity;

		if (movement == DOWN)
			Position -= Up * velocity;

	}

	void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		if (constrainPitch) {
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		updateCameraVectors();
	}

private:
	void updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};


#endif
glm::vec3 Camera::Position = glm::vec3(0.0f, 0.0f, 3.0f);