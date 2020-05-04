/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: input.cpp
Purpose: input controller
Language: C++ Visual Studio 2017
Platform: Windows 10
Project: cs460_ivan.iglesias_1
Author: Ivan Iglesias, ivan.iglesias
Creation date: 9/26/2019
----------------------------------------------------------------------------------------------------------*/
#include "input.h"
#include <iostream>
void Input::Reset_Cam(GLFWwindow *window_input)
{
	cam->Position = vec3(66.0f, 91.0f, 35.0f);
	cam->Yaw = -95.0f;
	cam->Pitch = -72.0f;

	g_input.cam_free = g_input.cam->free_cam = true;

	double xpos, ypos;

	glfwGetCursorPos(window_input, &xpos, &ypos);

	lastX = (float)xpos;
	lastY = (float)ypos;

	float xoffset = static_cast<float>(xpos - lastX);
	float yoffset = static_cast<float>(lastY - ypos); // reversed since y-coordinates go from bottom to top

	lastX = static_cast<float>(xpos);
	lastY = static_cast<float>(ypos);

	cam->ProcessMouseMovement(xoffset, yoffset);
}
//#include "renderer.h"
Input::Input()
{
	lastX = SCR_WIDTH / 2.0f;
	lastY = SCR_HEIGHT / 2.0f;

	//cam = g_render.cam;
}
/**
* @brief 	function to process the input from glfw window
* @param	window		pointer to window
*/

