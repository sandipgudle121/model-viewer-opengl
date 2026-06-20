#version 460 core

in vec4 oColor;
in vec3 oNormal;
in vec2 oTexCoord;
in vec3 oWorldPosition;

out vec4 FragColor;

uniform int uDebugMode;
uniform sampler2D uAlbedoTexture;
uniform bool uHasAlbedoTexture;
uniform vec3 uLightDirection;
uniform vec4 uLightAmbient;
uniform vec4 uLightDiffuse;
uniform vec4 uLightSpecular;
uniform vec4 uMaterialAmbient;
uniform vec4 uMaterialDiffuse;
uniform vec4 uMaterialSpecular;
uniform float uMaterialShininess;
uniform vec3 uCameraPosition;

void main(void) {
    if (uDebugMode == 1)
    {
        vec3 normalColor = normalize(oNormal) * 0.5 + 0.5;
        FragColor = vec4(normalColor, 1.0);
    }
    else if (uDebugMode == 2 && uHasAlbedoTexture)
    {
        FragColor = texture(uAlbedoTexture, oTexCoord);
    }
    else
    {
        vec3 N = normalize(oNormal);
        vec3 L = normalize(-uLightDirection);
        vec3 V = normalize(uCameraPosition - oWorldPosition);
        vec3 R = reflect(-L, N);

        vec4 ambient = uLightAmbient * uMaterialAmbient;
        float diff = max(dot(N, L), 0.0);
        vec4 diffuse = uLightDiffuse * uMaterialDiffuse * diff;
        float spec = pow(max(dot(R, V), 0.0), max(uMaterialShininess, 1.0));
        vec4 specular = uLightSpecular * uMaterialSpecular * spec;

        vec4 baseColor = uHasAlbedoTexture ? texture(uAlbedoTexture, oTexCoord) : oColor;
        FragColor = vec4(baseColor.rgb * (ambient.rgb + diffuse.rgb) + specular.rgb, baseColor.a);
    }
}
