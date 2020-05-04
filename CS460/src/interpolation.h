#pragma once
#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <list>
#include <utility> 


struct curve
{
	enum type
	{
		catmull, bezier, hermite, linear
	};
	type interpolation_type;

	std::vector<float> times;
	std::vector<glm::vec3> values;

	void update_time(float global_time);

	float duration();

	void draw_lines();

	glm::vec3 evaluate(float curve_time);

	float time_curve;

	unsigned int VAObject, VBObject;

	void gen_line_buffers();

	void compute_adapt_forward_diff();

	void create_curve_points();

	void divide(float A, float B, float C, int div_leve = 0);

	float get_time_in_curve(float arc_lenght);
	
	//time - arclenght
	std::vector<std::pair<float, float>> arc_lenghts;

	float error_perc{ 0.05f };
	int div_min{ 2 };

	std::vector<glm::vec3> points_curve;

private:


	float total_length = 0;
};

class Interpolation
{
public:
	Interpolation() {};
	

	~Interpolation() {};

	void ReadFile(std::string filename);

	std::vector<curve> curves;

	static float eval_ease(float t, float t1, float t2);
private:

};


