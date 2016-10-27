#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect dstTex;
uniform float alpha;

in vec2 texCoordVarying;

out vec4 outputColor;

void main (void)
{
	// Get the pixels
	vec4 src = texture( tex0, texCoordVarying );
	vec4 dst = texture( dstTex, gl_FragCoord.xy );
	
	// Modify foreground with opacity
	src.a *= alpha;
	
	// Compute alpha result
	float a = src.a + dst.a * ( 1.0 - src.a );
	
	// Premultiply colors
	vec3 s = src.rgb * src.a;
	vec3 d = dst.rgb * dst.a;
	
	// Blendmode formula
	vec3 rgb = s * d + s * ( 1.0 - dst.a ) + d * ( 1.0 - src.a );
	
	// Unpremultiplier
	float unpre = 1.0 / ( a == 0.0 ? 1.0 : a );
	
	outputColor = vec4( rgb * unpre, a );
}
