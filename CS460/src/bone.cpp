/**
* @file		ik.cpp
* @date 	2/11/2019
* @author	Ivan Iglesias
* @par		Login: ivan.iglesias
* @par		Course: CS350
* @par		Assignment #2
* @brief 	Implementation of the ik
*
* Hours spent on this assignment: 20h
*
*/
#include "bone.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <algorithm>
#include "renderer.h"
#include <glm/gtx/string_cast.hpp>



Bone::Bone(float l)
{
	length = l;
	M = glm::mat4(1.0f);
	parent = NULL;
	rotation = glm::quat();
};

Bone::Bone(const Bone& b)
{
	length = b.length;
	M = glm::mat4(b.M);
	parent = NULL;
	rotation = b.rotation;	
	add(b.next_bone);
	
}

Bone* Bone::add(Bone *b)
{
	b->parent = this;
	next_bone = b;
	return b;
}

void Bone::remove(Bone *b)
{
	b->parent->next_bone = b->next_bone;
	b->next_bone->parent = b->parent;
	delete b;
}

glm::vec3 Bone::getEndPosition()
{	
	return vec3(getTransform() * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));	
}

glm::vec3 Bone::getStartPosition()
{
	if (parent)
		return vec3(parent->getEndPosition());
	else//CHANGE HAS TO BE BONE START
		return vec3(0);
}

glm::mat4 Bone::getTransform()
{
	glm::mat4 P = glm::mat4(1.0f);

	if (parent != NULL)
	{
		P = glm::translate(P, parent->getEndPosition());		
	}else
		P = glm::translate(P, vec3(0,0,0));

	P *= getConcatRot();
	P = glm::scale(P, glm::vec3(length));

	return P;
}

glm::mat4 Bone::getConcatRot()
{

	if (parent != NULL)
	{
		return glm::toMat4(rotation) *  parent->getConcatRot();
	}

	return glm::toMat4(rotation);
	
}

void Bone::setEndPosition(glm::vec3 end)
{
	vec3 dirParent;
	if (parent)
		dirParent = normalize(parent->getEndPosition() - parent->getStartPosition());
	else
		dirParent = glm::vec3(0.0f, 0.0f, 1.0f);

	vec3 dirCurrent = normalize(end - getStartPosition());

	float cosine = dot(dirParent, dirCurrent);
	if (abs(cosine) < (1.0f - glm::epsilon<float>()))
	{
		glm::quat rotQuat(dirParent, dirCurrent);
		rotation = normalize(rotQuat);
	}
	else if (cosine < 0.0f)
	{
		vec3 angle_rand = vec3(glm::half_pi<float>(), 0, 0);
		glm::quat rotation(angle_rand);
		rotation = glm::normalize(rotation);
	}	

}

void Bone::detach()
{	
	parent->next_bone = next_bone;
	if(next_bone)
		next_bone->parent = parent;
	delete this;
}

Bone* Bone::getEndEffector()
{
	if (next_bone)
	{
		return next_bone->getEndEffector();
	}
	else
	{
		return this;
	}
}

Bone::~Bone()
{
	/*for (std::vector<BoneCCD*>::iterator it = bones.begin(); it != bones.end(); it++) {
		delete *it;
	}*/
}

void DrawBones(Bone * b)
{

	g_render.geometry_program.setUniform("model", b->getTransform());

	g_render.bones_ccd[0].bone->meshes[0].Draw(g_render.geometry_program);
	

	if(b->next_bone)
		DrawBones(b->next_bone);	

}

