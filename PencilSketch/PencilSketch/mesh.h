#pragma once
#include <string>
#include <vector>
using namespace std;

#include <GL\glew.h>
#include <GL\freeglut.h>

#include <FreeImage.h>

#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"freeglut.lib")
#pragma comment(lib,"assimp.lib")
#pragma comment(lib,"FreeImage.lib")

#define MTL 0

struct Vertex
{
	glm::vec3 m_pos;
	glm::vec2 m_tex;
	glm::vec3 m_normal;

	Vertex(const glm::vec3 &pos, const glm::vec2 &tex, const glm::vec3 normal) {
		m_pos = pos;
		m_tex = tex;
		m_normal = normal;
	}
};

class Texture
{
public:
	Texture(GLenum TextureTarget, const std::string &filename);
	
	bool Load();
	void Bind(GLenum TextureUnit);

private:
	std::string m_filename;
	GLenum m_textureTarget;
	GLuint m_textureObj;
	
	struct ImageInfo
	{
		GLuint m_width;
		GLuint m_height;
		GLubyte* byteImg;
	};

	ImageInfo m_imageInfo;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	bool LoadMesh(const string &filename);
	bool Render();

private:
	bool InitFromScene(const aiScene* pScene, const string &filename);
	void InitMesh(unsigned int index, const aiMesh* paiMesh);
	bool InitMaterial(const aiScene* pScene, const std::string &filename);
	void Clear();

	struct MeshEntry
	{
		void Init(const std::vector<Vertex>& Vertices, const std::vector<unsigned int>& Indices);

		GLuint vbo;
		GLuint ibo;
		unsigned int NumIndices;
		unsigned int MaterialIndex;
	};

	std::vector<MeshEntry> m_Entries;
	std::vector<Texture*> m_Textures;	
};