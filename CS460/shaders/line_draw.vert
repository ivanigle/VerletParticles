#version 400
layout(location = 0) in vec3 vertexPosition;

uniform mat4 projection;
uniform mat4 view;

void main()
{	
	gl_Position =  projection * view * vec4(vertexPosition, 1.0f);
	
}

