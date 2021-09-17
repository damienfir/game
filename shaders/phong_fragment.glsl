#version 330


uniform vec3 color;
uniform vec3 viewer_pos;

in vec3 normal;
in vec3 pos;

out vec4 FragColor;

void main(void) {
    const float PI = 3.1415926535897932384626433832795;

    vec3 ambient_color = vec3(0.1, 0.1, 0.1);

    vec3 light_pos = vec3(-4, 30, 5);
    vec3 light_dir = normalize(light_pos - pos);

    float diff = max(0, dot(light_dir, normal));

    vec3 light_dir_reflected = reflect(light_dir, normal);
    vec3 viewer_dir = normalize(viewer_pos - pos);
    int n = 70;
    float spec = ((n + 8) / (8*PI)) * pow(max(0, dot(-light_dir_reflected, viewer_dir)), n);

    vec3 light_color = vec3(1, 1, 1);
    vec3 ambient = 0.1 * light_color;
    vec3 diffuse = 0.7 * diff * light_color;
    vec3 specular = 0.2 * spec * light_color;

    FragColor = vec4((ambient + diffuse + specular) * color, 1.0);
//    FragColor = vec4(1, 1, 1, 1);
}