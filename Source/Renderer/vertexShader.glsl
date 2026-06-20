#version 460 core

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;
in vec4 aColor;

out vec4 oColor;
out vec3 oNormal;
out vec2 oTexCoord;
out vec3 oWorldPosition;

uniform mat4 uMVPMatrix;
uniform mat4 uModelMatrix;

void main(void) {
    vec4 worldPosition = uModelMatrix * vec4(aPosition, 1.0);
    gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
    oColor = aColor;
    oNormal = mat3(uModelMatrix) * aNormal;
    oTexCoord = aTexCoord;
    oWorldPosition = worldPosition.xyz;
}
