/* ---------------------------------------------------------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: model.cpp
Purpose: functions to read the model import with assimp
Language: C++ Visual Studio 2017
Platform: Windows 10
Project: cs460_ivan.iglesias_1
Author: Ivan Iglesias, ivan.iglesias
Creation date: 9/26/2019
----------------------------------------------------------------------------------------------------------*/
#include "Model.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#include <glm\gtc\matrix_transform.hpp>
#include "renderer.h"


glm::mat3 aiMatrix3x3ToGlm(const aiMatrix3x3 &from)
{
	glm::mat3 to;
	to[0][0] = from.a1; to[0][1] = from.a2; to[0][2] = from.a3;
	to[1][0] = from.b1; to[1][1] = from.b2; to[1][2] = from.b3;
	to[2][0] = from.c1; to[2][1] = from.c2; to[2][2] = from.c3;
	return glm::transpose(to);
}

glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &from)
{
	glm::mat4 to;
	to[0][0] = from.a1; to[0][1] = from.a2; to[0][2] = from.a3; to[0][3] = from.a4;
	to[1][0] = from.b1; to[1][1] = from.b2; to[1][2] = from.b3; to[1][3] = from.b4;
	to[2][0] = from.c1; to[2][1] = from.c2; to[2][2] = from.c3; to[2][3] = from.c4;
	to[3][0] = from.d1; to[3][1] = from.d2; to[3][2] = from.d3; to[3][3] = from.d4;
	return glm::transpose(to);
}
glm::quat aiQuatToGlm(const aiQuaternion & from)
{
	glm::quat to_quat(from.w, from.x, from.y, from.z);
	return to_quat;
}

glm::quat nlerp(const glm::quat & p, const glm::quat & q, const float & t)
{
	float cosom = p.x * q.x + p.y * q.y + p.z * q.z + p.w * q.w;

	glm::quat end = q;
	if (cosom < static_cast<float>(0.0))
	{
		cosom = -cosom;
		end.x = -end.x;   // Reverse all signs
		end.y = -end.y;
		end.z = -end.z;
		end.w = -end.w;
	}

	glm::quat p_quat = glm::normalize(p + (end - p) * t);
	return p_quat;
}

glm::quat slerp(const glm::quat & p, const glm::quat & q, const float & t)
{
	float cosom = p.x * q.x + p.y * q.y + p.z * q.z + p.w * q.w;

	glm::quat end = q;
	if (cosom < static_cast<float>(0.0))
	{
		cosom = -cosom;
		end.x = -end.x;   // Reverse all signs
		end.y = -end.y;
		end.z = -end.z;
		end.w = -end.w;
	}

	if (glm::dot(p, end) > (1.0f - glm::epsilon<float>()))
	{
		return nlerp(p, end, t);
	}
	{
		auto dot_quat = glm::dot(p, end);
		auto alpha = glm::acos(dot_quat);
		glm::quat p_quat = (glm::sin((1 - t)* alpha)* p + glm::sin(t*alpha)*end) / glm::sin(alpha);
		return glm::normalize(p_quat);
	}
}




void Model::load_model(std::string path)
{
	m_pScene = m_Importer.ReadFile(path, aiProcess_FlipUVs | aiProcessPreset_TargetRealtime_Quality);


	if (!m_pScene || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << m_Importer.GetErrorString() << std::endl;
		return;
	}
	else if (m_pScene)
	{
		m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation;
		m_GlobalInverseTransform.Inverse();
	}	

	directory = path.substr(0, path.find_last_of('/'));

	process_node(m_pScene->mRootNode, m_pScene);

}

void Model::process_node(aiNode * node, const aiScene * scene)
{

	//Iterate on each mesh
	for (unsigned i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(process_mesh(i, mesh, scene));
	}
	//go throw the submeshes
	for (unsigned i = 0; i < node->mNumChildren; i++)
	{
		process_node(node->mChildren[i], scene);
	}
}

