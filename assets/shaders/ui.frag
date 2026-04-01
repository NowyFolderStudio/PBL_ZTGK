#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D u_texture1;
uniform vec4 u_color;

void main() {
    FragColor = texture(u_texture1, TexCoord) * u_color;
}