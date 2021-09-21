#version 330


uniform vec3 color;
uniform vec3 viewer_pos;

in vec3 normal;
in vec3 pos;

out vec4 FragColor;

// https://stackoverflow.com/questions/12964279/whats-the-origin-of-this-glsl-rand-one-liner
float rand(vec2 co){
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main(void) {
    const float PI = 3.1415926535897932384626433832795;

    vec3 light_pos = vec3(5, 0, 5);
    vec3 light_dir = normalize(light_pos - pos);

    float diff = max(0, dot(light_dir, normal));

    vec3 light_dir_reflected = reflect(light_dir, normal);
    vec3 viewer_dir = normalize(viewer_pos - pos);
    int n = 70;
    float spec = ((n + 8) / (8*PI)) * pow(max(0, dot(-light_dir_reflected, viewer_dir)), n);

    vec3 light_color = vec3(1, 1, 1);
    vec3 ambient = 0.5 * light_color;
    vec3 diffuse = 0.7 * diff * light_color;
    vec3 specular = 0.2 * spec * light_color;

//    float noise = rand(pos.xy) * 0.2;
    FragColor = vec4((ambient + diffuse + specular) * color, 1.0);
    //    FragColor = vec4(1, 1, 1, 1);
}