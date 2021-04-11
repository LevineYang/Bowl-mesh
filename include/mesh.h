#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "shaders.h"



using uint = unsigned int;

#ifdef ANIMATION
#define MAX_BONE_INFLUENCE 4
using Vertex = struct Vertex_
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoords;
	//bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];

	Vertex_(const glm::vec3& position_, const glm::vec3& normal_, const glm::vec2& texcoords_) :
		position(position_), normal(normal_), texcoords(texcoords_) { }
};
#else
using Vertex = struct Vertex_
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoords;
	glm::vec3 tangent; // tangent
	glm::vec3 bitangent; // bitangent

	Vertex_(const glm::vec3& position_, const glm::vec3& normal_, const glm::vec2& texcoords_,
		const glm::vec3& tangent_ = glm::vec3(), const glm::vec3& bitangent_ = glm::vec3()) :
		position(position_), normal(normal_), texcoords(texcoords_), tangent(tangent_), bitangent(bitangent_) { }
};
#endif


using TexType = enum {
	tex_DIFFUSE,
	tex_SPECULAR,
	tex_NORMAL,
	tex_HEIGHT,
	tex_UNKNOWN
};

std::string TexGetNameByType(const TexType ttype);

using Texture = struct Texture_
{
	uint id;
	TexType type;
	std::string name;
	std::string path; // store path of texture to compare with other textures
	Texture_(const uint id_, const TexType type_, const std::string& name_, const std::string& path_=std::string()) :
		id(id_), type(type_), name(name_), path(path_) {}
	Texture_() : id(0), type(tex_UNKNOWN) {}
};



class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<uint> indices;
	std::vector<Texture> textures;
	Mesh(const std::vector<Vertex>& vertices_, const std::vector<uint>& indices_, const std::vector<Texture>& textures_) :
		vertices(vertices_), indices(indices_), textures(textures_) {initMesh();}
	~Mesh() { }
	Mesh& operator=(const Mesh&) = delete;
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept = default;
	Mesh& operator=(Mesh&&) = default;

	void Draw(Shader& shader)
	{
		size_t diffuseNr = 1;
		size_t specularNr = 1;
		size_t normalNr = 1;
		size_t heightNr = 1;
		for (size_t i = 0; i < textures.size(); ++i) {
			// activate texture unit
			glActiveTexture(GL_TEXTURE0 + i); 
			// retrieve texture number (the N in diffuse_textureN)
			std::string number;
			TexType name = textures[i].type;
			if (name == tex_DIFFUSE)
				number = std::to_string(diffuseNr++);
			else if(name == tex_SPECULAR)
				number = std::to_string(specularNr++);
			else if (name == tex_NORMAL)
				number = std::to_string(normalNr++);
			else if (name == tex_HEIGHT)
				number = std::to_string(heightNr++);

			shader.setInt((textures[i].name + number), i); // "materal."
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		
		//draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}
	void clearBuffers() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
	uint getVAO() const { return VAO; }
	uint getVBO() const { return VBO; }
	uint getEBO() const { return EBO; }
private:
	uint VAO;
	uint VBO;
	uint EBO;
	void initMesh()
	{
		// create a vertex array object 
		glGenVertexArrays(1, &VAO);
		// create a vertex buffer object
		glGenBuffers(1, &VBO);
		// create a element buffer object
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

		// vertex position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		// vertex normals position
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoords));

		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

		// unbound
		// glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};


std::string TexGetNameByType(const TexType ttype)
{
	switch (ttype)
	{
	case tex_DIFFUSE:
		return std::string{ "texture_diffuse" };
	case tex_SPECULAR:
		return std::string{ "texture_specular" };
	case tex_NORMAL:
		return std::string{ "texture_normal" };
	case tex_HEIGHT:
		return std::string{ "texture_height" };
	default:
		return std::string();
		break;
	}
}