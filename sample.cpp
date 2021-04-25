#include "include/sample.hpp"
#include "include/EllipticParaboloid.hpp"
#include "include/Disk.hpp"
#include "include/Bowl.hpp"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "include/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"



extern Camera cam_;
extern float aspect_ratio;
extern Shader SVshader;


using point3 = glm::vec3;
using byte = unsigned char;

glm::mat4 view(1.f);
glm::mat4 projection(1.f);

void renderBowl();
void renderEllipticParaboloid();
void renderDisk();
void renderPartBowl();
void renderQuad();
GLuint loadTexture(const char* path, const bool gammaCorrection = false, const bool isHdr = false);

static GLuint woodTexture = 0;
static bool loadText = false;


void displayDEMO(GLFWwindow* window)
{
	float timeValue = glfwGetTime();
	glm::mat4 model(1.f);
	view = cam_.getView();
	projection = glm::perspective(glm::radians(cam_.getCamZoom()), aspect_ratio, 0.1f, 100.f);

	SVshader.useProgramm();
	SVshader.setMat4("model", model);
	SVshader.setMat4("view", view);
	SVshader.setMat4("projection", projection);

#define NO_TEXTURE
#ifdef NO_TEXTURE
	if (!loadText) {
		stbi_set_flip_vertically_on_load(true);
		//woodTexture = loadTexture("resource/camres.png");
		woodTexture = loadTexture("resource/test.jpg");
		if (woodTexture == 0)
			exit(EXIT_FAILURE);
		SVshader.setInt("useTexture", 0);
		loadText = true;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	
#endif
	//renderDisk();
	//renderEllipticParaboloid();
	renderBowl();
	//renderPartBowl();
	//renderQuad();

	// unbound
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}



GLuint PartBowlVAO[4]{ 0 };
GLuint PartBowlVBO[4];
GLuint PartBowlEBO[4];
uint indexPartBowl[4];
void renderPartBowl()
{
	if (PartBowlVAO[0] == 0)
	{
		glGenVertexArrays(4, &PartBowlVAO[0]);

		glGenBuffers(4, &PartBowlVBO[0]);
		glGenBuffers(4, &PartBowlEBO[0]);

		auto inner_radius = 0.4f;
		auto radius = 0.7f;
		auto a = 0.5f;
		auto b = 0.5f;
		auto c = 0.7f;

		PartitionBowl bowl(inner_radius, radius, a, b, c);

		std::vector<std::vector<float>> data;
		std::vector<std::vector<uint>> idxs;

		//bowl.generate_mesh(4, 40.f, data, idxs);
		bowl.generate_mesh_uv(4, 40.f, data, idxs);


		for (auto i = 0; i < idxs.size(); ++i)
			indexPartBowl[i] = idxs.at(i).size();

		constexpr float stride = (3 + 2) * sizeof(float);

		glBindVertexArray(PartBowlVAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, PartBowlVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, data[0].size() * sizeof(float), &data[0][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PartBowlEBO[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexPartBowl[0] * sizeof(uint), &idxs[0][0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

		/*
		glBindVertexArray(PartBowlVAO[1]);
		glBindBuffer(GL_ARRAY_BUFFER, PartBowlVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, data[1].size() * sizeof(float), &data[1][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PartBowlEBO[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexPartBowl[1] * sizeof(uint), &idxs[1][0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		
		glBindVertexArray(PartBowlVAO[2]);
		glBindBuffer(GL_ARRAY_BUFFER, PartBowlVBO[2]);
		glBufferData(GL_ARRAY_BUFFER, data[2].size() * sizeof(float), &data[2][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PartBowlEBO[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexPartBowl[2] * sizeof(uint), &idxs[2][0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		*/
	}

	glBindVertexArray(PartBowlVAO[0]);
	glDrawElements(GL_TRIANGLE_STRIP, indexPartBowl[0], GL_UNSIGNED_INT, 0);

	/*
	glBindVertexArray(PartBowlVAO[1]);
	glDrawElements(GL_TRIANGLE_STRIP, indexPartBowl[1], GL_UNSIGNED_INT, 0);
	
	glBindVertexArray(PartBowlVAO[2]);
	glDrawElements(GL_TRIANGLE_STRIP, indexPartBowl[2], GL_UNSIGNED_INT, 0);
	*/
}


uint BowlVAO = 0;
uint BowlVBO;
uint BowlEBO;
uint indexBowl;
void renderBowl()
{
	if (BowlVAO == 0)
	{
		glGenVertexArrays(1, &BowlVAO);

		glGenBuffers(1, &BowlVBO);
		glGenBuffers(1, &BowlEBO);

		auto inner_radius = 0.3f;
		auto radius = 0.5f;
		auto hole_radius = 0.1f;
		auto a = 0.5f;
		auto b = 0.5f;
		auto c = 0.7f;

		Bowl bowl(inner_radius, radius, a, b, c);

		std::vector<float> data;
		std::vector<uint> idxs;

		//bowl.generate_mesh(40.f, data, idxs);
		//bowl.generate_mesh_hole(40.f, hole_radius, data, idxs);
		//bowl.generate_mesh_uv_hole(40.f, hole_radius, data, idxs);
		//bowl.generate_mesh_uv(40.f, data, idxs);
		bowl.generate_mesh_uv_hole_part(3.14159265359f, 40.f, hole_radius, data, idxs);
		//bowl.generate_mesh_uv_part(3.14159265359f, 40.f, data, idxs);
		//bowl.generate_mesh_hole_part(3.14159265359f, 40.f, hole_radius, data, idxs);

		indexBowl = idxs.size();

		glBindVertexArray(BowlVAO);
		glBindBuffer(GL_ARRAY_BUFFER, BowlVBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BowlEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBowl * sizeof(uint), &idxs[0], GL_STATIC_DRAW);
		constexpr float stride = (3 + 2) * sizeof(float);
		//constexpr float stride = 3 * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	}

	glBindVertexArray(BowlVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexBowl, GL_UNSIGNED_INT, 0);

}


uint DiskVAO = 0;
uint DiskVBO;
uint DiskEBO;
uint indexDisk;
void renderDisk()
{
	if (DiskVAO == 0)
	{
		glGenVertexArrays(1, &DiskVAO);

		glGenBuffers(1, &DiskVBO);
		glGenBuffers(1, &DiskEBO);

		auto radius = 0.7f;

		Disk disk(radius);

		std::vector<float> data;
		std::vector<uint> idxs;

		disk.generate_mesh_uv(60.f, data, idxs);
	
		indexDisk = idxs.size();

		glBindVertexArray(DiskVAO);
		glBindBuffer(GL_ARRAY_BUFFER, DiskVBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, DiskEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDisk * sizeof(uint), &idxs[0], GL_STATIC_DRAW);
		constexpr float stride = (3 + 2) * sizeof(float);
		//constexpr float stride = 3 * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	}

	glBindVertexArray(DiskVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexDisk, GL_UNSIGNED_INT, 0);

}

uint EllParVAO = 0;
uint EllParVBO;
uint EllParEBO;
uint indexEllPar;
void renderEllipticParaboloid()
{
	if (EllParVAO == 0)
	{
		glGenVertexArrays(1, &EllParVAO);

		glGenBuffers(1, &EllParVBO);
		glGenBuffers(1, &EllParEBO);

		auto radius = 0.7f;
		auto a = 0.5f;
		auto b = 0.5f;
		auto c = 0.7f;

		// Convert to rectangular coordinates

		std::vector<float> data;
		std::vector<uint> idxs;

		EllipticParaboloid ep(radius, a, b, c);

		ep.generate_mesh_uv_hole(60.f, 0.4f, data, idxs);

		indexEllPar = idxs.size();

		glBindVertexArray(EllParVAO);
		glBindBuffer(GL_ARRAY_BUFFER, EllParVBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EllParEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexEllPar * sizeof(uint), &idxs[0], GL_STATIC_DRAW);
		constexpr float stride = (3 + 2) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	}

	glBindVertexArray(EllParVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexEllPar, GL_UNSIGNED_INT, 0);
}


void saveScreenshot(const std::string& fname) {
	uint size = SRC_WIDTH * SRC_HEIGHT;
	byte* pixels = new byte[num_ch * size];
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, SRC_WIDTH, SRC_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	stbi_write_png(fname.c_str(), SRC_WIDTH, SRC_HEIGHT, num_ch, pixels, SRC_WIDTH * num_ch);

	std::cout << "Saving screenshot: " << fname << "\n";

	delete[] pixels;
}




void destroyShapes()
{
	glDeleteVertexArrays(1, &EllParVAO);
	glDeleteBuffers(1, &EllParVBO);
	glDeleteBuffers(1, &EllParEBO);

	glDeleteVertexArrays(1, &DiskVAO);
	glDeleteBuffers(1, &DiskVBO);
	glDeleteBuffers(1, &DiskEBO);

	glDeleteVertexArrays(1, &BowlVAO);
	glDeleteBuffers(1, &BowlVBO);
	glDeleteBuffers(1, &BowlEBO);


	glDeleteVertexArrays(4, &PartBowlVAO[0]);
	glDeleteBuffers(4, &PartBowlVBO[0]);
	glDeleteBuffers(4, &PartBowlEBO[0]);

}



// utility function for loading a 2D texture from file
// ---------------------------------------------------
GLuint loadTexture(const char* path, const bool gammaCorrection, const bool isHdr)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	// stbi_set_flip_vertically_on_load(true);
	GLint texwidth = 0; GLint texheight = 0; GLint nrComponents = 0;
	uchar* data = nullptr;
	float* fdata = nullptr;

	if (isHdr) // pbr: load the HDR environment map
		fdata = stbi_loadf(path, &texwidth, &texheight, &nrComponents, 0);
	else
		data = stbi_load(path, &texwidth, &texheight, &nrComponents, 0);

	if (data || fdata)
	{
		GLenum internalformat = GL_RGB;
		GLenum dataformat = GL_RGB;
		if (nrComponents == 1)
			internalformat = dataformat = GL_RED;
		else if (nrComponents == 3) {
			internalformat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataformat = GL_RGB;
		}
		else if (nrComponents == 4) {
			internalformat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataformat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		if (isHdr) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, texwidth, texheight, 0, GL_RGB, GL_FLOAT, fdata);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			stbi_image_free(fdata);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, internalformat, texwidth, texheight, 0, dataformat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, dataformat == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, dataformat == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			stbi_image_free(data);
		}

	}
	else
	{
		std::cerr << "Texture failed to load at path: " << path << "\n";
		stbi_image_free(data);
		return 0;
	}

	return textureID;
}



uint quadVAO = 0;
uint quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
