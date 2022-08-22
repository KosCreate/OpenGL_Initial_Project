#include "SkyboxClass.h"
Skybox::Skybox() {
	vertices = new float[24] 
	{
		-1.0f, -1.0f, 1.0f,//        7--------6
		1.0f, -1.0f, 1.0f,//       /|       /|
		1.0f, -1.0f, -1.0f,//      4--------5 |
		-1.0f, -1.0f, -1.0f,//      | |      | |
		-1.0f, 1.0f, 1.0f,//      | 3------|-2
		1.0f, 1.0f, 1.0f,//      |/       |/
		1.0f, 1.0f, -1.0f,//      0--------1
		-1.0f, 1.0f, -1.0f
	};
	indices = new unsigned int[36] {
		// Right
		1, 2, 6,
		6, 5, 1,
		// Left
		0, 4, 7,
		7, 3, 0,
		// Top
		4, 5, 6,
		6, 7, 4,
		// Bottom
		0, 3, 2,
		2, 1, 0,
		// Back
		0, 1, 5,
		5, 4, 0,
		// Front
		3, 7, 6,
		6, 2, 3
	};

	  // load textures
	 // -------------
	std::vector<std::string> faces {
		std::experimental::filesystem::path("resources/textures/skybox/right.jpg").string(),
		std::experimental::filesystem::path("resources/textures/skybox/left.jpg").string(),
		std::experimental::filesystem::path("resources/textures/skybox/top.jpg").string(),
		std::experimental::filesystem::path("resources/textures/skybox/bottom.jpg").string(),
		std::experimental::filesystem::path("resources/textures/skybox/front.jpg").string(),
		std::experimental::filesystem::path("resources/textures/skybox/back.jpg").string()
	};
}

void Skybox::CreateSkybox() {
	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Skybox::Activate() {

}