void Input::processInput(GLFWwindow *window_input)
{
	//frame time
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	//esc input
	/*if (glfwGetKey(window_input, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window_input, true);*/

	if (glfwGetKey(window_input, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window_input, GLFW_KEY_R) == GLFW_PRESS)
	{
		//Reset Camera 
		//g_render.cam = Camera();
		//g_render.ObjectsToRender.clear();
		/*g_render.Lights.clear();*/
		g_render.CreateLightsV2();

		unsigned size = 20;
		g_render.particle_system.CreateParticles(size, glm::vec3(0, -9.8f, 0), 1.0f / 60.0f, 0.2f);		   		 	  		
	

	}

	if (glfwGetKey(window_input, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window_input, GLFW_KEY_Q) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window_input, true);
	}



	//Changing scene
	if (glfwGetKey(window_input, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window_input, GLFW_KEY_1) == GLFW_PRESS)
	{
		
		g_render.inverse_kinematic_method = 0;

	}

	if (glfwGetKey(window_input, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window_input, GLFW_KEY_2) == GLFW_PRESS)
	{
		g_render.inverse_kinematic_method = 1;

	}

	if (glfwGetKey(window_input, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window_input, GLFW_KEY_3) == GLFW_PRESS)
	{
		g_render.inverse_kinematic_method = 2;

	}

	/*if (glfwGetKey(window_input, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window_input, GLFW_KEY_4) == GLFW_PRESS)
	{
		Reset_Cam(window_input);

		g_render.actual_curve = 3;
	}

	if (glfwGetKey(window_input, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window_input, GLFW_KEY_5) == GLFW_PRESS)
	{
		//Reset Camera
		g_input.cam_free = g_input.cam->free_cam = false;
		
		cam->Position = glm::vec3(0.0f, 0.0f, 20.0f);
		cam->ChangeCamFree();

		g_render.ObjectsToRender[0]->TransformData.Position = vec3(0, 0, -10);
		g_render.ObjectsToRender[0]->TransformData.rot_quat = glm::quat(0,0,0,0);

		g_render.actual_curve = 4;
	}*/


	//Cam input 
	if (glfwGetKey(window_input, GLFW_KEY_W) == GLFW_PRESS)
	{
		if (cam_free)
		{
			if (glfwGetKey(window_input, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				cam->ProcessKeyboard(FORWARD, deltaTime * 5);
			else
				cam->ProcessKeyboard(FORWARD, deltaTime);
		}
		else
		{
			cam->RotateUp();
		}
	}

	if (glfwGetKey(window_input, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (cam_free)
		{
			if (glfwGetKey(window_input, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				cam->ProcessKeyboard(BACKWARD, deltaTime * 5);
			else
				cam->ProcessKeyboard(BACKWARD, deltaTime);
		}
		else
		{
			cam->RotateDown();
		}

	}

	if (glfwGetKey(window_input, GLFW_KEY_A) == GLFW_PRESS)
	{
		if (cam_free)
		{
			if (glfwGetKey(window_input, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				cam->ProcessKeyboard(LEFT, deltaTime * 5);
			else
				cam->ProcessKeyboard(LEFT, deltaTime);
		}
		else
		{
			cam->RotateLeft();
		}
	}

	if (glfwGetKey(window_input, GLFW_KEY_D) == GLFW_PRESS)
	{
		if (cam_free)
		{
			if (glfwGetKey(window_input, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				cam->ProcessKeyboard(RIGHT, deltaTime * 5);
			else
				cam->ProcessKeyboard(RIGHT, deltaTime);
		}
		else
		{
			cam->RotateRight();
		}
	}

	if (glfwGetKey(window_input, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (cam_free)
		{
			cam->Position.y += 0.5f;			
		}		
	}
	if (glfwGetKey(window_input, GLFW_KEY_E) == GLFW_PRESS)
	{
		if (cam_free)
		{
			cam->Position.y -= 0.5f;
		}
	}	

	if (glfwGetKey(window_input, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		if (cam_free)
		{
			//reset camara
			cam->Position = glm::vec3(0.0f, 0.0f, 0.0f);
			cam->ChangeCamFree();
			cam->free_cam = cam_free = false;
		}
		else
		{
			cam->free_cam = cam_free = true;
		}
	}

	//delete object
/*	if (glfwGetKey(window_input, GLFW_KEY_DELETE) == GLFW_PRESS && object_num != -1)
	{
		my_octree.delete_object(objects_render[object_num]);
		delete objects_render[object_num];
		objects_render.erase(objects_render.begin() + object_num);
		object_num = -1;
		prev = -1;
		selecting = -1;
	}
	*/
	//compute mouse position in world
	glfwGetCursorPos(window_input, &mouse_xpos, &mouse_ypos);
	auto pos_x_view = mouse_xpos / SCR_WIDTH;
	auto pos_y_view = mouse_ypos / SCR_HEIGHT;
	//NDC
	auto ndc_x = (pos_x_view * 2) - 1;
	auto ndc_y = (pos_y_view * 2) - 1;
	//std::cout << "=================================" << std::endl;
	//std::cout << "ndc_x x" << ndc_x << std::endl;
	//std::cout << "ndc_x y" << ndc_y << std::endl;
	//Viewport
	vec4 ndc_vec = { ndc_x , -ndc_y, 1, 1 };
	auto inv_view = glm::inverse(cam->ProjectionMatrix() * cam->GetViewMatrix());
	vec4 world_pos = inv_view * ndc_vec;
	//perspective division
	world_pos /= world_pos.w;

	if (rotate)
	{
		double xpos, ypos;

		glfwGetCursorPos(window_input, &xpos, &ypos);

		if (firstMouse)
		{
			lastX = (float)xpos;
			lastY = (float)ypos;
			firstMouse = false;
		}

		float xoffset = static_cast<float>(xpos - lastX);
		float yoffset = static_cast<float>(lastY - ypos); // reversed since y-coordinates go from bottom to top

		lastX = static_cast<float>(xpos);
		lastY = static_cast<float>(ypos);

		if(cam_free)
			cam->ProcessMouseMovement(xoffset, yoffset);
		else
			cam->ProcessMouseMovementSperical(xoffset, -yoffset);

	}


}


void Input::mouse_buttom_callback(GLFWwindow* window_mouse, int button, int action, int mods)
{

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		rotate = true;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstMouse = true;
		rotate = false;
	}

	/*if (render.imgui_hovered)
		return;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (render.selecting != -1 && render.selecting != render.object_num)
		{
			if(render.object_num != -1)
				render.objects_render[render.object_num]->color_aabb = render.default_aabb_color;
			render.object_num = render.selecting;
			render.objects_render[render.object_num]->color_aabb = { 0,0,1,1 };
		}
		else
		{

			if (render.object_num != -1)
				render.objects_render[render.object_num]->color_aabb = render.default_aabb_color;
			render.object_num = -1;
		}

	}*/


}