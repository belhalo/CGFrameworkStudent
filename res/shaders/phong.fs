uniform vec3 u_ambientLight;
uniform vec3 u_sceneLight_position;
uniform vec3 u_sceneLight_intensity;
uniform vec3 u_camera_position;

uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;
uniform float u_shininess;

varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_world_normal;

void main()
{
    vec3 N = normalize(v_world_normal);
    vec3 L = normalize(u_sceneLight_position - v_world_position);
    vec3 V = normalize(u_camera_position - v_world_position);
    vec3 R = reflect(-L, N);

    float dotLN = max(dot(N, L), 0.0);
    float dotRV = max(dot(R, V), 0.0);

    vec3 Ia = u_Ka * u_ambientLight;
    vec3 Id = u_Kd * dotLN * u_sceneLight_intensity;
    vec3 Is = u_Ks * pow(dotRV, u_shininess) * u_sceneLight_intensity;

    vec3 finalColor = Ia + Id + Is;

    gl_FragColor = vec4(finalColor, 1.0);
}
