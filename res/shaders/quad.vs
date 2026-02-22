varying vec2 v_uv;

void main()
{	
	// Remember the UV's range [0.0, 1.0]
	v_uv = gl_MultiTexCoord0.xy;

	// Rebre les coordenades de textura (UVs)
	v_uv = gl_MultiTexCoord0.xy;

	// these are two example of tries
	//gl_Position = vec4(gl_Vertex.xyz, 0.0, 1.0);
	//gl_Position = vec4(v_uv, 1.0, 1.0);
}