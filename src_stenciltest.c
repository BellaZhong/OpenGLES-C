#include <stdlib.h>
#include "esUtil.h"

typedef struct
{
	// Handle to a program object
	GLuint programObject;
	GLuint singlecolor_prog;
	// Attribute locations
	GLint  positionLoc;

	// Uniform locations
	GLint  mvpLoc;
	// Sampler location
	GLint samplerLoc;

	// Texture handle
	GLuint textureId;

	// Vertex data
	int      numIndices;
	GLfloat *vertices;
	GLfloat *normals;
	GLuint  *indices;

	// Rotation angle
	GLfloat   angle;

	// MVP matrix
	ESMatrix  mvpMatrix;
	ESMatrix  scalemvpMatrix;

} UserData;

///
// Create a simple cubemap with a 1x1 face with a different
// color for each face
GLuint CreateSimpleTextureCubemap()
{
	GLuint textureId;
	// Six 1x1 RGB faces
	GLubyte cubePixels[6][3] =
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
		// Face 5 - White
		255, 255, 255
	};

	// Generate a texture object
	glGenTextures(1, &textureId);

	// Bind the texture object
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	// Load the cube face - Positive X
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, 1, 1, 0,
		GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[0]);

	// Load the cube face - Negative X
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, 1, 1, 0,
		GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[1]);

	// Load the cube face - Positive Y
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, 1, 1, 0,
		GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[2]);

	// Load the cube face - Negative Y
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, 1, 1, 0,
		GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[3]);

	// Load the cube face - Positive Z
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, 1, 1, 0,
		GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[4]);

	// Load the cube face - Negative Z
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, 1, 1, 0,
		GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[5]);

	// Set the filtering mode
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return textureId;

}


