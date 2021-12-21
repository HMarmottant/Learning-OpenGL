#shader vertex
#version 410 core

layout(location = 0) in vec4 position;

void main()
{
	gl_Position = position;
};

#shader fragment
#version 410 core

layout(location = 0) out vec4 color;
in vec4 gl_FragCoord;

float DivergenceTest();

uniform vec2 u_Offset;
uniform float u_FragDelta;
uniform float u_Iterations;

void main()
{
	float iter = DivergenceTest();
	color = vec4(1-(iter/u_Iterations),0,0,1);
}

float DivergenceTest()
{
	vec2 c = vec2(u_Offset.x + (gl_FragCoord.x * u_FragDelta), u_Offset.y + (gl_FragCoord.y * u_FragDelta));
	vec2 z = c;
	float i = 0.0;
	while ( i < u_Iterations && dot(z,z)<4.0)
	{
		z = vec2((z.x * z.x) - (z.y * z.y) + c.x, 2.0 * z.x * z.y + c.y);
		i++;
	}
	return i;
}