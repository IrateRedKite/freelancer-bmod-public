#version 330
/*
* Shader code by Schmackbolzen
* This code has seen a lot of testing and is not finalized.
* Expect some mess...
*/

//layout(depth_less) out float gl_FragDepth;

in vec2  texCoord;
in vec3  lightvec;
in float logz;
in vec4 vertColor;
uniform sampler2D texSampler;
uniform int useTexture;
void main (void)
{	
	vec4 text2D = texture(texSampler, texCoord);
	
	if (useTexture>0)	
	{
	 float alpha=vertColor.a*text2D.a;
	 gl_FragColor = vec4(text2D.rgb*vertColor.rgb,alpha);
	}
	else 
	 gl_FragColor = vertColor;
	//gl_FragDepth = logz;
}