Mesh Model::process_mesh(unsigned index, aiMesh * mesh, const aiScene * scene)
{
	// data to fill
	VertexBuffer vertex_buff(mesh->mNumVertices);
	std::vector<Texture> textures;	


	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// positions
		vertex_buff.Position[i].x = mesh->mVertices[i].x;
		vertex_buff.Position[i].y = mesh->mVertices[i].y;
		vertex_buff.Position[i].z = mesh->mVertices[i].z;
		// normals
		vertex_buff.Normal[i].x = mesh->mNormals[i].x;
		vertex_buff.Normal[i].y = mesh->mNormals[i].y;
		vertex_buff.Normal[i].z = mesh->mNormals[i].z;
		// texture coordinates
		if (mesh->mTextureCoords[0])
		{
			//take first texture coordinates
			vertex_buff.TexCoords[i].x = mesh->mTextureCoords[0][i].x;
			vertex_buff.TexCoords[i].y = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex_buff.TexCoords[i] = glm::vec2(0.0f, 0.0f);
		// tangent
		if (mesh->mTangents)
		{
			vertex_buff.Tangent[i].x = mesh->mTangents[i].x;
			vertex_buff.Tangent[i].y = mesh->mTangents[i].y;
			vertex_buff.Tangent[i].z = mesh->mTangents[i].z;
		}
		else
			vertex_buff.Tangent[i] = glm::vec3(0.0f, 0.0f, 0.0f);
		// bitangent
		if (mesh->mBitangents)
		{
			vertex_buff.Bitangent[i].x = mesh->mBitangents[i].x;
			vertex_buff.Bitangent[i].y = mesh->mBitangents[i].y;
			vertex_buff.Bitangent[i].z = mesh->mBitangents[i].z;
		}
		else
			vertex_buff.Bitangent[i] = glm::vec3(0.0f, 0.0f, 0.0f);		
	}
	

	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		//indices 
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	//materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];	

	//diffuse maps
	std::vector<Texture> diffuseMaps = load_mat_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());	
	//specular maps
	std::vector<Texture> specularMaps = load_mat_textures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	//normal maps
	std::vector<Texture> normalMaps = load_mat_textures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());	

	MatProperties props;
	aiColor3D color;
	//read material values	
	material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	props.K_Diffuse = glm::vec3(color.r, color.g, color.b);
	material->Get(AI_MATKEY_COLOR_SPECULAR, color);
	props.K_Specular = glm::vec3(color.r, color.g, color.b);
	material->Get(AI_MATKEY_COLOR_AMBIENT, color);
	props.K_Ambient = glm::vec3(color.r, color.g, color.b);
	material->Get(AI_MATKEY_SHININESS, props.NS);

	load_bones(index, mesh, vertex_buff);

	// return a mesh object created from the extracted mesh data
	return Mesh(vertex_buff, textures, props, indices);

}

std::vector<Texture> Model::load_mat_textures(aiMaterial * mat, aiTextureType type, std::string typeName)
{
	//vector for the resulting textures
	std::vector<Texture> textures;	
	//read all the textures
	for (unsigned i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false; 
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{	
			//Not to read the same texture
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			//create new texture
			Texture texture;
			texture.id = read_texture_from_file(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);

		}
	}
	return textures;
}

unsigned int Model::read_texture_from_file(const char * path, const std::string & directory, bool gamma)
{
	std::string filename = std::string(path);

	auto pos = filename.find_last_of('/');

	auto name = filename.substr(pos, filename.length());
	filename = directory + name;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	//Read texture from file
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	std::cout << "Texture load at path: " << filename << std::endl;

	//Analyze the data
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

		//Initialize each texture on opengl
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//free the used data
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}
	
	return textureID;
}

void Model::load_bones(unsigned int MeshIndex, const aiMesh * pMesh, VertexBuffer& vert_buffer)
{
	//Load all the bones of the mesh
	for (unsigned i = 0; i < pMesh->mNumBones; i++)
	{
		unsigned BoneIndex = 0;
		//Creat new bone
		std::string BoneName(pMesh->mBones[i]->mName.data);

		//try to find bone in the map
		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end())
		{
			BoneIndex = m_NumBones;
			//Add bone count
			m_NumBones++;
			BoneInfo bi;
			//store bone info
			m_BoneInfo.push_back(bi);
			m_BoneInfo[BoneIndex].BoneOffset = aiMatrix4x4ToGlm(pMesh->mBones[i]->mOffsetMatrix);
			m_BoneMapping[BoneName] = BoneIndex;
		}
		else
		{
			BoneIndex = m_BoneMapping[BoneName];
		}

		//Depending on the number of weitghts it hac
		for (unsigned j = 0; j < pMesh->mBones[i]->mNumWeights; j++)
		{
			//take weight
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			//take the vertexID
			unsigned VertexID = pMesh->mBones[i]->mWeights[j].mVertexId;
			//add bone data

			//Store maximum of 4 influences
			for (unsigned i = 0; i < 4; i++)
			{
				if (vert_buffer.Bones_Weights[VertexID][i] == 0.0)
				{
					vert_buffer.Bones[VertexID][i] = BoneIndex;
					vert_buffer.Bones_Weights[VertexID][i] = Weight;
					break;
				}
			}
		}

	}
}

void VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
{
	for (unsigned i = 0; i < num_of_elements(IDs); i++)
	{
		if (Weights[i] == 0.0)
		{
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}	
}



void Model::interpolate_rotation(glm::quat & Out, double AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1)
	{
		Out = aiQuatToGlm(pNodeAnim->mRotationKeys[0].mValue);
		return;
	}
	//take rotations
	unsigned rot_index = find_rotation_key(AnimationTime, pNodeAnim);
	unsigned next_rot = (rot_index + 1);
	//times values
	float delta_time = static_cast<float>(pNodeAnim->mRotationKeys[next_rot].mTime - pNodeAnim->mRotationKeys[rot_index].mTime);
	float Factor = (static_cast<float>(AnimationTime) - static_cast<float>(pNodeAnim->mRotationKeys[rot_index].mTime)) / delta_time;
	//init and end rotation
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[rot_index].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[next_rot].mValue;
	//Do nlerp or slerp 
	if (g_render.slerp_nlerp == 0)
		Out = slerp(aiQuatToGlm(StartRotationQ), aiQuatToGlm(EndRotationQ), Factor);
	else
		Out = nlerp(aiQuatToGlm(StartRotationQ), aiQuatToGlm(EndRotationQ), Factor);

}

void Model::interpolate_position(aiVector3D& Out, double AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}
	//Take start and end position
	unsigned pos_index = find_position_key(AnimationTime, pNodeAnim);
	unsigned next_pos = (pos_index + 1);
	//compute times
	float delta_time = static_cast<float>(pNodeAnim->mPositionKeys[next_pos].mTime - pNodeAnim->mPositionKeys[pos_index].mTime);
	float Factor = (static_cast<float>(AnimationTime) - static_cast<float>(pNodeAnim->mPositionKeys[pos_index].mTime)) / delta_time;
	//Take the start and end vectors
	const aiVector3D& Start = pNodeAnim->mPositionKeys[pos_index].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[next_pos].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void Model::interpolate_scale(aiVector3D& Out, double AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}
	//Scale index init and next
	unsigned scale_index = find_scale_key(AnimationTime, pNodeAnim);
	unsigned next_scale_index = (scale_index + 1);
	//compute times
	float delta_time = static_cast<float>(pNodeAnim->mScalingKeys[next_scale_index].mTime - pNodeAnim->mScalingKeys[scale_index].mTime);
	float Factor = (static_cast<float>(AnimationTime) - static_cast<float>(pNodeAnim->mScalingKeys[scale_index].mTime)) / delta_time;
	//Take initial and end values
	const aiVector3D& Start = pNodeAnim->mScalingKeys[scale_index].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[next_scale_index].mValue;
	//compute vector
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void Model::iterate_heirarchy(double AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
{
	string NodeName(pNode->mName.data);

	glm::mat4 NodeTransformation(aiMatrix4x4ToGlm(pNode->mTransformation));

	const aiAnimation* pAnimation = m_pScene->mAnimations[0];

	const aiNodeAnim* pNodeAnim = find_anim(pAnimation, NodeName);

	if (pNodeAnim)
	{
		//Interpolate Translation
		aiVector3D transform_vec;
		interpolate_position(transform_vec, AnimationTime, pNodeAnim);
		glm::mat4 translation_matrix = glm::mat4(1.0f);
		translation_matrix = glm::translate(translation_matrix, glm::vec3(transform_vec.x, transform_vec.y, transform_vec.z));

		//Interpolate Rotation
		glm::quat rot_quat;
		interpolate_rotation(rot_quat, AnimationTime, pNodeAnim);
		glm::mat4 rotation_matrix = glm::toMat4(rot_quat);

		//Interpolate Scale
		aiVector3D scale_vec;
		interpolate_scale(scale_vec, AnimationTime, pNodeAnim);
		glm::mat4 scale_matrix = glm::mat4(1.0f);
		scale_matrix = glm::scale(scale_matrix, glm::vec3(scale_vec.x, scale_vec.y, scale_vec.z ));	

		//Multiply all the transformations
		NodeTransformation = translation_matrix * rotation_matrix * scale_matrix;
	}

	glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end())
	{
		unsigned BoneIndex = m_BoneMapping[NodeName];
		//Compute the final transformation
		m_BoneInfo[BoneIndex].FinalTransformation = aiMatrix4x4ToGlm(m_GlobalInverseTransform) * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
	}

	for (unsigned i = 0; i < pNode->mNumChildren; i++)
		iterate_heirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	
}

