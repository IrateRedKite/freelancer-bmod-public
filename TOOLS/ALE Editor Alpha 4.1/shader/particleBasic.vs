#version 330
/*
* Shader code by Schmackbolzen
* This code has seen a lot of testing and is not finalized.
* Expect some mess...
*/
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec3 xyz; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 color; 
layout(location = 4) in vec4 rotSizeHVAspectMotionBlur;
layout(location = 5) in mat3 rotMat;
layout(location = 10) in vec4 TexCoordsMovxMovyScalexScaley; 

out vec2 texCoord;
out vec3 lightvec;
out float logz;
out vec4 vertColor;
uniform mat4 modelViewProjectionMatrix;
//uniform mat4 modelViewMatrix;
uniform vec3 camUp;
uniform vec3 camPos;

float PI = 3.14159265358979323846264;
float far = 50000.0; 
float C = 1; 
float FC = 1.0/log(far*C + 1);
void main()
{	
	float rotAngleZ=rotSizeHVAspectMotionBlur.x;
	float posX=squareVertices.x*rotSizeHVAspectMotionBlur.z;
	float posY=squareVertices.y;
	
				
	vec3 lookVec = normalize(camPos-xyz);
	vec3 posRot;
	if (rotSizeHVAspectMotionBlur.w == 0){
	vec3 posZRot=vec3(posX * cos(rotAngleZ)-posY * sin(rotAngleZ),
				posX * sin(rotAngleZ)+posY * cos(rotAngleZ),
				squareVertices.z)*rotSizeHVAspectMotionBlur.y;
	vec3 upVec=camUp;	
	vec3 rightVec=cross(upVec,lookVec);		
	upVec = normalize(cross(lookVec,rightVec));
	lookVec = normalize(cross(rightVec,upVec));
	posRot=mat3(rightVec,upVec,lookVec)*posZRot;
	}
	else{
	/*vec3 unrotatedVec=vec3(0,1,0);
	//rotAngleZ=acos(dot(unrotatedVec,rotMat[0])/sqrt(dot(unrotatedVec,unrotatedVec)*dot(rotMat[0],rotMat[0])));
	rotAngleZ=atan(length(cross(unrotatedVec,rotMat[0])),dot(unrotatedVec,rotMat[0]));
	if ((mat3(modelViewMatrix)*cross(unrotatedVec,rotMat[0])).z<0)
	 rotAngleZ=2*PI-rotAngleZ;
	vec3 posZRot=vec3(posX * cos(rotAngleZ)-posY * sin(rotAngleZ),
				posX * sin(rotAngleZ)+posY * cos(rotAngleZ),
				squareVertices.z)*rotSizeHVAspectMotionBlur.y;
	vec3 upVec=camUp;	
	vec3 rightVec=cross(upVec,lookVec);		
	upVec = normalize(cross(lookVec,rightVec));
	lookVec = normalize(cross(rightVec,upVec));
	posRot=mat3(rightVec,upVec,lookVec)*posZRot;*/
	
	vec3 upVec=rotMat[0];		
	float angle = atan(length(cross(lookVec,upVec)),dot(lookVec,upVec));		
	float stretchVel=sqrt(dot(rotMat[1],rotMat[1]))*0.03*abs(sin(angle));
	//posX +=0.5;
	posY*=rotSizeHVAspectMotionBlur.y;
	if (posY > 0)
	posY+=stretchVel;
	vec3 rightVec=normalize(cross(upVec,lookVec));	
	
	upVec=rightVec;	
	rightVec=cross(upVec,lookVec);		
	upVec = normalize(cross(lookVec,rightVec));
	lookVec = normalize(cross(rightVec,upVec));
	
	vec3 pos = vec3(posY, posX*rotSizeHVAspectMotionBlur.y, 0);
	posRot=mat3(rightVec,upVec,lookVec)*pos;
	//Axis aligned:
	/*vec3 upVec=rotMat[0];	
	vec3 rightVec=normalize(cross(upVec,lookVec));		
	lookVec = normalize(cross(rightVec,upVec));
	vec3 pos = vec3(posX, 0, posY)*rotSizeHVAspectMotionBlur.y;
	 posRot=mat3(rightVec,upVec,lookVec)*pos;*/
	}
	
	vec4 position=vec4((posRot+xyz),1.0);	
	
	gl_Position = modelViewProjectionMatrix*position;
	//logz = log(gl_Position.w*C + 1)*FC;
	//gl_Position.z = (2*logz - 1)*gl_Position.w;
	//eyeSpacePos=	vec3(modelViewMatrix * position);
	//lightvec=vec3(gl_LightSource[0].position.xyz - eyeSpacePos);
	
	vertColor=color;
	texCoord = (squareVertices.xy+ vec2(0.5, 0.5)+ vec2(TexCoordsMovxMovyScalexScaley.x,TexCoordsMovxMovyScalexScaley.y)) * vec2(TexCoordsMovxMovyScalexScaley.z,TexCoordsMovxMovyScalexScaley.w);
}
