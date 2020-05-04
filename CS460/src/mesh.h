/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.h
Purpose: Functions that define the shapes
Language: C++ Visual Studio 2015
Platform: Windows 7
Project: cs300_ivan.iglesias_1
Author: Ivan Iglesias, ivan.iglesias
Creation date: 5/21/2018
----------------------------------------------------------------------------------------------------------*/
#pragma once
#include <GL/gl3w.h>
#include <GL/GL.h>
//#include <SDL2/SDL.h>
#include <vector>
#include <glm\glm.hpp>
#include "GLSLProgram.hpp"


class Texture
{
public:
	unsigned int id;
	std::string type;
	std::string path;

};

struct VertexBuffer
{
	VertexBuffer()	{}

	VertexBuffer(size_t s)
	{
		Position.resize(s);
		Normal.resize(s);
		TexCoords.resize(s);
		Tangent.resize(s);
		Bitangent.resize(s);
		Bones_Weights.resize(s);
		Bones.resize(s);
	}
	std::vector<glm::vec3> Position;
	std::vector<glm::vec3> Normal;
	std::vector<glm::vec2> TexCoords;
	std::vector<glm::vec3> Tangent;
	std::vector<glm::vec3> Bitangent;
	std::vector<glm::vec4> Bones_Weights;
	std::vector<glm::ivec4> Bones;

};

#define ZERO_MEM(a) memset(a, 0, sizeof(a))

struct VertexBoneData
{
	unsigned int IDs[4];
	float Weights[4];

	VertexBoneData()
	{
		Reset();
	};

	void Reset()
	{
		ZERO_MEM(IDs);
		ZERO_MEM(Weights);
	}
	void AddBoneData(unsigned int BoneID, float Weight);
};
struct MatProperties
{
	//Material
	glm::vec3 K_Ambient, K_Diffuse, K_Specular;
	float NS;

};

class Mesh
{
public:
	//void CreateMesh();

	//For not applying shader
	bool NotShader = true;	

	bool DrawObjNorms = true;
	Mesh();
	/*void CreateCube();
	void Increase();
	void BindVertPos();
	void CreateCylinder();
	void CreateCone();*/
	void SetBuffers();
	void CreateSphere(int rings = 5, int slices = 10, float radius = 0.5f);
	/*void ComputeFaceNormals();
	void ComputeVertexNormals();
	glm::vec3 ComputeCenterTrig(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);*/

	

	//For assimp
	GLuint VAO;
	VertexBuffer vertices;
	//std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	
	std::vector<Texture> textures;
	


	Mesh(VertexBuffer vertices, std::vector<Texture> textures, MatProperties mat_props, std::vector<unsigned int> indices)
	{
		this->vertices = vertices;
		this->textures = textures;
		this->mat_props = mat_props;		
		this->indices = indices;
		setupMesh();
	}
	void setupMesh();

	void Draw(GLSLProgram & GLProg);
	void Draw_Picking(GLSLProgram & GLProg);

	
	//temporal
	glm::vec3 K_Ambient, K_Diffuse, K_Specular;
	float NS;
	MatProperties mat_props;
private:	
	 
	GLuint m_positions, m_normals, m_text_coords, m_tangents, m_bitangents, m_weight_bones, m_bones, m_index;



};

