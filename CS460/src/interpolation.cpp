#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "interpolation.h"
#include <GL/gl3w.h>
#include "renderer.h"
#include <algorithm>

void Interpolation::ReadFile(std::string filename)
{

	std::ifstream inputFileStream(filename);

	if (!inputFileStream)
	{
		std::cout << "unable to open file :" << filename << std::endl;
		return;
	}
	std::string line;
	std::getline(inputFileStream, line);
	curve act_curve;
	if ("BEZIER" == line)
		act_curve.interpolation_type = curve::type::bezier;
	else if ("CATMULL-ROM" == line)
		act_curve.interpolation_type = curve::type::catmull;
	else if("HERMITE" == line)
		act_curve.interpolation_type = curve::type::hermite;
	else
		act_curve.interpolation_type = curve::type::linear;


	std::getline(inputFileStream, line);

	while(inputFileStream)
	{
		std::string line;
		float actual_times;
		getline(inputFileStream, line);
		line.erase(std::remove_if(line.begin(), line.end(), isspace),line.end());

		if (line.empty())
			continue;

		auto init_delimit = line.find_first_of('=');
		auto end_delimit = line.find_first_of(';');
		auto last = end_delimit - init_delimit;
		actual_times = std::stof(line.substr(init_delimit + 1, last - 1));

		init_delimit = line.find_last_of('=');
		end_delimit = line.find_last_of(';');
		last = end_delimit - init_delimit;
		auto points = line.substr(init_delimit, last);

		glm::vec3 values;
		end_delimit = points.find_first_of(',');
		values.x = std::stof(points.substr(1, end_delimit - 1));
		init_delimit = end_delimit;
		end_delimit = points.find_last_of(',');
		auto temp = end_delimit - init_delimit;
		values.z = std::stof(points.substr(init_delimit + 1, temp - 1));
		values.y = std::stof(points.substr(end_delimit + 1, points.length()));		
		
		act_curve.values.push_back(values);
		act_curve.times.push_back(actual_times);

		
	}

	curves.push_back(act_curve);	
	curves.back().create_curve_points();
	curves.back().gen_line_buffers();

	if (act_curve.interpolation_type == curve::type::catmull)
	{
		curves.back().compute_adapt_forward_diff();
		//g_render.CreateLightsV2();


	}
}

float Interpolation::eval_ease(float t, float t1, float t2)
{

	float f = 2 * (t1 / glm::pi<float>()) + t2 - t1 + 2 * ((1 - t2) / glm::pi<float>());

	t = glm::clamp(t, 0.0f, 1.0f);

	if (t == 0.0f || t == 1.0f)
		return t;
	

	if (t < t1)
	{
		float result = (t1 * ( 2 / glm::pi<float>()) * (glm::sin((t/t1) * (glm::pi<float>() / 2) - (glm::pi<float>() / 2)) + 1)) / f;
		return result;
	}
	else if (t < t2)
	{
		float result = ((t - t1) + ((2 * t1) / glm::pi<float>())) / f;
		return result;
	}
	else
	{		
		float result = (2 * ((1 - t2) / glm::pi<float>()) * glm::sin(((t - t2) / (1 - t2)) * (glm::pi<float>() / 2)) + ((2 * t1) / glm::pi<float>()) + t2 - t1) / f;
		return result;
	}
}

void curve::update_time(float TimeInSeconds)
{
	time_curve = fmod(TimeInSeconds, duration());
}

