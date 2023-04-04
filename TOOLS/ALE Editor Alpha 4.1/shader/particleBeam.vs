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
layout(location = 8) in vec4 startColor;
layout(location = 9) in vec4 startIsFirstSizeLengthWidth;
layout(location = 10) in vec4 TexCoordsMovxMovyScalexScaley;

out vec2 texCoord;
out vec3  eyeSpacePos,lightvec;
out float logz;
out vec4 vertColor;
out float transparencyWeight;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform vec3 camLook;
uniform vec3 camPos;
uniform int pass;

float far = 50000.0; 
float C = 1; 
float FC = 1.0/log(far*C + 1);
void main()
{	
	//float scale=rotSizeLengthWidth.w*rotSizeLengthWidth.y;
	float width;
	float length=1;
	vec3 upVec;	
	vec3 lookVec=normalize(camLook);	
	vec3 pos3D;
	float xPos=squareVertices.x;
	float yPos=(squareVertices.y+0.5);
	transparencyWeight = 2-abs(xPos*2);
	if (squareVertices.y > 0 || startIsFirstSizeLengthWidth.x > 0)
	{
		upVec=rotMatPerp[0];
		pos3D=xyz;
		//lookVec = normalize(camPos-pos3D*100);			
		width=rotSizeLengthWidth.w*rotSizeLengthWidth.y;
		if (startIsFirstSizeLengthWidth.x > 0)
		length=rotSizeLengthWidth.z;//*rotSizeLengthWidth.y;
		vertColor=color;
		//vertColor=vec4(0,1,0,1);
	}
	else{
		pos3D=rotMatPerp[2];
		upVec=rotMatPerp[1];	
		//lookVec = normalize(camPos-pos3D*100);		
		width=startIsFirstSizeLengthWidth.w*startIsFirstSizeLengthWidth.y;
		length=startIsFirstSizeLengthWidth.z;//*startSizeLengthWidth.x;
		vertColor=startColor;//vec4(color.rgb,1);
		//vertColor=vec4(1,0,0,1);//startColor;//vec4(color.rgb,1);
		yPos+=1;
	}
	xPos*=width;
	yPos*=length;
	//pos3D=xyz;
		
	
	
	vec3 rightVec=normalize(cross(upVec,lookVec));		
	lookVec = normalize(cross(rightVec,upVec));
	vec3 pos2D = vec3(xPos, yPos, 0);
	if (pass>0){
		//Rotate around Y Axis		
		pos2D=vec3(pos2D.x * 0,
				pos2D.y,
				-pos2D.x * 1);			
	}
	vec3 posRot=mat3(rightVec,upVec,lookVec)*pos2D;
	vec4 position=vec4((posRot+pos3D),1.0);	
	
	gl_Position = modelViewProjectionMatrix*position;
	//logz = log(gl_Position.w*C + 1)*FC;
	//gl_Position.z = (2*logz - 1)*gl_Position.w;
	eyeSpacePos=	vec3(modelViewMatrix * position);
	//lightvec=vec3(gl_LightSource[0].position.xyz - eyeSpacePos);
	//vertColor=color;
	
	texCoord = (squareVertices.xy+ vec2(0.5, 0.5)+ vec2(TexCoordsMovxMovyScalexScaley.x,TexCoordsMovxMovyScalexScaley.y)) * vec2(TexCoordsMovxMovyScalexScaley.z,TexCoordsMovxMovyScalexScaley.w);

}
