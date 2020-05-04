#version 400
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPositionn;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 ambient_global;

struct Light {
    vec3 Position;
    vec3 Color;    
    float Linear;
    float Quadratic;
    float Radius;	
    vec3 diffuse;
    vec3 specular;
};



const int Total_Lights = 2;
uniform Light lights[Total_Lights];

uniform vec3 viewPos;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPositionn, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
	if(Normal == vec3(0,0,0))
		discard;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a - 1.0f;
	float ka = texture(gPositionn, TexCoords).a - 1.0f;
    float ns = texture(gNormal, TexCoords).a - 1.0f;

    // then calculate lighting as usual
	if(ka == 0.0f)
		ka = 1.0f;

    vec3 lighting  = Diffuse * ka * ambient_global;  
    vec3 viewDir  = normalize(viewPos - FragPos);

    for(int i = 0; i < Total_Lights; ++i)
    {
        // calculate distance between light source and current fragment
        float distance = length(lights[i].Position - FragPos);
        if(distance < lights[i].Radius)
        {
            // diffuse
            vec3 lightDir = normalize(lights[i].Position - FragPos);
            float diff = max(dot(Normal, lightDir), 0.0);
            // specular
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), ns);
			// attenuation
            float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
							
			vec3 diffuse  = lights[i].diffuse  * (diff * attenuation) * Diffuse;
            vec3 specular = lights[i].specular * (spec * attenuation) * Specular;                       
            lighting += diffuse + specular;					
        }
    }    
    FragColor = vec4(lighting, 1.0f);
}