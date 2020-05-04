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
#include "ik.h"
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>
#include <fstream>
#include "renderer.h"
using namespace std;
#include <vector>
const float epsilon_actual = 0.0001f;

#define IK_POS_THRESH 0.001


bool IK::ComputeTwoBoneAnalytic(float & angle_bone1, float & angle_bone2, float length_bone1, float length_bone2, glm::vec2 target_pos, bool solve_for_pos)
{
	if (length_bone1 <= 0 || length_bone2 <= 0)
		return false;

	float dist_to_target = glm::length(target_pos);

	bool founded = true;

	float distance = target_pos.x * target_pos.x + target_pos.y * target_pos.y;

	//values for angle2 along cosine

	float sin_angle2, cos_angle2;

	float cos_angle2_denom = 2 * length_bone1 * length_bone2;

	if (cos_angle2_denom > epsilon_actual)
	{
		cos_angle2 = (distance - length_bone1 * length_bone1 - length_bone2 * length_bone2)	/ cos_angle2_denom;

		//check  if the result is legal
		if ((cos_angle2 < -1.0) || (cos_angle2 > 1.0))
		{
			founded = false;
			return false;
		}

		cos_angle2 = std::fmax(-1.0f, std::fmin(1.0f, cos_angle2));

		angle_bone2 = std::acos(cos_angle2);

		if (!solve_for_pos)
			angle_bone2 = -angle_bone2;

		sin_angle2 = std::sin(angle_bone2);

	}
	else
	{
		float total_length_sqr = (length_bone1 + length_bone2) * (length_bone1 + length_bone2);
		if (distance < (total_length_sqr - epsilon_actual)|| distance >(total_length_sqr + epsilon_actual))
		{
			founded = false;
		}

		//set angle 2 to zero
		angle_bone2 = 0.0f;
		cos_angle2 = 1.0f;
		sin_angle2 = 0.0f;
	}

	//Compute the value of angle1 based on the sine and cosine of angle 2

	float tri_adjacent = length_bone1 + length_bone2 * cos_angle2;
	float tri_oppposite = length_bone2 * sin_angle2;

	float tan_Y = target_pos.y *  tri_adjacent - target_pos.x *  tri_oppposite;
	float tan_X = target_pos.x *  tri_adjacent + target_pos.y *  tri_oppposite;

	angle_bone1 = std::atan2(tan_Y, tan_X);

	return founded;
}

void IK::CCD(Bone * root, glm::vec3 target, int iterations)
{
	bool found = false;
	g_render.status_value = 2;

	Bone * endEffector = root->getEndEffector();

	while (!found && iterations--)
	{
		Bone * currentBone = endEffector;
		do 
		{
			glm::vec3 endEffectorPos = endEffector->getEndPosition();
			glm::vec3 startPosition = currentBone->getStartPosition();
			glm::vec3 toTarget = glm::normalize(target - startPosition);
			glm::vec3 toEnd = glm::normalize(endEffectorPos - startPosition);

			float cosine = dot(toEnd, toTarget);
			if (abs(cosine) < (1.0f - glm::epsilon<float>()))
			{
				glm::quat rotation(toEnd, toTarget);
				//rotation = glm::normalize(rotation);
				currentBone->rotation = normalize(rotation * currentBone->rotation);
			}
			else if(cosine < 0.0f)
			{
				vec3 angle_rand = vec3(glm::half_pi<float>(), 0,0);
				glm::quat rotation(angle_rand);
				currentBone->rotation = glm::normalize(rotation * currentBone->rotation);
			}

			glm::vec3 temp = glm::vec3(endEffector->getEndPosition());
			temp.x -= target.x;
			temp.y -= target.y;
			temp.z -= target.z;

			if (dot(temp, temp) < g_render.dist_threshold)
			{
				found = true;
			}
			currentBone = currentBone->parent;
		} while (currentBone != NULL);
	}

	if (found)
	{
		g_render.status_value = 0;
		g_render.iteration_counter = 0;
	}
	else
	{
		g_render.iteration_counter += g_render.num_iterations_per_frame;
		if (g_render.iteration_counter >= g_render.num_iterations_total)
		{
			g_render.status_value = 1;
			g_render.iteration_counter = 0;
		}
	}

	

}

void IK::FABRICK(Bone * root, glm::vec3 target, int iterations)
{

	bool found = false;
	g_render.status_value = 2;

	while (!found && iterations--)
	{
		std::vector<vec3> joints;

		joints.push_back({ root->getStartPosition() });

		Bone * currentBone = root;
		do
		{
			joints.push_back({ currentBone->getEndPosition() });
			currentBone = currentBone->next_bone;
		} while (currentBone != NULL);
		
		//backwards
		joints.back() = target;
		int i = static_cast<int>(joints.size()) - 2;
		currentBone = root->getEndEffector();
		do
		{
			vec3 delta = joints[i + 1] - joints[i];
			vec3 fix_delta = normalize(delta) * currentBone->length;
			joints[i] = joints[i + 1] - fix_delta;
			currentBone = currentBone->parent;
			i--;
		} while (currentBone != NULL);

		//fordwards
		i = 1;
		joints.front() = root->getStartPosition();
		currentBone = root;
		do
		{
			vec3 delta = joints[i - 1] - joints[i];
			vec3 fix_delta = normalize(delta) * currentBone->length;
			joints[i] = joints[i - 1] - fix_delta;
			currentBone = currentBone->next_bone;
			i++;
		} while (currentBone != NULL);

		i = 1;
		currentBone = root;
		do
		{
			currentBone->setEndPosition(joints[i]);
			currentBone = currentBone->next_bone;
			i++;
		} while (currentBone != NULL);
		
		glm::vec3 temp = root->getEndEffector()->getEndPosition();
		temp.x -= target.x;
		temp.y -= target.y;
		temp.z -= target.z;

		if (dot(temp, temp) < g_render.dist_threshold)
		{
			found = true;
		}

	}
	if (found)
	{
		g_render.status_value = 0;
		g_render.iteration_counter = 0;
	}
	else
	{
		g_render.iteration_counter += g_render.num_iterations_per_frame;
		if (g_render.iteration_counter >= g_render.num_iterations_total)
		{
			g_render.status_value = 1;
			g_render.iteration_counter = 0;
		}
	}


}






