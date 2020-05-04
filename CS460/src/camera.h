/**
* @file		camera.h
* @date 	2/11/2019
* @author	Ivan Iglesias
* @par		Login: ivan.iglesias
* @par		Course: CS350
* @par		Assignment #2
* @brief 	Implementation of the camera
*
* Hours spent on this assignment: 20h
*
*/
#pragma once
#include <glm\glm.hpp>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 10.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
	//Camera();
	mat4 ViewportMatrix();
	mat4 ProjectionMatrix();
	vec3 Position = { 0,0,0 };
	vec3 ViewVec = {0,0,-10};
	vec3 UpVec = { 0,1,0 };
	
	// Camera Attributes
//	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float MouseSensitivitySpherical{ 0.01f };

	float Zoom;

	float far_plane{ 1000.f };

	float phi, theta, length;
	float angleXZ;
	float angleYZ;
	float AngleMove;
	void RotateRight();
	void Rotate();
	void RotateLeft();
	void RotateUp();
	void RotateDown();

	bool free_cam{ true };

	void  ChangeCamFree();
	
	void Reset();


	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;

		phi = 0.0;
		theta = 0.0f;
		length = 10.0f;
		AngleMove = 0.1f;

		updateCameraVectors();

		glm::vec3 delta = Position - ViewVec;
		length = glm::length(delta);
		delta = glm::normalize(delta);

		phi = asin(delta.y);
		theta = asin(delta.z);

		if (delta.x < 0.0f)
		{
			theta += 2 * (glm::pi<float>() * 0.5f - theta);
		}
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;

		phi = 0.0;
		theta = 0.0f;
		length = 10.0f;
		AngleMove = 0.1f;

		updateCameraVectors();

		glm::vec3 delta = Position - ViewVec;
		length = glm::length(delta);
		delta = glm::normalize(delta);

		phi = asin(delta.y);
		theta = asin(delta.z);

		if (delta.x < 0.0f)
		{
			theta += 2 * (glm::pi<float>() * 0.5f - theta);
		}

	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix();
	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	// Processes input received from a mouse 
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

	void ProcessMouseMovementSperical(float xoffset, float yoffset, bool constrainPitch = true);


private:
	glm::vec3 direction;	

	// Calculates the front vector from the Camera
	void updateCameraVectors();


};