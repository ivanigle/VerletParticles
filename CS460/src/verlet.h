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
#include "model.h"
#define NUM_ITERATIONS 1

struct Constraint {
	Constraint(int  partiA, int  partiB, float length)
	{
		particleA = partiA;
		particleB = partiB;
		restlength = length;
	}
	int   particleA, particleB;
	float restlength;
};

class ParticleSystem
{
public:
	void TimeStep();
	void CreateParticles(int size, glm::vec3 gravity, float time_step, float separation);
	size_t total_size{ 0 };
	std::vector<glm::vec3> m_x;
	std::vector <Constraint> m_constraints;	
	std::vector<glm::vec3> points_grid;
	std::vector<bool> fixed;
	glm::vec3 m_vGravity, m_vWind;

	void draw_lines();

	void gen_line_buffers()
	{
		if (VBObject)
		{
			glDeleteBuffers(1, &VBObject);			
		}

		glGenVertexArrays(1, &VAObject);
		glGenBuffers(1, &VBObject);

		glBindVertexArray(VAObject);
		glBindBuffer(GL_ARRAY_BUFFER, VBObject);

		glBufferData(GL_ARRAY_BUFFER, points_grid.size() * sizeof(glm::vec3), points_grid.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//Clean
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}
	void bind_buffers()
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBObject);
		glBufferData(GL_ARRAY_BUFFER, points_grid.size() * sizeof(glm::vec3), points_grid.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//Clean
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}
private:

	void Verlet();
	void SatisfyConstrains();
	void AccumulateForces();

	std::vector<glm::vec3> m_oldx;
	std::vector<glm::vec3> m_a;

	float m_fTimeStep;

	size_t line_size{ 0 };


	unsigned int VAObject, VBObject;


	

};




