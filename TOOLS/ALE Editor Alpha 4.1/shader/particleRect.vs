#version 330
/*
* Shader code by Schmackbolzen
* This code has seen a lot of testing and is not finalized.
* Expect some mess...
*/
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec3 xyz; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 color; 
layout(location = 4) in vec4 rotSizeLengthWidth;
layout(location = 5) in mat3 rotMatPerp;
layout(location = 10) in vec4 TexCoordsMovxMovyScalexScaley;

out vec2 texCoord;
out vec3  eyeSpacePos,lightvec;
out float logz;
out vec4 vertColor;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform vec3 camUp;
uniform vec3 camPos;

float far = 50000.0; 
float C = 1; 
float FC = 1.0/log(far*C + 1);
void main()
{	
	float yPos=squareVertices.y;
	if (rotSizeLengthWidth.x > 0)
		yPos+=0.5;
	float xPos=squareVertices.x*rotSizeLengthWidth.w;
	yPos*=rotSizeLengthWidth.z;
	
	vec3 lookVec = normalize(camPos-xyz);	
	//Billboard:
	/*
	vec3 upVec=camUp;	
	vec3 rightVec=cross(upVec,lookVec);		
	upVec = normalize(cross(lookVec,rightVec));
	*/
	//Axis aligned:
	vec3 upVec=rotMatPerp[0];	
	vec3 rightVec=normalize(cross(upVec,lookVec));		
	lookVec = normalize(cross(rightVec,upVec));
	vec3 pos = vec3(xPos, yPos, 0)*rotSizeLengthWidth.y;
	/*vec3 posZRot=vec3(pos.x * cos(rotAngleZ)-pos.y * sin(rotAngleZ),
				pos.x * sin(rotAngleZ)+pos.y * cos(rotAngleZ),
				squareVertices.z)*rotSizeLengthWidth.y;*/
	vec3 posRot=mat3(rightVec,upVec,lookVec)*pos;
	vec4 position=vec4((posRot+xyz),1.0);	
	
	gl_Position = modelViewProjectionMatrix*position;
	//logz = log(gl_Position.w*C + 1)*FC;
	//gl_Position.z = (2*logz - 1)*gl_Position.w;
	eyeSpacePos=	vec3(modelViewMatrix * position);
	//lightvec=vec3(gl_LightSource[0].position.xyz - eyeSpacePos);
	
	vertColor=color;
	texCoord = (squareVertices.xy+ vec2(0.5, 0.5)+ vec2(TexCoordsMovxMovyScalexScaley.x,TexCoordsMovxMovyScalexScaley.y)) * vec2(TexCoordsMovxMovyScalexScaley.z,TexCoordsMovxMovyScalexScaley.w);
}
