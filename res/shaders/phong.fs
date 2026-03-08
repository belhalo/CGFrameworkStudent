uniform mat4 u_model;

uniform vec3 u_ambientLight;
uniform vec3 u_sceneLight_position;
uniform vec3 u_sceneLight_intensity;
uniform vec3 u_camera_position;

uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;
uniform float u_shininess;

uniform sampler2D u_texture;
uniform sampler2D u_specular_texture;
uniform sampler2D u_normal_texture;

uniform int u_use_color_texture;
uniform int u_use_specular_texture;
uniform int u_use_normal_texture;
uniform int u_use_ambient;

varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_world_normal;

void main()
{
    vec3 N = normalize(v_world_normal);

    if (u_use_normal_texture == 1)
    {
        vec3 normalTex = texture2D(u_normal_texture, v_uv).rgb;
        normalTex = normalTex * 2.0 - 1.0;

        vec3 mappedNormal = normalize((u_model * vec4(normalTex, 0.0)).xyz);
        N = normalize(mix(N, mappedNormal, 0.55));
    }

    vec3 Ka = u_Ka;
    vec3 Kd = u_Kd;
    vec3 Ks = u_Ks;

    vec4 tex = texture2D(u_texture, v_uv);

    if (u_use_color_texture == 1)
    {
        Ka = tex.rgb;
        Kd = tex.rgb;
    }

    if (u_use_specular_texture == 1)
    {
        vec3 specTex = texture2D(u_specular_texture, v_uv).aaa;
        Ks = 1.8 * specTex;
    }

    vec3 L = normalize(u_sceneLight_position - v_world_position);
    vec3 V = normalize(u_camera_position - v_world_position);
    vec3 R = reflect(-L, N);

    float dotLN = max(dot(N, L), 0.0);
    float dotRV = 0.0;

    if (dotLN > 0.0)
        dotRV = max(dot(R, V), 0.0);

    vec3 Ia = vec3(0.0);
    if (u_use_ambient == 1)
        Ia = Ka * u_ambientLight;

    vec3 Id = Kd * dotLN * u_sceneLight_intensity;
    vec3 Is = 0.35 * Ks * pow(dotRV, u_shininess) * u_sceneLight_intensity * dotLN;

    vec3 finalColor = Ia + Id + Is;
    finalColor = clamp(finalColor, 0.0, 1.0);

    gl_FragColor = vec4(finalColor, 1.0);
}
