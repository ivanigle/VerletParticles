/**
* @file		camera.cpp
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
#include "camera.h"

#include "renderer.h"

//Function to create camara
/*Camera::Camera()
{
	phi = 0.0;
	theta = 0.0f;
	length = 20.0f;
	AngleMove = 0.1f;
}*/
//Function to return the viewport matrix
glm::mat4 Camera::ViewportMatrix()
{
	ViewVec = vec3(0.0, 0.0, -10.0) - Position;

	/*glm::vec3 right = glm::cross(ViewVec, glm::vec3(0.0f, 1.0f, 0.0f));
	UpVec = glm::cross(right, ViewVec);*/
		
	mat4 view = glm::lookAt(Position, Position + ViewVec, UpVec);
	//return perspect * view;
	return view;
}
//Camara projection matrix
glm::mat4 Camera::ProjectionMatrix()
{
	return glm::perspective(glm::pi<float>() * 0.5f, (float)g_render.SCR_WIDTH / (float)g_render.SCR_HEIGHT, 0.1f, far_plane);
}

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	//calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{	
	if (free_cam)
	{
		ViewVec = Front;
		return glm::lookAt(Position, Position + Front, Up);
	}
	else
	{
		UpVec = { 0,1,0 };
		Up = { 0,1,0 };
		ViewVec = g_render.ObjectsToRender[0]->TransformData.Position - Position;
		Front = glm::normalize(ViewVec);
		return glm::lookAt(Position, Position + ViewVec, Up);

	}
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction_cam, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (direction_cam == FORWARD)
		Position += Front * velocity;
	if (direction_cam == BACKWARD)
		Position -= Front * velocity;
	if (direction_cam == LEFT)
		Position -= Right * velocity;
	if (direction_cam == RIGHT)
		Position += Right * velocity;
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	if (!free_cam)
		return;

	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;


	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}
void Camera::ProcessMouseMovementSperical(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= MouseSensitivitySpherical;
	yoffset *= MouseSensitivitySpherical;

	theta += xoffset;	

	if ((phi + yoffset) < glm::half_pi<float>()&& (phi + yoffset)> - glm::half_pi<float>() )
		phi += yoffset;

	
	Rotate();

}
void Camera::RotateRight()
{
	
	theta -= (float)0.1;
	Rotate();

}
void Camera::Rotate()
{
	Position = glm::vec3(length * cos(phi) * cos(theta), length * sin(phi), length * cos(phi) * sin(theta)) + g_render.ObjectsToRender[0]->TransformData.Position;
}
void Camera::RotateLeft()
{
	theta += (float)0.1;
	Rotate();

}

void Camera::RotateUp()
{
	if(phi < (glm::half_pi<float>() - 0.1F))
		phi += (float)0.1;
	Rotate();


}

void Camera::RotateDown()
{
	if (phi > -(glm::half_pi<float>() - 0.1F))
		phi -= (float)0.1;
	Rotate();
	
}

void Camera::ChangeCamFree()
{
	glm::vec3 delta = Position - g_render.ObjectsToRender[0]->TransformData.Position;
	length = glm::length(delta);
	delta = glm::normalize(delta);

	if (!free_cam)
	{
		phi = asin(delta.y);
		theta = asin(delta.z);

		if (delta.x < 0.0f)
		{
			theta += 2 * (glm::pi<float>() * 0.5f - theta);
		}
	}
	else
	{
		delta = -delta;
		Pitch = glm::degrees(asin(delta.y));
		Yaw = glm::degrees(asin(delta.z));

		if (delta.x < 0.0f)
		{
			Yaw += 2 * (90.0f - Yaw);			
		}
		updateCameraVectors();
	}

}

void Camera::Reset()
{
	Position = glm::vec3(-87.0f, 40.0f, 58.0f);
	WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	Up = glm::vec3(0.0f, 1.0f, 0.0f);
	ViewVec = { 0,0,-1 };

	Yaw = YAW;
	Pitch = PITCH;

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
