/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: main.cpp
Purpose: main functions of the framework
Language: C++ Visual Studio 2017
Platform: Windows 10
Project: cs460_ivan.iglesias_1
Author: Ivan Iglesias, ivan.iglesias
Creation date: 9/26/2019
----------------------------------------------------------------------------------------------------------*/
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <fstream>
#include <string>
#include <iostream>
#include "GLSLProgram.hpp"
#include <vector>
#include "renderer.h"
#include "input.h"
#include "model.h"
#include <sstream>
#include <nlohmann/json.hpp>
#include <imgui/ImGuizmo.h>
#include <imgui/imgui_internal.h>
#include "interpolation.h"
#include "verlet.h"


using json = nlohmann::json;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_buttom_callback(GLFWwindow* window, int button, int action, int mods);

void setWindowFPS(GLFWwindow* win);
void read_Json();


GLFWwindow* window;

std::string m_win_name;
ImGuiWindowFlags m_flags;
/**
* @brief 	function to init imgui
* @return	if failed or succes
*/
bool InitImGui()
{
	//call imgui functions
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();
	if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
		return false;

	g_render.window = window;

	const char * glsl_version = "#version 130";

	if (!ImGui_ImplOpenGL3_Init(glsl_version))
		return false;

	m_win_name = "Editor Render";
	//NOt moving windons!!!
	//m_flags = ImGuiWindowFlags_NoMove;

	return true;
}
/**
* @brief 	function to render imgui
*/
void EditTransform(const float *cameraView, float *cameraProjection, float* matrix)
{
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	/*if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;*/
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation, 3);
	/*ImGui::InputFloat3("Rt", matrixRotation, 3);
	ImGui::InputFloat3("Sc", matrixScale, 3);*/


	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

	/*if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}*/
	/*if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;*/
	/*ImGui::Checkbox("", &useSnap);
	ImGui::SameLine();

	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		ImGui::InputFloat3("Snap", &snap[0]);
		break;
	case ImGuizmo::ROTATE:
		ImGui::InputFloat("Angle Snap", &snap[0]);
		break;
	case ImGuizmo::SCALE:
		ImGui::InputFloat("Scale Snap", &snap[0]);
		break;
	}
	ImGui::Checkbox("Bound Sizing", &boundSizing);*/
	/*if (boundSizing)
	{
		ImGui::PushID(3);
		ImGui::Checkbox("", &boundSizingSnap);
		ImGui::SameLine();
		ImGui::InputFloat3("Snap", boundsSnap);
		ImGui::PopID();
	}*/

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

	ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);

	//Set values to object
	//g_render.ObjectsToRender[g_render.selected_object]->TransformData.Position = vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
	//g_render.ObjectsToRender[g_render.selected_object]->TransformData.Rotate = vec3(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
	//g_render.ObjectsToRender[g_render.selected_object]->TransformData.Scale = vec3(matrixScale[0], matrixScale[1], matrixScale[2]);

}
void DrawCurve(float & t1, float & t2)
{
	const ImGuiStyle& Style = ImGui::GetStyle();
	const ImGuiIO& IO = ImGui::GetIO();
	ImDrawList* DrawList = ImGui::GetWindowDrawList();
	ImGuiWindow* Window = ImGui::GetCurrentWindow();
	if (Window->SkipItems)
		return ;

	// header and spacing
	glm::vec2 time{ t1, t2 };
	ImGui::DragFloat2("Time Control", &time.x, 0.01f, 0, 1.0f);
	if (time.x < time.y)
	{
		t1 = time.x;
		t2 = time.y;
	}

	ImGui::Dummy(ImVec2(0, 3));


	// prepare canvas
	const float avail = ImGui::GetContentRegionAvailWidth();
	ImVec2 Canvas(avail, avail);

	ImRect bb(Window->DC.CursorPos, ImVec2(Window->DC.CursorPos.x + Canvas.x, Window->DC.CursorPos.y + Canvas.y));
	ImGui::ItemSize(bb);
	if (!ImGui::ItemAdd(bb, NULL))
		return ;

	const ImGuiID id = Window->GetID("Time Control");

	ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg, 1), true, Style.FrameRounding);

	int hovered = ImGui::IsItemHovered(); // IsItemDragged() ?

	// background grid
	for (float i = 0; i <= Canvas.x; i += (Canvas.x / 4)) {
		DrawList->AddLine(
			ImVec2(bb.Min.x + i, bb.Min.y),
			ImVec2(bb.Min.x + i, bb.Max.y),
			ImGui::GetColorU32(ImGuiCol_TextDisabled));
	}
	for (float i = 0; i <= Canvas.y; i += (Canvas.y / 4)) {
		DrawList->AddLine(
			ImVec2(bb.Min.x, bb.Min.y + i),
			ImVec2(bb.Max.x, bb.Min.y + i),
			ImGui::GetColorU32(ImGuiCol_TextDisabled));
	}

	// eval curve
	const unsigned num_points = 100;
	ImVec2 results[num_points];
	for (unsigned i = 0; i < num_points; i++)
	{
		float t = i / static_cast<float>(num_points - 1);
		results[i].x = t;
		results[i].y = Interpolation::eval_ease(t, t1, t2);
			
	}

	for (unsigned i = 0; i < num_points - 1; i++)
	{
		ImVec2 p = results[i];
		ImVec2 q = results[i + 1];

		ImVec2 p_sceen = ImVec2(p.x * (bb.Max.x - bb.Min.x) + bb.Min.x, (1 - p.y) * (bb.Max.y - bb.Min.y) + bb.Min.y);
		ImVec2 q_sceen = ImVec2(q.x * (bb.Max.x - bb.Min.x) + bb.Min.x, (1 - q.y) * (bb.Max.y - bb.Min.y) + bb.Min.y);
		if(p.x < t1)
			DrawList->AddLine(p_sceen, q_sceen, ImColor(1.0f, 0.0f, 0.0f), 4.0f);
		else if(p.x < t2)
			DrawList->AddLine(p_sceen, q_sceen, ImColor(0.0f, 1.0f, 0.0f), 4.0f);
		else
			DrawList->AddLine(p_sceen, q_sceen, ImColor(0.0f, 0.0f, 1.0f), 4.0f);
			

	}



}

