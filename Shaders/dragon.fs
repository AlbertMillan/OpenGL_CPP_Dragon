#version 330

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct DirectionalLight
{
    vec3 lightColor;
    float ambientIntensity;
    vec3 lightSourcePos;
    vec3 viewPos;
};

uniform DirectionalLight gDirectionalLight;
uniform sampler2D texture_diffuse;


void main()
{
    // ambient
	vec4 ambient = vec4( gDirectionalLight.lightColor, 1.0f) *
                   gDirectionalLight.ambientIntensity;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(gDirectionalLight.lightSourcePos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec4 diffuse = vec4(diff * gDirectionalLight.lightColor, 1.0f);

    // specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(gDirectionalLight.viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec4 specular = vec4(specularStrength * spec * gDirectionalLight.lightColor, 1.0f);

    gl_FragColor = vec4( texture(texture_diffuse, TexCoords) ) *
                   (ambient + diffuse + specular);

}