void curve::gen_line_buffers()
{
	glGenVertexArrays(1, &VAObject);
	glGenBuffers(1, &VBObject);

	glBindVertexArray(VAObject);
	glBindBuffer(GL_ARRAY_BUFFER, VBObject);

	glBufferData(GL_ARRAY_BUFFER, points_curve.size() * sizeof(glm::vec3), points_curve.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//Clean
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void curve::compute_adapt_forward_diff()
{	
	arc_lenghts.push_back({ 0.0f, 0.0f });

	for (unsigned i = 1; i < times.size(); i++)
	{
		auto A = times[i - 1];
		auto B = times[i];
		auto C = (B-A) * 0.5f + A;
		divide(A, C, B);
	}

	float total_length = 0.0f;
	//Add lenghts and normalize
	for (unsigned i = 0; i < arc_lenghts.size(); i++)
	{
		arc_lenghts[i].first /= arc_lenghts[arc_lenghts.size() - 1].first;
		arc_lenghts[i].second += total_length;
		total_length = arc_lenghts[i].second;
	}

}

float curve::duration()
{
	return times[times.size() - 1];
}

void curve::draw_lines()
{
	glBindVertexArray(VAObject);
	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(points_curve.size()));
}
void curve::create_curve_points()
{
	float num_points = 1000.0f;
	float final_time = duration();

	for (float i = 0; i < (num_points + 1); i++)
	{
		float actual_time = (final_time) * (i / num_points);
		points_curve.push_back(evaluate(actual_time));
	}

}

void curve::divide(float A, float C, float B, int division_level)
{
	//eval times

	glm::vec3 point_1 = evaluate(A);
	glm::vec3 point_2 = evaluate(C);
	glm::vec3 point_3 = evaluate(B);

	float dist_A = glm::length(point_1 - point_2);
	float dist_B = glm::length(point_2 - point_3);
	float dist_C = glm::length(point_1 - point_3);
	
	//dont subdivide
	if (((dist_A + dist_B) - dist_C) < (error_perc / glm::pow(2, division_level))  && division_level > div_min)
	{
		arc_lenghts.push_back({ C, dist_A });
		arc_lenghts.push_back({ B, dist_B });
		return;
	}

	divide(A, ((C - A) * 0.5f + A), C, division_level + 1);
	divide(C, ((B - C) * 0.5f + C), B, division_level + 1);

	
}

struct keycompare {
	bool operator()(const std::pair<float, float>& v, const float& k) {
		return (v.second < k);
	}
	bool operator()(const float& k, const std::pair<float, float>& v) {
		return (k < v.second);
	}
};

float curve::get_time_in_curve(float arc_lenght)
{

	auto it = std::lower_bound(arc_lenghts.begin(), arc_lenghts.end(), arc_lenght, keycompare());
	if (it == arc_lenghts.end()) {
		return 0.0;
	}
	else if (it->second == arc_lenght)
	{
		return it->first;
	}
	else {
		if (it != arc_lenghts.begin())
		{
			auto it_prev = it;
			it_prev--;
			float coeff = (arc_lenght - it_prev->second) / (it->second - it_prev->second);
			
			float real_time = it_prev->first + (it->first - it_prev->first) * coeff;
			return real_time;
		}
	}

	return 0.0f;
}

glm::vec3 curve::evaluate(float curve_time)
{

	glm::vec3 eval_point;

	// boundary conditions
	if (curve_time < times[0])
		return values[0];
	if (curve_time > times[times.size() - 1])
		return values[values.size() - 1];

	

	if (interpolation_type == type::linear)
	{
		u32 frame_idx = 1;

		while (curve_time > times[frame_idx])
			++frame_idx;

		// find the segment we interpolate
		glm::vec3 p0 = values[frame_idx - 1];
		glm::vec3 p1 = values[frame_idx];
		// normalize the time to match the keytime interval
		f32 interval_duration = times[frame_idx] - times[frame_idx - 1];
		f32 local_time = curve_time - times[frame_idx - 1];
		f32 tn = local_time / interval_duration;
		// apply linear interpolation
		eval_point = p0 + (p1 - p0) * tn;
	}
	else if (interpolation_type == type::hermite)
	{
		u32 frame_idx = 3;
		while (curve_time > times[frame_idx])
			frame_idx += 3;		

		glm::vec3 P0 = values[frame_idx - 3];
		glm::vec3 CP0 = values[frame_idx - 2];
		glm::vec3 CP1 = values[frame_idx - 1];
		glm::vec3 P1 = values[frame_idx];

		float time = (curve_time - times[frame_idx -3]) / (times[frame_idx] - times[frame_idx - 3]);

		for (int i = 0; i < 3; i++) //Aplly Hermite formula
			eval_point[i] = (2 * (P0[i] - P1[i]) + CP0[i] + CP1[i]) * (time * time * time) +
			(3 * (P1[i] - P0[i]) - 2 * CP0[i] - CP1[i]) * (time * time) + CP0[i] * time + P0[i];

	}
	else if (interpolation_type == type::bezier)
	{
		u32 frame_idx = 3;
		while (curve_time > times[frame_idx])
			frame_idx += 3;

		glm::vec3 P0 = values[frame_idx - 3];
		glm::vec3 CP0 = values[frame_idx - 2];
		glm::vec3 CP1 = values[frame_idx - 1];
		glm::vec3 P1 = values[frame_idx];

		float tn = (curve_time - times[frame_idx - 3]) / (times[frame_idx] - times[frame_idx - 3]);


		for (int i = 0; i < 3; i++) //Calculate the out res with the formula of bezier
			eval_point[i] = ((1 - tn) * (1 - tn) * (1 - tn) * P0[i]) + 3 * tn * ((1 - tn) *
			(1 - tn)) * CP0[i] + 3 * (tn * tn) * (1 - tn) * CP1[i] + (tn * tn * tn) * P1[i];
	}
	else if (interpolation_type == type::catmull)
	{
		u32 frame_idx = 1;
		while (curve_time > times[frame_idx])
			frame_idx++;

		glm::vec3 P0, P1, P2, P3, CP0, CP1;

		
		P1 = values[frame_idx - 1];
		P2 = values[frame_idx];		
		
		if (frame_idx - 1 > 0)
			CP0 = (P2 - values[frame_idx - 2]) / 2.0f;
		else
		{
			P3 = values[frame_idx + 1];
			CP0 = ((P2 - P1) + (P2 - P3));
		}	

		if (frame_idx < (values.size() - 1))
			CP1 = (values[frame_idx + 1] - P1) / 2.0f;
		else
		{
			P0 = values[frame_idx - 2];
			CP1 = -((P1 - P2) + (P1 - P0)) / 2.0f;
		}

					   
		float time = (curve_time - times[frame_idx - 1]) / (times[frame_idx] - times[frame_idx - 1]);

		for (int i = 0; i < 3; i++) //Aply Hermite formula
			eval_point[i] = (2 * (P1[i] - P2[i]) + CP0[i] + CP1[i]) * (time * time * time) +
			(3 * (P2[i] - P1[i]) - 2 * CP0[i] - CP1[i]) * (time * time) + CP0[i] * time + P1[i];

	}

	return eval_point;
}
