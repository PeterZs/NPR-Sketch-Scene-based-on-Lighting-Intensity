#pragma once
/*
*	Shader Loader
*	Zhe_CatMachine
*	Feb. 2017
*/

#include <string>
#include <fstream>
#include <GL\glew.h>
#include <GL\freeglut.h>
using namespace std;

#pragma comment(lib,"freeglut.lib")

void InitializeShaderProgram(GLuint& program) {
	GLint ret;
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &ret);
	if (!ret) {
		char log[1024];
		glGetProgramInfoLog(program, sizeof(log), NULL, log);
		fprintf(stderr, "Error: %s\n", log);
	}
	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &ret);
	if (!ret) {
		char log[1024];
		glGetProgramInfoLog(program, sizeof(log), NULL, log);
		fprintf(stderr, "Error: %s\n", log);
	}
}

void AttachShader(GLuint program, const char* text, GLenum shaderType) {
	GLint ret;
	GLuint shader = glCreateShader(shaderType);
	const GLchar* pText[1];
	pText[0] = text;
	GLint len[1];
	len[0] = strlen(text);

	glShaderSource(shader, 1, pText, len);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		char log[1024];
		glGetShaderInfoLog(shader, sizeof(log), NULL, log);
		fprintf(stderr, "%s\n", log);
	}
	glAttachShader(program, shader);
}

bool ReadShaderFile(const char* filename, string* output) {
	bool ret = false;
	ifstream file(filename);

	if (file.is_open()) {
		string line;
		while (getline(file, line)) {
			output->append(line);
			output->append("\n");
		}
		ret = true;
	}

	if (!ret) fprintf(stderr, "Error: read shader file failed");
	return ret;
}