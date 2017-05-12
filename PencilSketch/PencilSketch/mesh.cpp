#include "mesh.h"

Texture::Texture(GLenum TextureTarget, const std::string& filename) {
	m_textureTarget = TextureTarget;
	m_filename = filename;
}

bool Texture::Load() {
	FIBITMAP* tempImg = FreeImage_Load(FreeImage_GetFileType(m_filename.c_str(), 0), m_filename.c_str());
	FIBITMAP* img = FreeImage_ConvertTo32Bits(tempImg);
	FreeImage_Unload(tempImg);

	m_imageInfo.m_width = FreeImage_GetWidth(img);
	m_imageInfo.m_height = FreeImage_GetHeight(img);
	m_imageInfo.byteImg = FreeImage_GetBits(img);
	
	img = NULL;
	FreeImage_Unload(img);

	glGenTextures(1, &m_textureObj);
	glBindTexture(m_textureTarget, m_textureObj);
	glTexImage2D(m_textureTarget, 0, GL_RGBA, m_imageInfo.m_width, m_imageInfo.m_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)m_imageInfo.byteImg);
	glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(m_textureTarget, 0);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	return true;
}

void Texture::Bind(GLenum TextureUnit) {
	glActiveTexture(TextureUnit);
	glBindTexture(m_textureTarget, m_textureObj);
}

Mesh::Mesh() {

}

Mesh::~Mesh() {
	Clear();
}

void Mesh::Clear() {
	for (unsigned int i = 0; i < m_Textures.size(); i++) {
		if (m_Textures[i]) {
			delete m_Textures[i];
			m_Textures[i] = NULL;
		}
	}
}

bool Mesh::LoadMesh(const string &filename) {
	Clear();
	bool ret = false;
	Assimp::Importer Importer;

	const aiScene* pScene = Importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	if (pScene) {
		ret = InitFromScene(pScene, filename);
	}
	else {
		fprintf(stderr, "Assimp Read File Error: %s\n", Importer.GetErrorString());
	}

	return ret;
}

bool Mesh::InitFromScene(const aiScene* pScene, const string &filename) {
	m_Entries.resize(pScene->mNumMeshes);
	m_Textures.resize(pScene->mNumMaterials);

	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh);
	}

#if MTL
	return InitMaterial(pScene, filename);
#else
	return true;
#endif
}

void Mesh::InitMesh(unsigned int index, const aiMesh* paiMesh) {
	m_Entries[index].MaterialIndex = paiMesh->mMaterialIndex;

	vector<Vertex> Vertices;
	vector<unsigned int> Indices;

	const aiVector3D default3D(0.f, 0.f, 0.f);

	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &default3D;

		Vertex temp(
			glm::vec3(pPos->x, pPos->y, pPos->z),
			glm::vec2(pTexCoord->x,pTexCoord->y),
			glm::vec3(pNormal->x,pNormal->y,pNormal->z)
		);

		Vertices.push_back(temp);
	}

	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace &Face = paiMesh->mFaces[i];
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}

	m_Entries[index].Init(Vertices, Indices);
}

void Mesh::MeshEntry::Init(const vector<Vertex>& Vertices, const vector<unsigned int> &Indices) {
	NumIndices = Indices.size();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*NumIndices, &Indices[0], GL_STATIC_DRAW);

}

bool Mesh::InitMaterial(const aiScene* pScene, const string &filename) {
	string::size_type SlashIndex = filename.find_last_of("/");
	string Dir;
	bool ret = false;

	if (SlashIndex == string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = filename.substr(0, SlashIndex);
	}

	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		m_Textures[i] = NULL;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				string fullPath = Dir + "/" + Path.data;
				m_Textures[i] = new Texture(GL_TEXTURE_2D, fullPath.c_str());

				if (!m_Textures[i]->Load()) {
					fprintf(stderr, "Error Load Texture %s\n", fullPath.c_str());
					delete m_Textures[i];
					m_Textures[i] = NULL;
					ret = false;
				}
				else {
					printf("Texture Loaded Success %s\n", fullPath.c_str());
					ret = true;
				}
			}
		}
	}

	return ret;
}

bool Mesh::Render() {
#if 1
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
#endif

	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].ibo);
#if MTL
		const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

		if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
			m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
		}
#endif

		glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
	}
#if 1
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
#endif

	return true;
}
