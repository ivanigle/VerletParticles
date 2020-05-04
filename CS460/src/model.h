/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Model.h
Purpose: Functions that define the shapes
Language: C++ Visual Studio 2015
Platform: Windows 7
Project: cs300_ivan.iglesias_1
Author: Ivan Iglesias, ivan.iglesias
Creation date: 5/21/2018
----------------------------------------------------------------------------------------------------------*/
#pragma once
#include "glm\glm.hpp"
#include "mesh.h"
#include "transform.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>
#include <iostream>
#include <glm/gtx/quaternion.hpp>


#define num_of_elements(a) (sizeof(a)/sizeof(a[0]))

struct BoneInfo
{
	glm::mat4 BoneOffset;
	glm::mat4 FinalTransformation;
};

class Model
{
public:
	Mesh MeshData;
	Transform TransformData;	
	bool RenderObject = true;
	Model(){}

	Model(const char * path)
	{
		load_model(path);
	}
	//Data for model
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;

	void bone_transformation(float TimeInSeconds, std::vector<glm::mat4>& Transforms);

	const aiScene* m_pScene;
	Assimp::Importer m_Importer;

	float distance{ 0.0 };

	float anim_time{ 0.0f };
	float anim_speed{ 1.0f };
		

private:
	
	//funcionts for loading the model
	void load_model(std::string path);
	void process_node(aiNode * node, const aiScene * scene);
	Mesh process_mesh(unsigned index, aiMesh * mesh, const aiScene * scene);
	std::vector<Texture> load_mat_textures(aiMaterial * mat, aiTextureType type, std::string typeName);
	unsigned int read_texture_from_file(const char *path, const std::string &directory, bool gamma = false);

	//Bones
	void load_bones(unsigned int MeshIndex, const aiMesh* pMesh, VertexBuffer & Bones);
	aiMatrix4x4 m_GlobalInverseTransform;
	std::map<std::string, unsigned int> m_BoneMapping;
	unsigned int m_NumBones;
	std::vector<BoneInfo> m_BoneInfo;
	//Interpolate funcionts
	void interpolate_scale(aiVector3D& Out, double AnimationTime, const aiNodeAnim* pNodeAnim);
	void interpolate_rotation(glm::quat & Out, double AnimationTime, const aiNodeAnim* pNodeAnim);
	void interpolate_position(aiVector3D& Out, double AnimationTime, const aiNodeAnim* pNodeAnim);
	//find functions
	unsigned find_scale_key(double AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned find_rotation_key(double AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned find_position_key(double AnimationTime, const aiNodeAnim* pNodeAnim);
	//find animation
	const aiNodeAnim* find_anim(const aiAnimation* pAnimation, const string NodeName);
	void iterate_heirarchy(double AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);

	void update_anim_time(float time);

};
