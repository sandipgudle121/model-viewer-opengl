#version 460 core

in vec3 aPosition;
in vec3 aNormal;
in vec2 aTexCoord;
in vec4 aColor;

out vec4 oColor;
uniform mat4 uMVPMatrix;

void main(void) {
    gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
    oColor = aColor;
}
