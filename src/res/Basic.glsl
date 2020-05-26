#version 330 core

#ifdef VERTEX_SHADER

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normals;

out vec3 v_FragPos;
out vec2 v_TexCoord;
out vec3 v_Normals;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
    gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0f);
    v_TexCoord = texCoord;
    v_FragPos = vec3(u_Model * vec4(position, 1.0f));
    v_Normals = mat3(transpose(inverse(u_Model))) * normals;
}

#endif

#ifdef FRAGMENT_SHADER

layout(location = 0) out vec4 color;

in vec3 v_FragPos;
in vec2 v_TexCoord;
in vec3 v_Normals;

uniform vec3 u_CameraPos;
uniform sampler2D u_Texture;
uniform vec3 u_AlbedoColor;
uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform vec3 u_AmbientColor;
uniform vec3 u_SpecularColor;
uniform float u_AmbientIntensity;
uniform float u_LightIntensity;
uniform float u_SpecularIntensity;

void main() {
    // Ambient
    vec3 ambientLight = u_AmbientColor * u_AmbientIntensity;

    // Diffuse
    vec3 posToLightVec = normalize(u_LightPos - v_FragPos);
    float diffuse = clamp(dot(posToLightVec, v_Normals), 0, 1);
    vec3 diffuseLight = diffuse * u_LightColor * u_LightIntensity;

    // Specular
    vec3 viewDir = normalize(u_CameraPos - v_FragPos);
    vec3 reflectDir = reflect(-posToLightVec, v_Normals);
    float specular = pow(clamp(dot(viewDir, reflectDir), 0, 1), 32);
    vec3 specularLight = u_SpecularIntensity * specular * u_SpecularColor;

    vec4 texColor = texture(u_Texture, v_TexCoord);
    color = texColor * vec4(u_AlbedoColor, 1.0f) *
        (vec4(ambientLight, 1.0f) + vec4(diffuseLight, 1.0f) + vec4(specularLight, 1.0f));
}

#endif