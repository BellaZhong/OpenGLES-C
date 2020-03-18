// The MIT License (MIT)
//
// Copyright (c) 2013 Dan Ginsburg, Budirijanto Purnomo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//
// Simple_TextureQuadPyramid.c
//
//    This is a simple example that draws a sphere with a cubemap image applied.
//
#include <stdlib.h>
#include "esUtil.h"

typedef struct
{
	// Handle to a program object
	GLuint programObject;
	// Attribute locations
	GLint  positionLoc;

	// Uniform locations
	GLint  mvpLoc;
	// Sampler location
	GLint samplerLoc;

	// Texture handle
	GLuint *textureId;

	// Vertex data
	int      numIndices;
	GLfloat *vertices;
	GLfloat *normals;
	GLuint  *indices;

	// Rotation angle
	GLfloat   angle;

	// MVP matrix
	ESMatrix  mvpMatrix;

} UserData;


GLfloat QuaPryms_vertices[16][3] =
{
	0.0f,0.0f,1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,

	0.0f,0.0f,1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f,1.0f,-1.0f,

	0.0f,0.0f,1.0f,
	1.0f,1.0f,-1.0f,
	-1.0f,1.0f,-1.0f,

	0.0f,0.0f,1.0f,
	-1.0f,1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
};

GLfloat QuaPryms_texcoord[16][2] = {

	0.5f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	0.5f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	0.5f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	0.5f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	//0.0f, 0.0f,
	//1.0f, 1.0f,
	//1.0f, 0.0f,

	//0.0f, 0.0f,
	//0.0f, 1.0f,
	//1.0f, 1.0f,
	0.0f, 0.0f, 
	0.0f, 1.0f, 
	1.0f, 1.0f, 
	1.0f, 0.0f, 
};
GLuint QuaPryms_indices[6][3] =
{
	0,1,2,
	3,4,5,
	6,7,8,
	9,10,11,
	12,13,14,
	12,14,15,
	
};

///
// Create a simple Quadrangular pyramid with different
// color for each surface
// Six 1x1 RGB faces
GLubyte QuaPryms_colors[5][3] =
{
	// Face 0 - Red
	255, 0, 0,
	// Face 1 - Green,
	0, 255, 0,
	// Face 2 - Blue
	0, 0, 255,
	// Face 3 - Yellow
	255, 255, 0,
	// Face 4 - Purple
	255, 0, 255,

};



///
// Initialize the shader and program object
//
int Init(ESContext *esContext)
{
	UserData *userData = esContext->userData;
	ESMatrix perspective;
	ESMatrix modelview;
	GLuint *textureIds;
	float aspect;
	int i = 0;
	//deltaTime = 1.0;
	char vShaderStr[] =
		"#version 300 es                            \n"
		"layout(location = 0) in vec3 a_Position;   \n"
		"layout(location = 1) in vec2 a_normal;     \n"
		"uniform mat4 u_mvpMatrix;                   \n"
		"out vec2 v_normal;                         \n"
		"void main()                                \n"
		"{                                          \n"
		"	gl_Position = vec4(a_Position, 1.0);      \n"
		"   gl_Position = u_mvpMatrix * gl_Position;     \n"
		"   v_normal = a_normal;                    \n"
		"}                                          \n";


	char fShaderStr[] =
		"#version 300 es                                     \n"
		"precision mediump float;                            \n"
		"in vec2 v_normal;                                   \n"
		"layout(location = 0) out vec4 outColor;             \n"
		"uniform sampler2D s_texture;                      \n"
		"void main()                                         \n"
		"{                                                   \n"
		"   outColor = texture( s_texture, v_normal );       \n"
		"}                                                   \n";


	GLuint programObject;
	// Load the shaders and get a linked program object
	programObject = esLoadProgram(vShaderStr, fShaderStr);
	if (programObject == 0)
	{
		return GL_FALSE;
	}
	userData->programObject = programObject;
	// Get the attribute locations
	userData->positionLoc = glGetAttribLocation(userData->programObject, "a_Position");

	// Get the uniform locations
	userData->mvpLoc = glGetUniformLocation(userData->programObject, "u_mvpMatrix");
	// Get the sampler locations
	userData->samplerLoc = glGetUniformLocation(userData->programObject, "s_texture");

	// Load the texture
	// Generate five texture object
	textureIds = malloc(sizeof(GLuint) * 5);
	glGenTextures(5, textureIds);
	for (i = 0; i < 5; i++)
	{
		// Bind the texture object
		GLint temptexid = 0;
		temptexid = *(textureIds + i);
		glBindTexture(GL_TEXTURE_2D, *(textureIds + i));
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0,
			GL_RGB, GL_UNSIGNED_BYTE, &QuaPryms_colors[i]);
		// Set the filtering mode
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	userData->textureId = malloc(sizeof(GLuint) * 5);
	//userData->textureId = textureIds;
	memcpy(userData->textureId, textureIds, sizeof(GLuint) * 5);

	// Generate the vertex data
	userData->vertices = QuaPryms_vertices;
	userData->indices = QuaPryms_indices;
	userData->numIndices = 16;
	userData->normals = QuaPryms_texcoord;
	// Starting rotation angle for the cube
	userData->angle = 0.0f;
	// Compute the window aspect ratio
	aspect = (GLfloat)esContext->width / (GLfloat)esContext->height;

	// Generate a perspective matrix with a 60 degree FOV
	esMatrixLoadIdentity(&perspective);
	esPerspective(&perspective, 90.0f, aspect, 1.0f, 20.0f);

	// Generate a model view matrix to rotate/translate the cube
	esMatrixLoadIdentity(&modelview);

	// Translate away from the viewer
	esTranslate(&modelview, 0.0, 0.0, -2.0);

	// Rotate the cube
	esRotate(&modelview, userData->angle, 1.0, 1.0, 1.0);

	// Compute the final MVP by multiplying the
	// modevleiw and perspective matrices together
	esMatrixMultiply(&userData->mvpMatrix, &modelview, &perspective);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	return TRUE;
}

