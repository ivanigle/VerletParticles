/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GraphManager.h
Purpose: Functions for the Graphics Manager
Language: C++ Visual Studio 2015
Platform: Windows 7
Project: cs300_ivan.iglesias_1
Author: Ivan Iglesias, ivan.iglesias
Creation date: 5/21/2018
----------------------------------------------------------------------------------------------------------*/
#pragma once
#include "GLSLProgram.hpp"
#include "model.h"
#include "camera.h"
#include "light.h"
#include <GLFW/glfw3.h>
#include "interpolation.h"
#include "ik.h"
#include "verlet.h"
class Render
{

public:

	static Render& getInstance()
	{
		static Render    instance; // Guaranteed to be destroyed.
								   // Instantiated on first use.
		return instance;
	}
	Render(Render const&) = delete;
	void operator=(Render const&) = delete;


	void RenderWindow();
	Render();	

	//variables
	GLSLProgram geometry_program;
	GLSLProgram lighting_program;
	GLSLProgram lighting_program_lights;
	GLSLProgram picking_program;
	GLSLProgram line_program;
	GLSLProgram shader;
	GLSLProgram skyboxShader;



	GLFWwindow * window;
	//deffer
	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gAlbedoSpec;
	//temporal lights
	const unsigned int Total_Lights{ 2 };
	/*std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;*/

	//void CreateLigths();
	void CreateLightsV2();
	//void PassLightsToShader();

	std::vector<Model*> ObjectsToRender;
	std::vector<Light> Lights;

	Camera cam;

	void GenerateTexture();

	bool ActivateNorm = true;
	//movement 
	bool ActiveRotation = true;
	bool ActiveSin = true;

	bool DrawingNorm = true;

	bool UseTexture = true;
	bool UseDefaultColor = false;

	void InitializeTexture(const std::string & filename);
	void ConfigGBuffer();
	void MousePicking();
	unsigned int loadTexture(const char *path);
	unsigned int loadCubemap(std::vector<std::string> faces);
	//ImGui values
	int selected_light{ -1 };
	int selected_object{ -1 };
	glm::vec3 ambient_global{ 0.8f };
	//screen values 
	int SCR_WIDTH = 1280;
	int SCR_HEIGHT = 720;

	mat4 Viewport;
	mat4 Projection;

	Interpolation curves;
	int actual_curve{ 1 };

	int slerp_nlerp{ 0 };

	float speed_in_curve{ 30.0f };
	float t1{ 0.2f };
	float t2{ 0.8f };

	float prev_time{ 0.0f };

	bool follow_point{ false };

	float max_anim_spped{ 0.8f };

	unsigned int cubeVAO, cubeVBO;
	unsigned int skyboxVAO, skyboxVBO;

	unsigned int cubemapTexture;

	//2DBoneAnalytic

	float angle_bone1 = 0.0f;
	float angle_bone2 = 0.0f;
	float length_bone1 = 5.0f;
	float length_bone2 = 4.0f;
	bool solve_for_pos{ false };
	//CCD & FABRICK;
	std::vector<IK::Bone_3D_CCD> bones_ccd;
	Bone * root;
	int selected_bone_CCD_num{ -1 };
	Bone * selected_bone_CCD = nullptr;
	int num_iterations_per_frame{ 10 };
	int num_iterations_total{ 100 };
	int iteration_counter{ 0 };

	float dist_threshold{ 0.001f };
	int status_value{ 0 };
	vec3 prev_pos_target{ 0 };
	//InverseKinematic method
	int inverse_kinematic_method{ 0 };
	//Verlet
	ParticleSystem particle_system;

private:
	//Render() {}	
	GLuint texture;
	glm::vec4 GenTexture[6][6];
	float sinNum;
	float angleNum;
	float addingDiff;



};

static Render & g_render = Render::getInstance();