void Model::update_anim_time(float dt)
{	
	anim_time = fmod(anim_time + (dt * anim_speed), static_cast<float>(m_pScene->mAnimations[0]->mDuration));
}


void Model::bone_transformation(float dt, std::vector<glm::mat4>& Transforms)
{
	if (!m_pScene->mNumAnimations)
		return;

	//evaluate curve
	if (g_render.actual_curve != 4)
	{
		distance += g_render.speed_in_curve * dt;
		float dist = Interpolation::eval_ease(distance / g_render.curves.curves[g_render.actual_curve].arc_lenghts.back().second, g_render.t1, g_render.t2);

		dist *= g_render.curves.curves[g_render.actual_curve].arc_lenghts.back().second;

		float time_curve = g_render.curves.curves[g_render.actual_curve].get_time_in_curve(dist);
	
		float prev_time = Interpolation::eval_ease(time_curve - 0.01f, g_render.t1, g_render.t2);
		float next_time = Interpolation::eval_ease(time_curve + 0.01f, g_render.t1, g_render.t2);

		anim_speed = (next_time - prev_time) / (0.02f);

		anim_speed *= g_render.max_anim_spped;

		if (time_curve == 1.0)
		{
			distance = 0.0;
		}
		auto dur = g_render.curves.curves[g_render.actual_curve].duration();
		g_render.curves.curves[g_render.actual_curve].update_time(static_cast<float>(dt));

		glm::vec3 pos = g_render.curves.curves[g_render.actual_curve].evaluate(time_curve * dur);
		TransformData.Position = pos;		

		//Rotation
		if (g_render.follow_point)
		{
			glm::vec3 target_point = g_render.Lights.back().TransformData.Position;
			glm::vec3 actual_point = g_render.curves.curves[g_render.actual_curve].evaluate(time_curve * dur);

			glm::vec3 front_vec = glm::normalize(actual_point - target_point);
			TransformData.rot_quat = glm::normalize(glm::quatLookAt(front_vec, glm::vec3(0, 1, 0)));
		}
		else
		{
			glm::vec3 next_point = g_render.curves.curves[g_render.actual_curve].evaluate((time_curve + 0.01f) * dur);
			glm::vec3 prev_point = g_render.curves.curves[g_render.actual_curve].evaluate((time_curve - 0.01f) * dur);

			glm::vec3 front_vec = glm::normalize(prev_point - next_point);
			TransformData.rot_quat = glm::normalize(glm::quatLookAt(front_vec, glm::vec3(0, 1, 0)));
		}
	}

 	glm::mat4 Identity = glm::mat4(1.0f);	

	/*double TicksPerSecond = (m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
	double TimeInTicks = TimeInSeconds * TicksPerSecond;
	double AnimationTime = fmod(TimeInTicks, m_pScene->mAnimations[0]->mDuration);*/
	update_anim_time(dt);

	iterate_heirarchy(anim_time, m_pScene->mRootNode, Identity);

	Transforms.resize(m_NumBones);

	for (unsigned i = 0; i < m_NumBones; i++) 		
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
	
}


const aiNodeAnim* Model::find_anim(const aiAnimation* pAnimation, const string NodeName)
{
	for (unsigned i = 0; i < pAnimation->mNumChannels; i++)
	{
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName)
		{
			return pNodeAnim;
		}
	}
	return NULL;
}

unsigned Model::find_position_key(double AnimationTime, const aiNodeAnim* pNodeAnim)
{
	//Find to what position key corresponds this animation
	for (unsigned i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if (AnimationTime < pNodeAnim->mPositionKeys[i + 1].mTime)
			return i;
	}
	return 0;
}


unsigned Model::find_rotation_key(double AnimationTime, const aiNodeAnim* pNodeAnim)
{
	//Find to what rotation key corresponds this animation
	for (unsigned i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if (AnimationTime < pNodeAnim->mRotationKeys[i + 1].mTime)
			return i;
	}
	return 0;
}


unsigned Model::find_scale_key(double AnimationTime, const aiNodeAnim* pNodeAnim)
{
	//Find to what scale key corresponds this animation

	for (unsigned i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
	{
		if (AnimationTime < pNodeAnim->mScalingKeys[i + 1].mTime)
			return i;
	}
	return 0;
}