/**
* @file		ik.h
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
#include "model.h"
#include "bone.h"



namespace IK
{
	class Bone_3D_CCD
	{
		public:
			glm::vec3 pos;
			Model * bone; 
	};	

	bool ComputeTwoBoneAnalytic(float & angle_bone1, float & angle_bone2,
	float length_bone1, float length_bone2, glm::vec2 target_pos, bool solve_for_pos);

	void CCD(Bone * root, glm::vec3 target, int iterations);
	void FABRICK(Bone * root, glm::vec3 target, int iterations);


	const glm::vec3 ref_rot_vector(0.0f, 0.0f, -1.0f);

};





