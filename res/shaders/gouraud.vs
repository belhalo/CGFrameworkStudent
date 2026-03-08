uniform mat4 u_model;
uniform mat4 u_viewprojection;

uniform vec3 u_ambientLight;
uniform vec3 u_sceneLight_position;
uniform vec3 u_sceneLight_intensity;
uniform vec3 u_camera_position;

uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;
uniform float u_shininess;

varying vec3 v_light_color;
varying vec2 v_uv;

void main()
{
    v_uv = gl_MultiTexCoord0.xy;

    vec3 world_position = (u_model * vec4(gl_Vertex.xyz, 1.0)).xyz;
    vec3 world_normal = normalize((u_model * vec4(gl_Normal.xyz, 0.0)).xyz);

    vec3 L = normalize(u_sceneLight_position - world_position);
    vec3 V = normalize(u_camera_position - world_position);
    vec3 R = reflect(-L, world_normal);

    float dotLN = max(dot(world_normal, L), 0.0);
    float dotRV = 0.0;

    if (dotLN > 0.0)
        dotRV = max(dot(R, V), 0.0);

    vec3 Ia = u_Ka * u_ambientLight;
    vec3 Id = u_Kd * dotLN * u_sceneLight_intensity;
    vec3 Is = u_Ks * pow(dotRV, u_shininess) * u_sceneLight_intensity;

    v_light_color = Ia + Id + Is;

    gl_Position = u_viewprojection * vec4(world_position, 1.0);
}
