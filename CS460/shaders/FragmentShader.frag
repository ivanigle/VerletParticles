#version 400
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_normal;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
};
uniform Material mat;

uniform bool no_textures;


void main()
{    
    // store the fragment position vector in the first gbuffer texture
	
    gPosition = vec4(FragPos, mat.ambient.x + 1.0f);
    // also store the per-fragment normals into the gbuffer

	vec3 normal;

	if(!no_textures)
	{
		normal = normalize(TBN * texture(texture_normal, TexCoords).rgb * 2.0 - vec3(1.0));
	}else
	{
		normal = normalize(Normal);
	}

	gNormal = vec4(normal, mat.shininess + 1.0f);


    // and the diffuse per-fragment color

	vec4 diff_color = texture(texture_diffuse, TexCoords);

	if(!no_textures && diff_color.a < 0.5)	
		discard;

	if(no_textures)
		gAlbedoSpec.rgb = mat.diffuse.rgb;
	else
		gAlbedoSpec.rgb = texture(texture_diffuse, TexCoords).rgb * mat.diffuse.rgb;

    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(texture_specular, TexCoords).r * mat.specular.r + 1.0f;
}