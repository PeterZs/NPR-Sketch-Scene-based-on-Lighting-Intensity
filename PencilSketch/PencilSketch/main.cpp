/*
	*	NPR Sketch Scene based on Lighting Intensity
	*	Zhe Zeng
	*	Feb. 2017 @Purdue
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

#include <GL\glew.h>
#include <GL\freeglut.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\rotate_vector.hpp>

#include "LoadShader.h"
#include "mesh.h"
#include "LoadTexture.h"

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"freeglut.lib")
#pragma comment(lib,"assimp.lib")

const char* vertex_shader_name = "./shaders/vertex.vert";
const char* frag_shader_name = "./shaders/fragment.frag";
const string model_file_name = "../resource/bunny.obj";
const string hatchs[6] = {
	"../resource/hatch_0.jpg",
	"../resource/hatch_1.jpg",
	"../resource/hatch_2.jpg",
	"../resource/hatch_3.jpg",
	"../resource/hatch_4.jpg",
	"../resource/hatch_5.jpg"
};

GLuint program;
GLuint proj_loc;
GLuint view_loc;
GLuint model_loc;
GLuint sampler_loc[6];
GLuint ca_loc;
GLuint textureObj[6];
GLuint plane_vao;
GLuint plane_vbo;
GLenum textureTargets[6] = {GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2,  GL_TEXTURE3,  GL_TEXTURE4,  GL_TEXTURE5 };

glm::vec3 plane[4] = {
	glm::vec3(-5.0f, 0.0f, -4.0f),
	glm::vec3(-5.0f, 0.0f, 5.0f),
	glm::vec3(5.0f, 0.0f, 5.0f),
	glm::vec3(5.0f, 0.0f, -4.0f)
};

Mesh* pmesh;
float gtime = 0.f;
bool isPaused = false;

glm::vec3 m_target = glm::vec3(0.f, 0.f, -1.f);
glm::vec3 m_up = glm::vec3(0.f, 1.f, 0.f);
glm::vec3 m_position = glm::vec3(0.f, 2.5f, 4.0f);

glm::mat4 model = glm::mat4(1.f);
glm::mat4 view = glm::mat4(1.f);
glm::mat4 proj = glm::perspective(
	glm::radians(60.f),
	1.f,
	1.0f,
	40.f
);

glm::mat4 lightSpace = glm::mat4(1.f);

void DrawPlane() {
	model = glm::scale(glm::mat4(1.0), glm::vec3(0.01f));
	model = glm::translate(model, glm::vec3(0.0f, -1.5f, 0.0f));	

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));
	glUniform1i(glGetUniformLocation(program, "colMode"), 1);

	glEnableVertexAttribArray(0);
	glBindVertexArray(plane_vao);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableVertexAttribArray(0);
	
}

void DrawObjects() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	view = glm::lookAt(
		m_position,
		glm::vec3(0.0f),//m_position + m_target,
		m_up);
	
	glUniform3fv(ca_loc, 1, glm::value_ptr(m_position));
	for (int i = 0; i < 6; i++) {
		BindTexture(textureObj[i], textureTargets[i]);
		glUniform1i(sampler_loc[i], i);
	}

	model = glm::scale(glm::mat4(1.0), glm::vec3(0.01f));
	model = glm::rotate(model, gtime, glm::vec3(0.f, 1.f, 0.f));

	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));

	glUniform1i(glGetUniformLocation(program, "colMode"), 2);
	pmesh->Render();
}



void Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	DrawObjects();	
	DrawPlane();
	glutSwapBuffers();
}

void Idle() {
	glUseProgram(program);
	if(!isPaused)	gtime += 0.1f;
	//printf("%f\n", gtime);
	glutPostRedisplay();
	
}

void InitShaderProgram(GLuint &program) {
	program = glCreateProgram();
	string vs, fs;
	ReadShaderFile(vertex_shader_name, &vs);
	ReadShaderFile(frag_shader_name, &fs);
	AttachShader(program, vs.c_str(), GL_VERTEX_SHADER);
	AttachShader(program, fs.c_str(), GL_FRAGMENT_SHADER);
	InitializeShaderProgram(program);
	
	proj_loc = glGetUniformLocation(program, "proj");
	view_loc = glGetUniformLocation(program, "view");
	model_loc = glGetUniformLocation(program, "model");
	ca_loc = glGetUniformLocation(program, "ca");
	for (int i = 0; i < 6; i++) {
		stringstream ss;
		ss << "level[" << i <<"]";
		sampler_loc[i] = glGetUniformLocation(program, ss.str().c_str());
	}
}

void InitModel() {
	pmesh = new Mesh();
	if (!pmesh->LoadMesh(model_file_name)) {
		fprintf(stderr, "Error: Load Mesh or MTL Failed");
	}
	
	for (int i = 0; i < 6; i++) {
		InitializeTexture(hatchs[i]);
		BindTextureBuffer(textureObj[i]);
	}
	
	glGenVertexArrays(1, &plane_vao);
	glBindVertexArray(plane_vao);
	glGenBuffers(1, &plane_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void Keyboard(unsigned char input, int x, int y) {
	switch (input)
	{
	case 'r': {
		InitShaderProgram(program);
		break;
	}
	case 'p': {
		if (isPaused)	isPaused = false;
		else			isPaused = true;
		break;
	}
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1024, 1024);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Pencil Sketch");

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: initialize glew failed");
		exit(0);
	}	
	InitShaderProgram(program);
	InitModel();

	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();
	return 0;
}

