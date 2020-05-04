/**
* @file		verlet.cpp
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
#include "verlet.h"
#include "renderer.h"
void ParticleSystem::CreateParticles(int size, glm::vec3 gravity, float time_step, float separation)
{

	m_constraints.clear();
	m_x.clear();
	m_oldx.clear();
	m_a.clear();
	fixed.clear();
	points_grid.clear();

	for (unsigned i = 0; i < static_cast<unsigned>(size*size); i++)
	{
		m_x.push_back(glm::vec3(0.0f));		
		m_oldx.push_back(glm::vec3(0.0f));
		m_a.push_back(glm::vec3(0.0f));
		fixed.push_back(false);
	}
	m_vGravity = gravity;
	m_vWind = glm::vec3{ 0.0f, 0.0f ,-5.0f };
	m_fTimeStep = time_step;

	
	for (int i = 0; i < size; i++)
	{
		for (int j = (size * i); j < ((size * (i+1)) - 1); j++)
		{
			m_constraints.push_back({ j , j + 1 , separation });
		}
	}

	for (int i = 0; i < size - 1; i++)
	{
		for (int j = (size * i); j < ((size * (i + 1))); j++)
		{
			m_constraints.push_back({ j , j + static_cast<int>(size) , separation });
		}
	}

	line_size = size;
	total_size = size * size;

	for (unsigned i = 0; i < line_size; i++)
	{		
		fixed[i] = true;		
		m_x[i] = glm::vec3(i * separation, 0, 0);
	}
	points_grid.resize(m_constraints.size() * 2,glm::vec3(0.0f));
	gen_line_buffers();
}
void ParticleSystem::draw_lines()
{	
	glBindVertexArray(VAObject);
	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(points_grid.size()));	
}
void ParticleSystem::Verlet()
{
	for (unsigned i = 0; i < m_x.size(); i++)
	{
		if (fixed[i])
			continue;
		glm::vec3 & x = m_x[i];
		glm::vec3 temp = x;
		glm::vec3 & oldx = m_oldx[i];
		glm::vec3 & a = m_a[i];
		//check collision with spehere radius 1
		auto new_pos = x + (x - oldx + a * m_fTimeStep * m_fTimeStep);
		
		glm::vec3 vec_to_center(new_pos - g_render.Lights.back().TransformData.Position);

		float dist = glm::length(vec_to_center);

		if (dist > 1.0f)
		{
			x = new_pos;
		}
		else
		{
			x = glm::normalize(vec_to_center) + g_render.Lights.back().TransformData.Position;
		}
		oldx = temp;
	}
}

void ParticleSystem::AccumulateForces()
{
	for (int i = 0; i < m_x.size(); i++)
	{	
		if(!fixed[i])
			m_a[i] = m_vGravity + m_vWind;
	}
}
void ParticleSystem::SatisfyConstrains()
{ 
	for (int j = 0; j < NUM_ITERATIONS; j++)
	{		
		for (int i = 0; i < m_constraints.size(); i++)
		{ 
			Constraint & c = m_constraints[i];  
			glm::vec3 & x1 = m_x[c.particleA];        
			glm::vec3 & x2 = m_x[c.particleB];
			glm::vec3 delta = x2 - x1;
			float deltalength = glm::length(delta); 
			float diff;
			if(deltalength != 0)
				diff = (deltalength - c.restlength) / deltalength;
			else 
				diff = 0.0f;

			if (!fixed[c.particleA])
			{
				if (!fixed[c.particleB])
				{
					x1 += delta * 0.5f * diff;
					x2 -= delta * 0.5f * diff;
				}else
					x1 += delta * diff;
			}else if(!fixed[c.particleB])
				x2 -= delta * diff;

		}
			
		
	}
}

void ParticleSystem::TimeStep()
{ 
	AccumulateForces();
	Verlet(); 
	SatisfyConstrains();
}