void render_imgui()
{
	ImGui::Begin(m_win_name.c_str(), nullptr, m_flags);
	


	/*if(ImGui::Button("Reset Scene"))
	{
		//Reset Camera 
		g_render.cam.Reset();
		g_render.ObjectsToRender.clear();
		g_render.Lights.clear();
		read_Json();
		g_render.CreateLightsV2();

	}
	if (ImGui::Button("Close Window"))
	{
		glfwSetWindowShouldClose(window, true);
	}*/

	//ImGui::ListBox("Central Object", &g_render.selected_light, listbox_items.c_str(), listbox_items.size(), listbox_items.size());
	

	/*const char* items[] = { "Bezier", "Catmull", "Hermite", "Linear", "No Curve"};
	
	ImGui::Combo("combo", &g_render.actual_curve, items, IM_ARRAYSIZE(items));*/

	
	//DrawCurve(g_render.t1, g_render.t2);

	/*string text_curve;

	
		switch (g_render.actual_curve)
		{
		case 0:
			text_curve = "Scene Bezier";
			break;
		case 1:
			text_curve = "Scene Catmull";
			break;
		case 2:
			text_curve = "Scene Hermite";
			break;
		case 3:
			text_curve = "Scene Linear";
			break;
		case 4:
			text_curve = "Scene Animation";
			break;		
		default:
			break;
		}*/
	//ImGui::Text(text_curve.c_str());

	/*const char* items[] = { "Slerp", "Nlerp"};

	ImGui::Combo("Interpolation", &g_render.slerp_nlerp, items, IM_ARRAYSIZE(items));

	ImGui::DragFloat("Speed", &g_render.speed_in_curve, 0.01f, 0.0f, 1000.f);

	ImGui::DragFloat("AnimSpeed", &g_render.max_anim_spped, 0.01f, 0.0f, 1000.f);*/


	//ImGui::Checkbox("Free Camera", &g_input.cam->free_cam);

//	ImGui::Checkbox("Look At Target", &g_render.follow_point);

	/*const char* items[] = { "2DAnlytic", "CCD", "Fabrick" };

	ImGui::Combo("Method", &g_render.inverse_kinematic_method, items, IM_ARRAYSIZE(items));

	if (g_render.inverse_kinematic_method == 1 || g_render.inverse_kinematic_method == 2)
	{
		ImGui::DragInt("IterationsPerFram", &g_render.num_iterations_per_frame);
		ImGui::DragInt("IterationsTotal", &g_render.num_iterations_total);
		
		ImGui::DragFloat("Distance", &g_render.dist_threshold, 0.0001f, 0.000001f, 1000);
		// Text
		ImGui::Text("Status :"); ImGui::SameLine();
		if(g_render.status_value == 0)
			ImGui::TextColored(ImVec4(0, 1, 0, 1), "Success");
		else if(g_render.status_value == 1)
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failure");
		else 
			ImGui::TextColored(ImVec4(0, 0, 1, 1), "Processing");

		if (ImGui::TreeNode("BONES"))
		{
			std::string item_list;

			unsigned i = 0;
			Bone * currentBone = g_render.root->getEndEffector();
			while (currentBone->parent != NULL)
			{
				item_list += ("Bone" + std::to_string(i) + '\0');
				i++;
				currentBone = currentBone->parent;

			}

			ImGui::Combo("Bone", &g_render.selected_bone_CCD_num, item_list.c_str());

			i = 0;
			currentBone = g_render.root->getEndEffector();
			while (currentBone->parent != NULL)
			{
				if (g_render.selected_bone_CCD_num != -1 && g_render.selected_bone_CCD_num == i)
				{
					g_render.selected_bone_CCD = currentBone;
					break;
				}
				i++;
				currentBone = currentBone->parent;
			}

			if (ImGui::Button("Add Bone"))
			{
				g_render.root->getEndEffector()->add(new Bone(1));
			}

			if (g_render.selected_bone_CCD_num != -1)
			{
				ImGui::DragFloat("LenghtBone", &g_render.selected_bone_CCD->length, 0.001f, 0.0001f, 1000);

				if (ImGui::Button("Delete Bone"))
				{
					g_render.selected_bone_CCD_num = -1;
					g_render.selected_bone_CCD->detach();
					g_render.selected_bone_CCD = nullptr;
				}

			}

			ImGui::TreePop();

		}
	}
	else if (g_render.inverse_kinematic_method == 0)
	{
		ImGui::DragFloat("Length_Bone1", &g_render.length_bone1, 0.001f, 0.0001f, 1000);
		ImGui::DragFloat("Length_Bone2", &g_render.length_bone2, 0.001f, 0.0001f, 1000);
		ImGui::Checkbox("Solve Positive", &g_render.solve_for_pos);
		
	}


	
	if (g_input.cam_free != g_input.cam->free_cam)
	{
		g_input.cam_free = g_input.cam->free_cam;
		g_input.cam->ChangeCamFree();
	}*/

	static bool display_break[]{ false, false, false };

	/*if (ImGui::TreeNode("Ilumination"))
	{
		if (ImGui::TreeNode("SceneParameters"))
		{
			ImGui::PushID(0);
			ImGui::Checkbox("", display_break);
			ImGui::SameLine();
			if (display_break[0])
				ImGui::DragFloat3("Global_Ambient", &g_render.ambient_global.x, 0.001f, 0.0f, 1.0f);
			else if (ImGui::DragFloat("Global_Ambient", &g_render.ambient_global.x, 0.001f, 0.0f, 1.0f))
				g_render.ambient_global.z = g_render.ambient_global.y = g_render.ambient_global.x;
			ImGui::PopID();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Lights"))
		{
			std::string item_list;
			for (unsigned i = 0; i < g_render.Lights.size(); i++)
			{
				item_list += ("light" + std::to_string(i) + '\0');
			}

			ImGui::Combo("Lights", &g_render.selected_light, item_list.c_str());

			if (g_render.selected_light != -1)
			{
				//if this returns true the objects has been move
				ImGui::DragFloat3("Pos", &g_render.Lights[g_render.selected_light].TransformData.Position.x, 0.01f);
				//Information of the light selected	
				ImGui::PushID(1);
				ImGui::Checkbox("", display_break + 1);
				ImGui::SameLine();
				if (display_break[1])
					ImGui::DragFloat3("Diffuse", &g_render.Lights[g_render.selected_light].diffuse.x, 0.001f, 0.0001f, 1);
				else if (ImGui::DragFloat("Diffuse", &g_render.Lights[g_render.selected_light].diffuse.x, 0.001f, 0.0001f, 1))
					g_render.Lights[g_render.selected_light].diffuse.z = g_render.Lights[g_render.selected_light].diffuse.y = g_render.Lights[g_render.selected_light].diffuse.x;
				ImGui::PopID();

				ImGui::PushID(2);
				ImGui::Checkbox("", display_break + 2);
				ImGui::SameLine();
				if (display_break[2])
					ImGui::DragFloat3("Specular", &g_render.Lights[g_render.selected_light].specular.x, 0.001f, 0.0001f, 1);
				else if (ImGui::DragFloat("Specular", &g_render.Lights[g_render.selected_light].specular.x, 0.001f, 0.0001f, 1))
					g_render.Lights[g_render.selected_light].specular.z = g_render.Lights[g_render.selected_light].specular.y = g_render.Lights[g_render.selected_light].specular.x;
				ImGui::PopID();

				ImGui::DragFloat("LinarAtt", &g_render.Lights[g_render.selected_light].Linear, 0.001f, 0.0001f, 1000);
				ImGui::DragFloat("QuadraticAtt", &g_render.Lights[g_render.selected_light].Quadratic, 0.001f, 0.0001f, 1000);
				ImGui::DragFloat("ConstantAtt", &g_render.Lights[g_render.selected_light].constant, 0.001f, 0.0001f, 1000);


			}

			ImGui::TreePop();

		}
		ImGui::TreePop();

	}*/

	ImGui::DragFloat3("Gravity Force", &g_render.particle_system.m_vGravity.x, 0.1f);
	ImGui::DragFloat3("Wind Force", &g_render.particle_system.m_vWind.x, 0.1f);


	if (g_render.selected_object != -1 || g_render.selected_light != -1)
	{

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::BeginFrame();

		if (g_render.selected_object != -1)
			EditTransform(&g_render.Viewport[0].x, &g_render.Projection[0].x, &g_render.ObjectsToRender[g_render.selected_object]->TransformData.Model[0].x);
		else
		{
			if (g_render.selected_light < (g_render.Lights.size() - 1))
			{
				EditTransform(&g_render.Viewport[0].x, &g_render.Projection[0].x, &g_render.Lights[g_render.selected_light].TransformData.Model[0].x);
				float matrixTranslation[3], matrixRotation[3], matrixScale[3];
				ImGuizmo::DecomposeMatrixToComponents(&g_render.Lights[g_render.selected_light].TransformData.Model[0].x, matrixTranslation, matrixRotation, matrixScale);

				//Set values to object
				g_render.particle_system.m_x[g_render.selected_light] = glm::vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);

				//ImGui::Checkbox("Free Camera", &g_input.cam->free_cam);
				bool fixed = g_render.particle_system.fixed[g_render.selected_light];
				ImGui::Checkbox("Fixed", &fixed);
				g_render.particle_system.fixed[g_render.selected_light] = fixed;
			}
			else if (g_render.selected_light == (g_render.Lights.size() - 1))
			{
				EditTransform(&g_render.Viewport[0].x, &g_render.Projection[0].x, &g_render.Lights[g_render.selected_light].TransformData.Model[0].x);
				float matrixTranslation[3], matrixRotation[3], matrixScale[3];
				ImGuizmo::DecomposeMatrixToComponents(&g_render.Lights[g_render.selected_light].TransformData.Model[0].x, matrixTranslation, matrixRotation, matrixScale);
				g_render.Lights.back().TransformData.Position = glm::vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
			}
			

		}

		/*ImGui::Separator();

		ImGui::DragFloat3("Position", &g_render.ObjectsToRender[g_render.selected_object]->TransformData.Position.x, 0.01f);
		ImGui::DragFloat3("Rotate", &g_render.ObjectsToRender[g_render.selected_object]->TransformData.Rotate.x, 1.0f, 0.0001f, 1000.f);
		ImGui::DragFloat3("Scale", &g_render.ObjectsToRender[g_render.selected_object]->TransformData.Scale.x, 0.01f, 0.0001f, 1000.f);*/
	}


	//}*/

	/*if (ImGui::TreeNode("FrameBuffers"))
	{
		ImGui::Image((void*)(intptr_t)g_render.gPosition, ImVec2(256, 256), ImVec2{ 0.f, 1.f }, ImVec2{ 1.f, 0.f });
		ImGui::Image((void*)(intptr_t)g_render.gNormal, ImVec2(256, 256), ImVec2{ 0.f, 1.f }, ImVec2{ 1.f, 0.f });
		ImGui::Image((void*)(intptr_t)g_render.gAlbedoSpec, ImVec2(256, 256), ImVec2{ 0.f, 1.f }, ImVec2{ 1.f, 0.f });

		ImGui::TreePop();
	}*/

	
	ImGui::End();
}
/**
* @brief 	function to update imgui
*/
void update_imgui()
{
	//call imgui funcions on update
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow();
	render_imgui();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
/**
* @brief 	function to shutdown imgui
*/
void shutdown_imgui()
{
	//shutdown imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
/**
* @brief 	function to create window
*/
void WindowCreation()
{
	// glfw window creation
	window = glfwCreateWindow(g_render.SCR_WIDTH, g_render.SCR_HEIGHT, "Framework", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_buttom_callback);

}
/**
* @brief 	function to init glfw
*/
void InitGlfw()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	WindowCreation();
}
/**
* @brief 	function to load opengl
*/
void LoadOpenGL()
{
	// glad: load all OpenGL function pointers	
	if (gl3wInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
	}
}
/**
* @brief 	function to compile shaders
*/
void BuildAndCompileShaders()
{
	glEnable(GL_DEPTH_TEST);
	//std::vector<GLuint> shaderList;

	g_render.geometry_program.compileShaderFromFile("shaders/FragmentShader.frag", GLSLShader::GLSLShaderType::FRAGMENT);
	g_render.geometry_program.compileShaderFromFile("shaders/VertexShader.vert", GLSLShader::GLSLShaderType::VERTEX);
	g_render.geometry_program.link();

	g_render.lighting_program.compileShaderFromFile("shaders/deferred_fragment.frag", GLSLShader::GLSLShaderType::FRAGMENT);
	g_render.lighting_program.compileShaderFromFile("shaders/deferred_vertex.vert", GLSLShader::GLSLShaderType::VERTEX);
	g_render.lighting_program.link();

	g_render.lighting_program_lights.compileShaderFromFile("shaders/deferred_light_fragment.frag", GLSLShader::GLSLShaderType::FRAGMENT);
	g_render.lighting_program_lights.compileShaderFromFile("shaders/deferred_light_vertex.vert", GLSLShader::GLSLShaderType::VERTEX);
	g_render.lighting_program_lights.link();

	g_render.picking_program.compileShaderFromFile("shaders/picking_fragment.frag", GLSLShader::GLSLShaderType::FRAGMENT);
	g_render.picking_program.compileShaderFromFile("shaders/picking_vertex.vert", GLSLShader::GLSLShaderType::VERTEX);
	g_render.picking_program.link();

	g_render.line_program.compileShaderFromFile("shaders/line_draw.frag", GLSLShader::GLSLShaderType::FRAGMENT);
	g_render.line_program.compileShaderFromFile("shaders/line_draw.vert", GLSLShader::GLSLShaderType::VERTEX);
	g_render.line_program.link();
	
	g_render.shader.compileShaderFromFile("shaders/cubemaps.fs", GLSLShader::GLSLShaderType::FRAGMENT);
	g_render.shader.compileShaderFromFile("shaders/cubemaps.vs", GLSLShader::GLSLShaderType::VERTEX);
	g_render.shader.link();

	g_render.skyboxShader.compileShaderFromFile("shaders/skybox.fs", GLSLShader::GLSLShaderType::FRAGMENT);
	g_render.skyboxShader.compileShaderFromFile("shaders/skybox.vs", GLSLShader::GLSLShaderType::VERTEX);
	g_render.skyboxShader.link();


	g_render.ConfigGBuffer();

}

std::string path("models/sponza/meshes/sponza.obj");

/**
* @brief 	function to load all the meshes
*/
void load_all_meshes()
{
	Model * nano = new Model(path.c_str());

	//quad.MeshData.CreateCube();
	g_render.ObjectsToRender.push_back(nano);
	//g_render.GenerateTexture();
}

void read_curves()
{
	g_render.curves.ReadFile("curves/bezier.txt");
	g_render.curves.ReadFile("curves/catmull2.txt");
	g_render.curves.ReadFile("curves/hermite.txt");
	g_render.curves.ReadFile("curves/line.txt");


}
void read_Json()
{
	std::ifstream ifs("scenes/scene.json");	

	json j = json::parse(ifs);

	json object = j["objects"];

	// iterate the array
	for (json::iterator it = object.begin(); it != object.end(); ++it)
	{
		//new object
		std::string mesh_j = (*it)["mesh"];
		json translation = (*it)["translation"];
		json rotate = (*it)["rotate"];
		json scale = (*it)["scale"];
		Model * obj = new Model(mesh_j.c_str());
		obj->TransformData.Position = glm::vec3(translation["x"], translation["y"], translation["z"]);
		obj->TransformData.Scale = glm::vec3(scale["x"], scale["y"], scale["z"]);
		obj->TransformData.Rotate = glm::vec3(rotate["x"], rotate["y"], rotate["z"]);

		obj->TransformData.UpdateMatrices();
		g_render.ObjectsToRender.push_back(obj);
	}

	//Create bone hierarchy
	unsigned num_bones = 4;
	int constant_length = 10;

	g_render.bones_ccd.resize(num_bones);
	for (unsigned i = 0; i < num_bones; i++)
	{
		g_render.bones_ccd[i].pos = glm::vec3(constant_length * i, 0, 0);

		json::iterator it = object.begin();
		//new object
		std::string mesh_j = (*it)["mesh"];

		g_render.bones_ccd[i].bone = new Model(mesh_j.c_str());
		g_render.bones_ccd[i].bone->TransformData.Position = glm::vec3(constant_length * i, 0, 0);
		g_render.bones_ccd[i].bone->TransformData.Scale = glm::vec3(constant_length, 1, 1);
		g_render.bones_ccd[i].bone->TransformData.UpdateMatrices();
	}

	g_render.root = new Bone(1.0f);

	g_render.root->add(new Bone(1))
		->add(new Bone(1))
		->add(new Bone(1))
		->add(new Bone(1))
		->add(new Bone(1))
		->add(new Bone(1))
		->add(new Bone(1));
	
}
void CheckWindowSize()
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if (width != g_render.SCR_WIDTH || g_render.SCR_HEIGHT != height)
	{
		g_input.SCR_HEIGHT = g_render.SCR_HEIGHT = height;
		g_input.SCR_WIDTH = g_render.SCR_WIDTH = width;
		g_render.ConfigGBuffer();
	}

}
void cube_map()
{
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float cubeVertices[] = {
		// positions          // normals
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// cube VAO
	glGenVertexArrays(1, &g_render.cubeVAO);
	glGenBuffers(1, &g_render.cubeVBO);
	glBindVertexArray(g_render.cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, g_render.cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	// skybox VAO
	glGenVertexArrays(1, &g_render.skyboxVAO);
	glGenBuffers(1, &g_render.skyboxVBO);
	glBindVertexArray(g_render.skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, g_render.skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// load textures
	// -------------
	std::vector<std::string> faces;
	
	faces.push_back("skybox/textures/right.jpg");
	faces.push_back("skybox/textures/left.jpg");
	faces.push_back("skybox/textures/top.jpg");
	faces.push_back("skybox/textures/bottom.jpg");
	faces.push_back("skybox/textures/front.jpg");
	faces.push_back("skybox/textures/back.jpg");
	
	g_render.cubemapTexture = g_render.loadCubemap(faces);

	// shader configuration
	// --------------------
	g_render.shader.use();
	g_render.shader.setUniform("skybox", 0);

	g_render.skyboxShader.use();
	g_render.skyboxShader.setUniform("skybox", 0);
}
void create_ParticleSystem()
{
	unsigned size = 20;
	g_render.particle_system.CreateParticles(size, glm::vec3(0,-9.8f,0), 1.0f/60.0f, 0.2f);
	g_render.CreateLightsV2();
}

int main()
{
	//cal init and load functions
	InitGlfw();
	LoadOpenGL();
	InitImGui();
	BuildAndCompileShaders();
	//set initial camera
	g_input.cam = &g_render.cam;

	read_Json();
	read_curves();
	cube_map();
	create_ParticleSystem();

	//load_all_meshes();
	//Read JsonObjects
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		//Check size window
		CheckWindowSize();
		// input
		g_input.processInput(window);	
		//render scene
		g_render.RenderWindow();
		

		update_imgui();
		// glfw: swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
		setWindowFPS(window);
	}
	return 0;
}

/**
* @brief 	function to change glfw window size
* @param	window		pointer to window
*/
void framebuffer_size_callback(GLFWwindow* window_framebuffer, int width, int height)
{
	//change viewport
	glViewport(0, 0, width, height);
}

void mouse_buttom_callback(GLFWwindow* window_mouse, int button, int action, int mods)
{

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		g_input.rotate = true;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		g_input.firstMouse = true;
		g_input.rotate = false;
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

int nbFrames = 0;
int lastTime = 0;
void setWindowFPS(GLFWwindow* win)
{
	// Measure speed
	double currentTime = glfwGetTime();
	double delta = currentTime - lastTime;
	nbFrames++;
	if (delta >= 1.0) { 
		double fps = double(nbFrames) / delta;

		std::stringstream ss;
		ss << " [" << fps << " FPS]";

		glfwSetWindowTitle(win, ss.str().c_str());

		nbFrames = 0;
		lastTime = static_cast<int>(currentTime);
	}
}
