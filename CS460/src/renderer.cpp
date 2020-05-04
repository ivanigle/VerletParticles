/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: renderer.cpp
Purpose: Functions for the Graphics Manager
Language: C++ Visual Studio 2017
Platform: Windows 10
Project: cs460_ivan.iglesias_1
Author: Ivan Iglesias, ivan.iglesias
Creation date: 9/26/2019
----------------------------------------------------------------------------------------------------------*/
#include "renderer.h"
#include "glm\gtc\constants.hpp"
#include <iostream>
#include "glm/gtx/transform.hpp"
#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>
#include <stb_image/stb_image.h>
#include "ik.h"
#include <glm/gtx/string_cast.hpp>

bool just_once = true;
void Render::CreateLightsV2()
{
	Lights.clear();
	Lights.resize(particle_system.total_size + 1);
	for (unsigned int i = 0; i < particle_system.total_size; i++)
	{
		// calculate slightly random offsets
		Lights[i].TransformData.Position.x = 0;
		Lights[i].TransformData.Position.y = 0;
		Lights[i].TransformData.Position.z = 0;

		Lights[i].TransformData.Scale.x = 0.2f;
		Lights[i].TransformData.Scale.y = 0.2f;
		Lights[i].TransformData.Scale.z = 0.2f;

		Lights[i].TransformData.UpdateMatrices();
		// also calculate random color
		Lights[i].Color.r = ((rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
		Lights[i].Color.g = ((rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
		Lights[i].Color.b = ((rand() % 100) / 200.0f) + 0.5f; // between 0.5 and 1.0
		Lights[i].light_mesh.CreateSphere();
	}

	// calculate slightly random offsets
	Lights.back().TransformData.Position.x = 2.5f;
	Lights.back().TransformData.Position.y = -2.5f;
	Lights.back().TransformData.Position.z = 5.0f;

	Lights.back().TransformData.Scale.x = 1.0;
	Lights.back().TransformData.Scale.y = 1.0;
	Lights.back().TransformData.Scale.z = 1.0;

	Lights.back().TransformData.UpdateMatrices();
	// also calculate random color
	Lights.back().Color.r =1; // between 0.5 and 1.0
	Lights.back().Color.g = 1; // between 0.5 and 1.0
	Lights.back().Color.b = 1; // between 0.5 and 1.0
	Lights.back().light_mesh.CreateSphere(50,100, 1.0f);

	

	// shader config   
	lighting_program.use();
	lighting_program.setUniform("gPositionn", 0);
	lighting_program.setUniform("gNormal", 1);
	lighting_program.setUniform("gAlbedoSpec", 2);
}
void Render::ConfigGBuffer()
{


	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	//position buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, g_render.SCR_WIDTH, g_render.SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	//normal buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, g_render.SCR_WIDTH, g_render.SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	//specular buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, g_render.SCR_WIDTH, g_render.SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	//array of buffers
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, g_render.SCR_WIDTH, g_render.SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

}
//function to the control mouse picking
void Render::MousePicking()
{

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) && !ImGui::IsMouseHoveringAnyWindow() && !ImGuizmo::IsOver() && !ImGui::GetIO().WantCaptureMouse)
	{ 
		// Clear the screen in white
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		picking_program.use();

		glEnableVertexAttribArray(0);

		for (unsigned i = 0; i < ObjectsToRender.size(); i++)
		{
			//ObjectsToRender[i]->TransformData.UpdateMatrices();
			mat4 MVP = Projection * Viewport * ObjectsToRender[i]->TransformData.Model;
			picking_program.setUniform("MVP", MVP);

			vec4 color;
			//convert the integer position into a color
			color.r = ((i & 0x000000FF) >> 0) / 255.0f;
			color.g = ((i & 0x0000FF00) >> 8) / 255.0f;
			color.b = ((i & 0x00FF0000) >> 16) / 255.0f;
			color.a = 1.0f;

			picking_program.setUniform("PickingColor", color);

			for (unsigned j = 0; j < ObjectsToRender[i]->meshes.size(); j++)
				ObjectsToRender[i]->meshes[j].Draw_Picking(picking_program);
		}

		for (unsigned i = 0; i < Lights.size(); i++)
		{
			mat4 MVP = Projection * Viewport * Lights[i].TransformData.Model;
			picking_program.setUniform("MVP", MVP);

			vec4 color;
			int j = i + static_cast<unsigned>(ObjectsToRender.size());
			//convert the integer position into a color
			color.r = ((j & 0x000000FF) >> 0) / 255.0f;
			color.g = ((j & 0x0000FF00) >> 8) / 255.0f;
			color.b = ((j & 0x00FF0000) >> 16) / 255.0f;
			color.a = 1.0f;

			picking_program.setUniform("PickingColor", color);
			
			Lights[i].light_mesh.Draw_Picking(picking_program);
		}

		glDisableVertexAttribArray(0);

		glFlush();
		glFinish();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		double xpos, ypos;
		//getting cursor position
		glfwGetCursorPos(window, &xpos, &ypos);

		unsigned char data[4];

		glReadPixels(static_cast<GLint>(xpos), static_cast<GLint>(SCR_HEIGHT - ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

		//convert back to id 

		int pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;

		if (pickedID != 0x00ffffff)
		{		
			if ((ObjectsToRender.size() <= 0  && Lights.size()) || pickedID > (ObjectsToRender.size() - 1))
			{
				selected_light = pickedID - static_cast<int>(ObjectsToRender.size());
				selected_object = -1;
			}
			else
			{
				selected_object = pickedID;
				selected_light = -1;
			}
		}
		else
		{
			selected_object = -1;
			selected_light = -1;

		}
		


		//glfwSwapBuffers(window);
	}
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

unsigned quadVAO = 0;
unsigned quadVBO;
//function to print the quad with the image in ndc
void render_scree_quad()
{

	if (quadVAO == 0)
	{
		//position and textures
		float quad_vert[] = {			
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// set the buffers
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vert), &quad_vert, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Render::RenderWindow()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	GLenum error = glGetError();
	// Bind the glsl program and this object's VAO
	
	geometry_program.use();
	DrawingNorm = false;
	
	Viewport = cam.GetViewMatrix();
	Projection = cam.ProjectionMatrix();

	MousePicking();

	//Geometry pass
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	geometry_program.use();

	geometry_program.setUniform("view", Viewport);
	geometry_program.setUniform("projection", Projection);
	
	float actual_time = static_cast<float>(glfwGetTime());

	float dt = actual_time - prev_time;

	prev_time = actual_time;
	for (unsigned i = 0; i < ObjectsToRender.size(); i++)
	{	

		std::vector<glm::mat4> Transforms;


		ObjectsToRender[i]->bone_transformation(dt, Transforms);

		ObjectsToRender[i]->TransformData.UpdateMatrices();
		geometry_program.setUniform("model", ObjectsToRender[i]->TransformData.Model);
		geometry_program.setUniform("is_anim", ObjectsToRender[i]->m_pScene->mNumAnimations > 0);

		char buffer[25];

		for (uint i = 0; i < Transforms.size(); i++)
		{
			sprintf_s(buffer, "gBones[%d]", i);
			geometry_program.setUniform(buffer, Transforms[i]);
		}		

		for (unsigned j = 0; j < ObjectsToRender[i]->meshes.size(); j++)
		{
			ObjectsToRender[i]->meshes[j].Draw(geometry_program);
		}
		
	}

	int i = static_cast<int>(Lights.size()) - 1;
	auto pos_target = vec3(Lights[i].TransformData.Position.x, Lights[i].TransformData.Position.y, Lights[i].TransformData.Position.z);


	g_render.geometry_program.setUniform("is_anim", 0);
	/*int i = static_cast<int>(Lights.size()) - 1;
	auto pos_target = vec3(-Lights[i].TransformData.Position.x, Lights[i].TransformData.Position.y, -Lights[i].TransformData.Position.z);
	*/

	//set processing
	if (prev_pos_target != pos_target)
	{
		status_value = 2;
		prev_pos_target = pos_target;
		iteration_counter = 0;
	}
	
	/*if (inverse_kinematic_method == 1)
	{
		//CCD
		if (status_value != 0 && status_value != 1)
		{
			IK::CCD(root, pos_target, num_iterations_per_frame);
		}
		//root->M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		DrawBones(root);
	}
	else if (inverse_kinematic_method == 2)
	{
		//Fabrick
		//CCD
		if (status_value != 0 && status_value != 1)
			IK::FABRICK(root, pos_target, num_iterations_per_frame);
		//root->M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		DrawBones(root);
		
	}
	*/


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//2. Ligting pass
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	lighting_program.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

	//send light uniforms
	for (unsigned int i = 0; i < Total_Lights; i++)
	{
		char buffer[25];
		sprintf_s(buffer, "lights[%d].Position", i);
		lighting_program.setUniform(buffer, Lights[i].TransformData.Position);		

		sprintf_s(buffer, "lights[%d].Linear", i);
		lighting_program.setUniform(buffer, Lights[i].Linear);
		sprintf_s(buffer, "lights[%d].Quadratic", i);
		lighting_program.setUniform(buffer, Lights[i].Quadratic);
		// then calculate radius of light volume/sphere
		const float maxBrightness = std::fmaxf(std::fmaxf(Lights[i].Color.r, Lights[i].Color.g), Lights[i].Color.b);
		float radius = (-Lights[i].Linear + std::sqrt(Lights[i].Linear * Lights[i].Linear - 4 * Lights[i].Quadratic * (Lights[i].constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * Lights[i].Quadratic);
		sprintf_s(buffer, "lights[%d].Radius", i);
		lighting_program.setUniform(buffer, radius);
		sprintf_s(buffer, "lights[%d].diffuse", i);
		lighting_program.setUniform(buffer, Lights[i].diffuse);
		sprintf_s(buffer, "lights[%d].specular", i);
		lighting_program.setUniform(buffer, Lights[i].specular);

	}	
	lighting_program.setUniform("ambient_global", ambient_global);
	lighting_program.setUniform("viewPos", cam.Position);
	//Render the screen quad
	render_scree_quad();
	

	

	//  copy geometry's depth buffer to default framebuffer's depth buffer		
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
	glBlitFramebuffer(0, 0, g_render.SCR_WIDTH, g_render.SCR_HEIGHT, 0, 0, g_render.SCR_WIDTH, g_render.SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//Update Particle System
	particle_system.TimeStep();

	for (unsigned i = 0; i < particle_system.m_x.size(); i++)
	{
		Lights[i].TransformData.Position = particle_system.m_x[i];		
		Lights[i].TransformData.UpdateMatrices();
	}
	// render object lights	
	lighting_program_lights.use();
	lighting_program_lights.setUniform("projection", Projection);
	lighting_program_lights.setUniform("view", Viewport);



	for (unsigned int i = 0; i < Lights.size(); i++)
	{
		lighting_program_lights.setUniform("model", Lights[i].TransformData.Model);
		lighting_program_lights.setUniform("lightColor", Lights[i].Color);
		
		glBindVertexArray(Lights[i].light_mesh.VAO);		
		error = glGetError();
		// Draw
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(Lights[i].light_mesh.indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);		
	}
	//draw lines
	
	// draw scene as normal
/*	shader.use();
	glm::mat4 model = glm::mat4(1.0f);		
	shader.setUniform("model", model);
	shader.setUniform("view", Viewport);
	shader.setUniform("projection", Projection);
	shader.setUniform("cameraPos", cam.Position);
	// cubes
	glBindVertexArray(cubeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);*/

	// draw skybox as last
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader.use();
	auto view = glm::mat4(glm::mat3(Viewport)); // remove translation from the view matrix
	skyboxShader.setUniform("view", view);
	skyboxShader.setUniform("projection", Projection);
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default

	
	line_program.use();
	line_program.setUniform("projection", Projection);
	line_program.setUniform("view", Viewport);	

	unsigned j = 0;
	for (unsigned i = 0; i < particle_system.m_constraints.size(); i++ )
	{
		particle_system.points_grid[j] =  particle_system.m_x[particle_system.m_constraints[i].particleA];
		particle_system.points_grid[j + 1] = particle_system.m_x[particle_system.m_constraints[i].particleB];
		j += 2;
	}

	particle_system.bind_buffers();
	particle_system.draw_lines();
	

	// Unbind
	glBindVertexArray(0);
	glUseProgram(0);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int Render::loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

//loads a cubemap texture from 6 individual texture faces
// -------------------------------------------------------
unsigned int Render::loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

Render::Render()
{
	//Image
	sinNum = 0.0f; 
	angleNum = 0.0f;
	//CreateLigths();
	addingDiff = glm::two_pi<float>() / 8;
	
}


