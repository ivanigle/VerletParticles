/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: input.h
Purpose: input controller of the framework
Language: C++ Visual Studio 2017
Platform: Windows 10
Project: cs460_ivan.iglesias_1
Author: Ivan Iglesias, ivan.iglesias
Creation date: 9/26/2019
----------------------------------------------------------------------------------------------------------*/
#pragma once
//#include <GLFW/glfw3.h>
#include "camera.h"
#include "renderer.h"

class Input
{
public:
	static Input& getInstance()
	{
		static Input    instance;							  
		return instance;
	}
	Input(Input const&) = delete;
	void operator=(Input const&) = delete;
	
	Camera * cam;

	void processInput(GLFWwindow *window_input);
	void mouse_buttom_callback(GLFWwindow* window_mouse, int button, int action, int mods);

	bool rotate{ false };
	bool firstMouse = true;	
	
	int SCR_WIDTH = 1280;
	int SCR_HEIGHT = 720;

	void Reset_Cam(GLFWwindow *window_input);
	bool cam_free{ true };

private:
	Input();

	double mouse_xpos, mouse_ypos;
	vec3 mouse_pos;
	// timing
	float deltaTime = 0.0f;	// time between current frame and last frame
	float lastFrame = 0.0f;



	float lastX;
	float lastY;

};

static Input & g_input = Input::getInstance();