///
// Draw a triangle using the shader pair created in Init()
//
void Draw(ESContext *esContext)
{
	UserData *userData = esContext->userData;

	// Set the viewport
	glViewport(0, 0, esContext->width, esContext->height);

	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
	// Use the program object
	glUseProgram(userData->programObject);

	//// Load the vertex position
	//glVertexAttribPointer(userData->positionLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), userData->vertices);

	//glEnableVertexAttribArray(userData->positionLoc);
	// Load the vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT,
		GL_FALSE, 0, QuaPryms_vertices);
	// Load the texture coords
	glVertexAttribPointer(1, 2, GL_FLOAT,
		GL_FALSE, 0, QuaPryms_texcoord);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Load the MVP matrix
	glUniformMatrix4fv(userData->mvpLoc, 1, GL_FALSE, (GLfloat*)&userData->mvpMatrix.m[0][0]);
	// Bind the texture
	glActiveTexture(GL_TEXTURE0);

	// Set the sampler texture unit to 0
	glUniform1i(userData->samplerLoc, 0);
	// Draw the QuadPyrimid

	glBindTexture(GL_TEXTURE_2D, *(userData->textureId + 0));
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, &QuaPryms_indices[0]);

	glBindTexture(GL_TEXTURE_2D, *(userData->textureId + 1));
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, &QuaPryms_indices[1]);

	glBindTexture(GL_TEXTURE_2D, *(userData->textureId + 2));
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, &QuaPryms_indices[2]);

	glBindTexture(GL_TEXTURE_2D, *(userData->textureId + 3));
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, &QuaPryms_indices[3]);

	glBindTexture(GL_TEXTURE_2D, *(userData->textureId + 4));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, &QuaPryms_indices[4]);
	//eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
}

///
// Update MVP matrix based on time
//
void Update(ESContext *esContext, float deltaTime)
{
	UserData *userData = (UserData*)esContext->userData;
	ESMatrix perspective;
	ESMatrix modelview;
	float aspect;
	deltaTime = 1.0;
	// Compute a rotation angle based on time to rotate the cube
	//userData->angle += (deltaTime * 40.0f);
	//if (userData->angle >= 360.0f)
	//	userData->angle -= 360.0f;

	// Compute the window aspect ratio
	aspect = (GLfloat)esContext->width / (GLfloat)esContext->height;

	// Generate a perspective matrix with a 60 degree FOV
	esMatrixLoadIdentity(&perspective);
	esPerspective(&perspective, 60.0f, aspect, 1.0f, 20.0f);

	// Generate a model view matrix to rotate/translate the cube
	esMatrixLoadIdentity(&modelview);

	// Translate away from the viewer
	esTranslate(&modelview, 0.0, 0.0, -2.0);

	// Rotate the cube
	esRotate(&modelview, userData->angle, 1.0, 0.0, 1.0);

	// Compute the final MVP by multiplying the
	// modevleiw and perspective matrices together
	esMatrixMultiply(&userData->mvpMatrix, &modelview, &perspective);
}

///
// Cleanup
//
void ShutDown(ESContext *esContext)
{
	UserData *userData = esContext->userData;

	// Delete texture object
	glDeleteTextures(1, &userData->textureId);

	// Delete program object
	glDeleteProgram(userData->programObject);

	free(userData->vertices);
	free(userData->normals);
}


int esMain(ESContext *esContext)
{
	esContext->userData = malloc(sizeof(UserData));

	esCreateWindow(esContext, "Simple Quad Pyramid", 320, 240, ES_WINDOW_RGB);

	if (!Init(esContext))
	{
		return GL_FALSE;
	}
	///esRegisterUpdateFunc(&esContext, Update);
	esRegisterDrawFunc(esContext, Draw);
	
	esRegisterShutdownFunc(esContext, ShutDown);
	
	return GL_TRUE;
}