///
// Initialize the shader and program object
//
int Init(ESContext *esContext)
{
	UserData *userData = esContext->userData;
	ESMatrix perspective;
	ESMatrix modelview;
	//ESMatrix scaleMatrix;
	GLfloat scale = 1.2;
	float aspect;
	//deltaTime = 1.0;
	char vShaderStr[] =
		"#version 300 es                            \n"
		"layout(location = 0) in vec4 a_position;   \n"
		"layout(location = 1) in vec3 a_normal;     \n"
		"uniform mat4 u_mvpMatrix;                   \n"
		"out vec3 v_normal;                         \n"
		"void main()                                \n"
		"{                                          \n"
		"   gl_Position = u_mvpMatrix * a_position;  \n"
		"   v_normal = a_normal;                    \n"
		"}                                          \n";
	//"uniform mat4 u_mvpMatrix;                   \n"
	//"attribute vec4 a_position;                  \n"
	//"void main()                                 \n"
	//"{                                           \n"
	//"   gl_Position = u_mvpMatrix * a_position;  \n"
	//"}										     \n";

	char fShaderStr[] =
		"#version 300 es                                     \n"
		"precision mediump float;                            \n"
		"in vec3 v_normal;                                   \n"
		"layout(location = 0) out vec4 outColor;             \n"
		"uniform samplerCube s_texture;                      \n"
		"void main()                                         \n"
		"{                                                   \n"
		"   outColor = texture( s_texture, v_normal );       \n"
		"}                                                   \n";

	char singlecolor_fShader[] =
		"#version 300 es                                     \n"
		"precision mediump float;                            \n"
		"in vec3 v_normal;                                   \n"
		"layout(location = 0) out vec4 outColor;             \n"
		//"uniform samplerCube s_texture;                      \n"
		"void main()                                         \n"
		"{                                                   \n"
		"   outColor = vec4( 1.0, 1.0, 1.0, 1.0 );       \n"
		"}                                                   \n";
	//"precision mediump float;                            \n"
	//"void main()                                         \n"
	//"{                                                   \n"
	//"  gl_FragColor = vec4( 1.0, 1.0, 0.0, 1.0 );        \n"
	//"}                                                   \n";

	GLuint programObject;
	GLuint singlecolor_prog;
	// Load the shaders and get a linked program object
	programObject = esLoadProgram(vShaderStr, fShaderStr);
	if (programObject == 0)
	{
		return GL_FALSE;
	}
	singlecolor_prog = esLoadProgram(vShaderStr, singlecolor_fShader);
	if (singlecolor_prog == 0)
	{
		return GL_FALSE;
	}
	userData->programObject = programObject;
	userData->singlecolor_prog = singlecolor_prog;
	// Get the attribute locations
	userData->positionLoc = glGetAttribLocation(userData->programObject, "a_position");

	// Get the uniform locations
	userData->mvpLoc = glGetUniformLocation(userData->programObject, "u_mvpMatrix");
	// Get the sampler locations
	userData->samplerLoc = glGetUniformLocation(userData->programObject, "s_texture");

	// Load the texture
	userData->textureId = CreateSimpleTextureCubemap();
	// Generate the vertex data
	userData->numIndices = esGenCube(1.0, &userData->vertices, &userData->normals, NULL, &userData->indices);

	// Starting rotation angle for the cube
	userData->angle = 60.0f;
	// Compute the window aspect ratio
	aspect = (GLfloat)esContext->width / (GLfloat)esContext->height;

	// Generate a perspective matrix with a 60 degree FOV
	esMatrixLoadIdentity(&perspective);
	esPerspective(&perspective, 80.0f, aspect, 1.0f, 10.0f);

	// Generate a model view matrix to rotate/translate the cube
	esMatrixLoadIdentity(&modelview);

	// Translate away from the viewer
	esTranslate(&modelview, 0.0, 0.0, -2.0);

	// Rotate the cube
	esRotate(&modelview, userData->angle, 1.0, 1.0, 1.0);

	// Compute the final MVP by multiplying the
	// modevleiw and perspective matrices together
	esMatrixMultiply(&userData->mvpMatrix, &modelview, &perspective);
	memcpy(&userData->scalemvpMatrix, &userData->mvpMatrix, sizeof(userData->mvpMatrix));
	esScale(&userData->scalemvpMatrix, scale, scale, scale);
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

	glEnable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);//depth test always fail
	glClear(GL_STENCIL_BUFFER_BIT);

	//first draw outline
	glStencilMask(0xFF);//enable stencil write
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	// stencil fail, keep the stencil buffer value
	// stencil pass, depth fail, keep the stencil buffer value
	// stencil pass, depth pass, repace the stencil buffer value to 1
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	// update first object stencil buffer as 1, output all the fragments
	// we want all the frag pass the stencil test and depth test
	// thus, we would follow the rules GL_REPLACE, to ref 1
		
	// Use the program object
	glUseProgram(userData->singlecolor_prog);
	// Load the vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT,
		GL_FALSE, 0, userData->vertices);
	// Load the normal
	glVertexAttribPointer(1, 3, GL_FLOAT,
		GL_FALSE, 0, userData->normals);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Load the MVP matrix
	glUniformMatrix4fv(userData->mvpLoc, 1, GL_FALSE, (GLfloat*)&userData->scalemvpMatrix.m[0][0]);
	// Set the sampler texture unit to 0
	glUniform1i(userData->samplerLoc, 0);
	// Draw the cube
	glDrawElements(GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, userData->indices);

	//////////////////////////////////////////////////////
	//second draw object
	glStencilFunc(GL_ALWAYS, 0, 0x00);//Always passes, 0 bit plane, 0 as mask;
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	// stencil fail, keep the stencil buffer value
	// stencil pass, depth fail, keep the stencil buffer value
	// stencil pass, depth pass, repace the stencil buffer value to 0

	glUseProgram(userData->programObject);
	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, userData->textureId);

	glUniformMatrix4fv(userData->mvpLoc, 1, GL_FALSE, (GLfloat*)&userData->mvpMatrix.m[0][0]);
	glDrawElements(GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, userData->indices);
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

	esCreateWindow(esContext, "Simple Cubemap", 320, 240, ES_WINDOW_RGB);

	if (!Init(esContext))
	{
		return GL_FALSE;
	}
	///esRegisterUpdateFunc(&esContext, Update);
	esRegisterDrawFunc(esContext, Draw);

	esRegisterShutdownFunc(esContext, ShutDown);

	return GL_TRUE;
}