#version 330

uniform vec3 color;
in vec3 normal;
in vec3 pos;

out vec4 FragColor;

void main(void) {
    vec3 light_pos = vec3(10, 10, 20);
    float ambient = 0.2;

    float diffuse = max(0, dot(normalize(light_pos - pos), normal));

    FragColor = vec4((diffuse + ambient) * color, 1.0);
}