
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
	GLint viewPos;
	GLint lightColor;
	GLint objectColor;
	GLint modelLoc;
	GLint viewLoc;
	GLint projectionLoc;
	GLint lightPosLoc;

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
	ESMatrix  modelMatrix;
	ESMatrix  viewMatrix;
	ESMatrix  projectionMatrix;

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
	ESMatrix modelMatrix;
	ESMatrix viewMatrix;
	ESMatrix projectionMatrix;
	ESMatrix translateMatrix;
	ESMatrix rotateMatrix;
	ESMatrix scaleMatrix;
	ESMatrix indentityMatrix;

	float aspect;
	//deltaTime = 1.0;
	char vShaderStr[] =
		"#version 300 es                            \n"
		"layout(location = 0) in vec4 a_position;   \n"
		"layout(location = 1) in vec3 a_normal;     \n"
		"uniform mat4 u_mvpMatrix;                   \n"
		"uniform mat4 model;                        \n"
		"uniform mat4 view;                         \n"
		"uniform mat4 projection;                   \n"
		"out vec3 v_normal;                         \n"//texture coord
		"out vec3 fragPos;                          \n"
		"out vec3 normal;                           \n"
		"void main()                                \n"
		"{                                          \n"
		//"   gl_Position = u_mvpMatrix * a_position;  \n"
		"   v_normal = a_normal;                    \n"
		"   fragPos = vec3(model * a_position);\n"
		"   normal = mat3(transpose(inverse(model))) * a_normal; \n"
		//"   gl_Position = projection * view * vec4(fragPos, 1.0);\n"
		"   gl_Position = u_mvpMatrix * a_position;  \n"
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
		"in vec3 fragPos;                                    \n"
		"in vec3 normal;                                     \n"
		"layout(location = 0) out vec4 outColor;             \n"
		"uniform samplerCube s_texture;                      \n"
		"uniform vec3 lightPos;                              \n"
		"uniform vec3 viewPos;                               \n"
		"uniform vec3 lightColor;                            \n"
		"uniform vec3 objectColor;                           \n"

		"float myPow(float x, int r) {                       \n"
		"    float result = 1.0;                                 \n"
		"    for (int i = 0; i < r; i = i + 1) {                 \n"
		"	       result = result * x;                             \n"
		"    }                                                   \n"
		"    return result;                                      \n"
		"}                                                   \n"
		"void main()                                         \n"
		"{                                                   \n"
		/* point light fs
		"   vec3 texColor = texture( s_texture, v_normal ).rgb; \n"
		"	// ambient                                          \n"
		"	float ambientStrength = 0.5;                        \n"
		"	vec3 ambient = ambientStrength * lightColor;        \n"

		"	// diffuse                                          \n"
		"	vec3 norm = normalize(normal);                      \n"
		"	vec3 lightDir = normalize(lightPos - fragPos);      \n"
		"	float diff = max(dot(norm, lightDir), 0.0);         \n"
		"	vec3 diffuse = diff * lightColor;                   \n"

		"	// specular                                         \n"
		"	float specularStrength = 2.0;                       \n"
		"	vec3 viewDir = normalize(viewPos - fragPos);        \n"
		"	vec3 reflectDir = reflect(-lightDir, norm);         \n"
		"	float spec = myPow(max(dot(viewDir, reflectDir), 0.0), 16);\n"
		"	vec3 specular = specularStrength * spec * lightColor;\n"

		"	// attenuation                                      \n"
		"	float dist = length(lightPos - fragPos);            \n"
		"	float kc = 0.1;                                     \n"
		"	float kl = 0.05;                                    \n"
		"	float kq = 0.05;                                    \n"
		"	float attenuation = 1.0 / (kc + kl * dist + kq * (dist * dist));\n"
		"	ambient *= attenuation;                             \n"
		"	diffuse *= attenuation;                             \n"
		"	specular *= attenuation;                            \n"

		"	vec3 result = (ambient + diffuse + specular) * texColor;\n"
		"	outColor = vec4(result, 1.0);                      \n"
		*/
		"	vec3 texColor = texture(s_texture, v_normal).rgb; \n"
		"	// ambient                                        \n"
		"	float ambientStrength = 0.5;                      \n"
		"	vec3 ambient = ambientStrength * lightColor;      \n"

		"	vec3 spotDir = vec3(-2.0, 0.0, 0.0);              \n"
		"	vec3 lightDir = normalize(lightPos - fragPos);    \n"
		"	float theta = dot(lightDir, normalize(-spotDir)); \n"

		"	// diffuse                                        \n"
		"	vec3 norm = normalize(normal);                    \n"
		"	float diff = max(dot(norm, lightDir), 0.0);       \n"
		"	vec3 diffuse = diff * lightColor;                 \n"

		"	// specular                                       \n"
		"	float specularStrength = 2.0;                     \n"
		"	vec3 viewDir = normalize(viewPos - fragPos);      \n"
		"	vec3 reflectDir = reflect(-lightDir, norm);       \n"
		"	float spec = myPow(max(dot(viewDir, reflectDir), 0.0), 16);\n"
		"	vec3 specular = specularStrength * spec * lightColor;\n"

		"	// spotlight (soft edges)                         \n"
		"	float epsilon = 0.15;                             \n"
		"	float intensity = clamp((theta - 0.85) / epsilon, 0.0, 1.0);\n"
		"	diffuse *= intensity;                             \n"
		"	specular *= intensity;                            \n"

		"	// attenuation                                    \n"
		"	float dist = length(lightPos - fragPos);          \n"
		"	float kc = 0.1;                                   \n"
		"	float kl = 0.1;                                   \n"
		"	float kq = 0.1;                                   \n"
		"	float attenuation = 1.0 / (kc + kl * dist + kq * (dist * dist));\n"
		"	ambient *= attenuation;                           \n"
		"	diffuse *= attenuation;                           \n"
		"	specular *= attenuation;                          \n"

		"	vec3 result = (ambient + diffuse + specular) * texColor;\n"
		"	outColor = vec4(result, 1.0);                     \n"
		"}                                                   \n";
	//"precision mediump float;                            \n"
	//"void main()                                         \n"
	//"{                                                   \n"
	//"  gl_FragColor = vec4( 1.0, 1.0, 0.0, 1.0 );        \n"
	//"}                                                   \n";

	GLuint programObject;
	// Load the shaders and get a linked program object
	programObject = esLoadProgram(vShaderStr, fShaderStr);
	if (programObject == 0)
	{
		return GL_FALSE;
	}
	userData->programObject = programObject;
	// Get the attribute locations
	userData->positionLoc = glGetAttribLocation(userData->programObject, "a_position");

	userData->viewPos = glGetUniformLocation(userData->programObject, "viewPos");
	userData->lightColor = glGetUniformLocation(userData->programObject, "lightColor");
	userData->objectColor = glGetUniformLocation(userData->programObject, "objectColor");
	userData->modelLoc = glGetUniformLocation(userData->programObject, "model");
	userData->viewLoc = glGetUniformLocation(userData->programObject, "view");
	userData->projectionLoc = glGetUniformLocation(userData->programObject, "projection");
	// Get the uniform locations
	userData->mvpLoc = glGetUniformLocation(userData->programObject, "u_mvpMatrix");
	// Get the sampler locations
	userData->samplerLoc = glGetUniformLocation(userData->programObject, "s_texture");

	userData->lightPosLoc = glGetUniformLocation(userData->programObject, "lightPos");
	// Load the texture
	userData->textureId = CreateSimpleTextureCubemap();
	// Generate the vertex data
	userData->numIndices = esGenCube(1.0, &userData->vertices, &userData->normals, NULL, &userData->indices);

	esMatrixLoadIdentity(&modelMatrix);
	esMatrixLoadIdentity(&viewMatrix);
	esMatrixLoadIdentity(&projectionMatrix);
	esMatrixLoadIdentity(&indentityMatrix);

	//esTranslate(&translateMatrix, 0.0f, 0.0f, 0.0f);
	//esRotate(&rotateMatrix, 0.0f, 1.0f, 0.0f, 0.0f);
	//esRotate(&rotateMatrix, -45.0f, 0.0f, 1.0f, 0.0f);
	//esRotate(&rotateMatrix, 30.0f, 0.0f, 0.0f, 1.0f);
	//esScale(&scaleMatrix, 1.0f, 1.0f, 1.0f);
	//esMatrixMultiply(&modelMatrix, &rotateMatrix, &scaleMatrix);
	//esMatrixMultiply(&userData->modelMatrix, &modelMatrix, &translateMatrix);
	//esMatrixLookAt(&viewMatrix,
	//	0.0f, 0.0f, 5.0f,
	//	0.0f, 0.0f, 0.0f,
	//	0.0f, 1.0f, 0.0f);
	//esFrustum(&projectionMatrix, -1.0f, 1.0f, -1.0f, 1.0f, 2.0f, 100.0f);
	//esMatrixMultiply(&userData->mvpMatrix, &viewMatrix, &modelMatrix);
	//esMatrixMultiply(&userData->mvpMatrix, &projectionMatrix, &userData->mvpMatrix);
	//esMatrixMultiply(&userData->modelMatrix, &modelMatrix, &indentityMatrix);
	//esMatrixMultiply(&userData->viewMatrix, &viewMatrix, &indentityMatrix);
	//esMatrixMultiply(&userData->projectionMatrix, &projectionMatrix, &indentityMatrix);
	//
	
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
	esMatrixMultiply(&userData->modelMatrix, &modelview, &indentityMatrix);
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
	// Use the program object
	glUseProgram(userData->programObject);

	//// Load the vertex position
	//glVertexAttribPointer(userData->positionLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), userData->vertices);

	//glEnableVertexAttribArray(userData->positionLoc);
	// Load the vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT,
		GL_FALSE, 0, userData->vertices);
	// Load the normal
	glVertexAttribPointer(1, 3, GL_FLOAT,
		GL_FALSE, 0, userData->normals);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glUniform3f(userData->viewPos, 0.0f, 0.0f, 5.0f);
	glUniform3f(userData->lightColor, 1.0f, 1.0f, 1.0f);
	glUniform3f(userData->objectColor, 1.0f, 1.0f, 0.0f);
	glUniform3f(userData->lightPosLoc, 2.0f, -1.0f, -1.0f);
	// Load the MVP matrix
	glUniformMatrix4fv(userData->modelLoc, 1, GL_FALSE, (GLfloat*)&userData->modelMatrix.m[0][0]);
	glUniformMatrix4fv(userData->viewLoc, 1, GL_FALSE, (GLfloat*)&userData->viewMatrix.m[0][0]);
	glUniformMatrix4fv(userData->projectionLoc, 1, GL_FALSE, (GLfloat*)&userData->projectionMatrix.m[0][0]);
	glUniformMatrix4fv(userData->mvpLoc, 1, GL_FALSE, (GLfloat*)&userData->mvpMatrix.m[0][0]);
	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, userData->textureId);

	// Set the sampler texture unit to 0
	glUniform1i(userData->samplerLoc, 0);
	// Draw the cube
	glDrawElements(GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, userData->indices);

	//eglSwapBuffers(esContext->eglDisplay, esContext->eglSurface);
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