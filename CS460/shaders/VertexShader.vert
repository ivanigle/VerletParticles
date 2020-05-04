#version 400
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec4 Weights;
layout (location = 6) in ivec4 BoneIDs;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;

const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool is_anim;


uniform mat4 gBones[MAX_BONES];

void main()
{

	
	vec4 PosL;

	if(is_anim)
	{
		mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
		BoneTransform     += gBones[BoneIDs[1]] * Weights[1];
		BoneTransform     += gBones[BoneIDs[2]] * Weights[2];
		BoneTransform     += gBones[BoneIDs[3]] * Weights[3];
		
		PosL = BoneTransform * vec4(aPos, 1.0);
	}
	else
		PosL = vec4(aPos, 1.0);
	
    vec4 worldPos = model * PosL;
    FragPos = worldPos.xyz; 
    TexCoords = aTexCoords;	
	
	vec3 N = normalize(mat3(model) * aNormal); 
	vec3 T = normalize(mat3(model) * aTangent);
	vec3 B = cross(T, N);
    TBN = mat3(T, B, N);
	
    //mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = aNormal;

    gl_Position = projection * view * worldPos;
}