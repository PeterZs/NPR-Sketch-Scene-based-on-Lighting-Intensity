#pragma once
#include <FreeImage.h>
#include <string>
#include <sstream>
using namespace std;

#include <GL/glew.h>
#include <GL/freeglut.h>

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "FreeImage.lib")

struct imgcontent {
	float m_width;
	float m_height;
	GLubyte* byteImg;
	//GLuint textureObj;
}imgcontent;

void BindTexture(GLuint &textureObj, GLenum texturen) {
	glActiveTexture(texturen);
	glBindTexture(GL_TEXTURE_2D, textureObj);
}

void BindTextureBuffer(GLuint &textureObj) {
	glGenTextures(1, &textureObj);
	glBindTexture(GL_TEXTURE_2D, textureObj);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgcontent.m_width, imgcontent.m_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)imgcontent.byteImg);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void InitializeTexture(const string& filename) {
	FIBITMAP* tempImg = FreeImage_Load(FreeImage_GetFileType(filename.c_str(), 0), filename.c_str());
	FIBITMAP* img = FreeImage_ConvertTo32Bits(tempImg);
	imgcontent.m_width = FreeImage_GetWidth(img);
	imgcontent.m_height = FreeImage_GetHeight(img);
	imgcontent.byteImg = FreeImage_GetBits(img);
	img = NULL;
	FreeImage_Unload(tempImg);
	FreeImage_Unload(img);
}