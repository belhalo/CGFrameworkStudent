// This variables comes from the vertex shader
// They are baricentric interpolated by pixel according to the distance to every vertex
varying vec3 v_light_color;
varying vec2 v_uv;

void main()
{
	// Set the ouput color per pixel

	gl_FragColor = vec4(v_light_color, 1.0);
}
