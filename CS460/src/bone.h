/**
* @file		bone.h
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
#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>



class Bone 
{
	public:

		Bone * next_bone = nullptr;

		Bone * parent = nullptr;

		float length;
	
		glm::mat4 M;
	
		glm::quat rotation;	
	
		Bone* add(Bone* b);	
		
		void remove(Bone* b);	
		
		void detach();			
		
		glm::vec3 getEndPosition();
		glm::vec3 getStartPosition();


		glm::mat4 getTransform();	

		glm::mat4 getConcatRot();

		void setEndPosition(glm::vec3 end);
		
		Bone* getEndEffector();	
		
		Bone(float l);
	
		Bone(const Bone& b);
		
		~Bone();
};

void DrawBones(Bone * root);


