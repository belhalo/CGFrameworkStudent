// global var from the cpu -> from the transformation
uniform mat4 u_model; 
uniform mat4 u_viewProjection;

// var from suniformdata
uniform vec3 u_ambientLight;
uniform vec3 u_sceneLight_position;
uniform vec3 u_sceneLight_intensity;
uniform vec3 u_camera_position;

// var from the material
uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;
uniform float u_shininess;

// Variables to pass to the fragment shader
varying vec2 v_uv;
varying vec3 v_light_color; 
varying vec3 v_world_position;
varying vec v_world_normal;

void main()
{	
	v_uv = gl_MultiTexCoord0.xy;

	// convert local position to world space
	vec3 world_position = (u_model * vec4( gl_Vertex.xyz, 1.0)).xyz;

	// convert local normal to world space
	vec3 world_normal = normalize((u_model * vec4( gl_Normal.xyz, 0.0)).xyz);

	vec3 L = normalize(u_sceneLight_position - world_position);
	vec3 V = normalize(u_camera_position - world_position);
	vec3 R = reflect(-L, world_normal);

	float dotLN = max(dot(world_normal,L), 0.0); // we choose the max to avoid negative values (under the line)
	float dotRV = max(dot(R,V), 0.0); 

	vec3 Id = u_Kd * dotLN * u_seceneLight_intensity;
	vec3 Is = u_Ks * pow(dotRV, u_shininess) * u_sceneLight_intensity;
	vec3 Ia = u_Ka * u_ambientLight;

	v_light_color = Id + Is + Ia;

	// pass them to the fragment shader interpolated
	v_world_position = world_position;
	v_world_normal = world_normal;

	// Project the vertex using the model view projection matrix
	gl_Position = u_viewProjection * vec4(world_position, 1.0); //output of the vertex shader
}