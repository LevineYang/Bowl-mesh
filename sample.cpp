#include "include/sample.hpp"
#include "include/EllipticParaboloid.hpp"
#include "include/Disk.hpp"
#include "include/Bowl.hpp"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "include/stb_image_write.h"



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

	renderDisk();
	//renderEllipticParaboloid();
	//renderBowl();
	//renderPartBowl();

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

		auto inner_radius = 0.5f;
		auto radius = 0.7f;
		auto hole_radius = 0.2f;
		auto a = 0.5f;
		auto b = 0.5f;
		auto c = 0.7f;

		PartitionBowl bowl(inner_radius, radius, a, b, c);

		std::vector<std::vector<float>> data;
		std::vector<std::vector<uint>> idxs;

		bowl.generate_mesh(4, 40.f, data, idxs);


		for (auto i = 0; i < idxs.size(); ++i)
			indexPartBowl[i] = idxs.at(i).size();

		constexpr float stride = 3 * sizeof(float);

		glBindVertexArray(PartBowlVAO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, PartBowlVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, data[0].size() * sizeof(float), &data[0][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PartBowlEBO[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexPartBowl[0] * sizeof(uint), &idxs[0][0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

		glBindVertexArray(PartBowlVAO[1]);
		glBindBuffer(GL_ARRAY_BUFFER, PartBowlVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, data[1].size() * sizeof(float), &data[1][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PartBowlEBO[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexPartBowl[1] * sizeof(uint), &idxs[1][0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

		glBindVertexArray(PartBowlVAO[2]);
		glBindBuffer(GL_ARRAY_BUFFER, PartBowlVBO[2]);
		glBufferData(GL_ARRAY_BUFFER, data[2].size() * sizeof(float), &data[2][0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PartBowlEBO[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexPartBowl[2] * sizeof(uint), &idxs[2][0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	}

	glBindVertexArray(PartBowlVAO[0]);
	glDrawElements(GL_TRIANGLE_STRIP, indexPartBowl[0], GL_UNSIGNED_INT, 0);
	glBindVertexArray(PartBowlVAO[1]);
	glDrawElements(GL_TRIANGLE_STRIP, indexPartBowl[1], GL_UNSIGNED_INT, 0);
	glBindVertexArray(PartBowlVAO[2]);
	glDrawElements(GL_TRIANGLE_STRIP, indexPartBowl[1], GL_UNSIGNED_INT, 0);
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

		auto inner_radius = 0.5f;
		auto radius = 0.7f;
		auto hole_radius = 0.2f;
		auto a = 0.5f;
		auto b = 0.5f;
		auto c = 0.7f;

		Bowl bowl(inner_radius, radius, a, b, c);

		std::vector<float> data;
		std::vector<uint> idxs;

		//bowl.generate_mesh(60.f, data, idxs);
		bowl.generate_mesh_hole(60.f, hole_radius, data, idxs);


		indexBowl = idxs.size();

		glBindVertexArray(BowlVAO);
		glBindBuffer(GL_ARRAY_BUFFER, BowlVBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BowlEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBowl * sizeof(uint), &idxs[0], GL_STATIC_DRAW);
		constexpr float stride = 3 * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
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

		disk.generate_mesh(35.f, data, idxs);
		//disk.generate_mesh_hole(100.f, 0.4f, data, idxs);


		indexDisk = idxs.size();

		glBindVertexArray(DiskVAO);
		glBindBuffer(GL_ARRAY_BUFFER, DiskVBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, DiskEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDisk * sizeof(uint), &idxs[0], GL_STATIC_DRAW);
		constexpr float stride = 3 * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
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

		ep.generate_mesh(72.f,  data, idxs);
		//ep.generate_mesh_hole(80.f, 0.4f, data, idxs);

		indexEllPar = idxs.size();

		glBindVertexArray(EllParVAO);
		glBindBuffer(GL_ARRAY_BUFFER, EllParVBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EllParEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexEllPar * sizeof(uint), &idxs[0], GL_STATIC_DRAW);
		constexpr float stride = 3 * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
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

