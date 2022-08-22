#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <irrKlang/irrKlang.h>
#include <iostream>
#include "shaderClass.h"
#include "VertexArrayClass.h"
#include "VertexBufferClass.h"
#include "CameraClass.h"
#include "Model.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <assimp/ai_assert.h>
#include <vector>
#include <filesystem>

unsigned int numberOfPointLights = 10;
unsigned int numberOfRefractionPointLights = 4;

struct PointLightSettings
{
public:
	float ambient[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
	float specular[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
	float diffuse[4] = { 0.05f, 0.05f, 0.05f, 1.0f };
	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;
};

bool canDrawComic = true;
bool comicEffect;

unsigned int inputHolder;
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 1024;

glm::vec3 cameraStartingPosition = glm::vec3(0.0f, 0.0f, -3.0f);
glm::vec3 cameraStartingFront = glm::vec3(0.0f, 0.0f, 1.0f);

Camera camera(cameraStartingPosition, cameraStartingFront);

bool initiated = false;
bool locked = false;

float rateOfInput = 0.25f;
float nextInput = 0.0f;
bool pressed;
//Mouse variables...
//Last positions...
float last_x = 0.0f;
float last_y = 0.0f;

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

GLint total_mem_kb = 0;
GLint cur_avail_mem_kb = 0;

bool inGUI = false;
bool muteAudio = false;
ImGuiIO io;

/// <summary>
/// Process keyboard input
/// </summary>
void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); }
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { camera.ProcessKeyboard(FORWARD, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { camera.ProcessKeyboard(BACKWARD, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { camera.ProcessKeyboard(RIGHT, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { camera.ProcessKeyboard(LEFT, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {	camera.ProcessKeyboard(UP, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { camera.ProcessKeyboard(DOWN, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) { inputHolder = 0; }
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) { inputHolder = 1; }
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) { inputHolder = 2; }
	if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		if (inputHolder != 0) {
			nextInput = 0.0f;
			inputHolder = 0;
		}
		if (glfwGetTime() > nextInput) {
			nextInput = glfwGetTime() * 10.0f;	
			pressed = true;
			//Grab / Lock Cursor...
			locked = locked ? false : true;
			glfwSetInputMode(window, GLFW_CURSOR, locked ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_DISABLED);
		}
		
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && pressed) {
		nextInput = glfwGetTime() + rateOfInput;
		pressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		if (inputHolder != 1) {
			nextInput = 0.0f;
			inputHolder = 1;
		}
		if (glfwGetTime() > nextInput) {
			pressed = true;
			nextInput = glfwGetTime() * 10.0f;
			//Grab / Lock Cursor...
			inGUI = inGUI ? false : true;
		}
		
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE && pressed) {
		nextInput = glfwGetTime() + rateOfInput;
		pressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		if (inputHolder != 2) {
			nextInput = 0.0f;
			inputHolder = 2;
		}
		if (glfwGetTime() > nextInput) {
			pressed = true;
			nextInput = glfwGetTime() * 10.0f;
			muteAudio = muteAudio ? false : true;
		}
		
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE && pressed) {
		nextInput = glfwGetTime() + rateOfInput;
		pressed = false;
	}
}
/// <summary>
/// Handle & set window frame size 
/// </summary>
void set_frame_buffer_size(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	//Update screen width & height to update the aspect ratio in the projection matrix...
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}
/// <summary>
/// Process mouse input
/// </summary>
void mouse_input_callback(GLFWwindow* window, double xPos, double yPos) {
	if (!inGUI) {
		if (!initiated) {
			last_x = xPos;
			last_y = yPos;
			initiated = true;
		}
		float diff_x = xPos - last_x;
		float diff_y = yPos - last_y;

		last_x = xPos;
		last_y = yPos;
		camera.cameraParameters.yaw += diff_x * camera.cameraParameters.movementSpeed * deltaTime;
		camera.cameraParameters.pitch -= diff_y * camera.cameraParameters.movementSpeed * deltaTime;
		camera.ProcessMouseMovement(diff_x, diff_y, deltaTime);
	}
}
/// <summary>
/// Processing mouse scroll / "Zooming" in & out (increasing or decreasing FOV)
/// </summary>
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) { 
	if (!locked) {
		camera.ProcessMouseScroll(yOffset);
	}
	else {
		return;
	}
}
/// <summary>
/// GLFW window Creation
/// </summary>
void window_Creation(GLFWwindow** window) {
	//Initialize GLFW library...
	glfwInit();
	//Tell GLFW what openGL version are using
	//in this case we are using 3.3 ...
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//Content API Minor version...
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Initialize GLFW we are using the core profile...
	//So that means we are only gonna be using the modern functions...
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Creating the window with the specified width & height, naming it -> First Window Test & telling it that we don't...
	//want to be fullscreen...
	*window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LIGHTMAP_TESTING", NULL, NULL);
}
/// <summary>
/// GLFW window initialization
/// </summary>
int handle_WindowInitialization(GLFWwindow* window) {
	//Make the current window current the main thread context...
	glfwMakeContextCurrent(window);
	//Handle window resizing...
	glfwSetFramebufferSizeCallback(window, set_frame_buffer_size);
	glfwSetCursorPosCallback(window, mouse_input_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//if the window has not been created terminate glfw...
	if (window == NULL) {
		std::cout << "Window is null << TERMINATING WINDOW >>" << std::endl;
		glfwTerminate();
		return -1;
	}

}
/// <summary>
/// Initialize GLAD Library/attributes
/// </summary>
int initialize_GLAD()
{
	//Initialize GLAD function pointers...
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
}
/// <summary>
/// ImGui initializer...
/// </summary>
void GUIInitializer(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
}
/// <summary>
///Helper point light value assigner method...
/// </summary>
void SetPointLightConfigs(int pointLightIndex, Shader shader, glm::vec3 pointAmbient, glm::vec3 pointDiffuse, glm::vec3 pointSpecular,
	float constant, float linear, float quadratic) {
	std::string pointLightString = "pointLights[]";
	std::string pointLightIndexString = pointLightString.insert(12, std::to_string(pointLightIndex).c_str());
	shader.setVec3(pointLightIndexString + ".ambient", pointAmbient);
	shader.setVec3(pointLightIndexString + ".diffuse", pointDiffuse);
	shader.setVec3(pointLightIndexString + ".specular", pointSpecular);
	shader.setFloat(pointLightIndexString + ".constant", constant);
	shader.setFloat(pointLightIndexString + ".linear", linear);
	shader.setFloat(pointLightIndexString + ".quadratic", quadratic);
}

// loads a cubemap texture from 6 individual texture faces...
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

int main() {
	GLFWwindow* window{};

	window_Creation(&window);

	handle_WindowInitialization(window);

	GUIInitializer(window);

	initialize_GLAD();

	//Main audio initialization ==========================
	irrklang::ISoundEngine* MainThemeSoundEngine = irrklang::createIrrKlangDevice();
	irrklang::ISoundEngine* AmbienceSoundEngine = irrklang::createIrrKlangDevice();
	std::string pianoMainThemePath = "Audio_Clips/PIANO_THEME.mp3";
	std::string ambiencePath = "Audio_Clips/Ambience.mp3";
	MainThemeSoundEngine->play2D(pianoMainThemePath.c_str(), true);
	AmbienceSoundEngine->play2D(ambiencePath.c_str(), true);
	//=====================================================

	glEnable(GL_DEPTH_TEST);

	glm::vec3 cubePositions[] =
	{
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(3.0f, 0.0f, 0.0f),
		glm::vec3(6.0f, 0.0f, 0.0f),
		glm::vec3(12.0f, 0.0f, 0.0f),
		glm::vec3(18.0f, 0.0f, 0.0f),
		glm::vec3(24.0f, 0.0f, 0.0f),
		glm::vec3(30.0f, 0.0f, 0.0f),
		glm::vec3(36.0f, 0.0f, 0.0f),
		glm::vec3(42.0f, 0.0f, 0.0f),
		glm::vec3(48.0f, 0.0f, 0.0f)
	};

	std::string modelPaths[]
	{
		"Models/backpack/backpack.obj",
		"Models/plant/indoor plant_02.obj",
		"Models/nanosuit/nanosuit.obj",
		"Models/tower/Medieval_tower_High.obj",
		"Models/buildings/Residential Buildings 001.obj",
		"Models/buildings/Residential Buildings 002.obj",
		"Models/buildings/Residential Buildings 003.obj",
		"Models/buildings/Residential Buildings 004.obj",
		"Models/buildings/Residential Buildings 005.obj",
		"Models/buildings/Residential Buildings 006.obj",
		"Models/buildings/Residential Buildings 007.obj",
		"Models/buildings/Residential Buildings 008.obj",
		"Models/buildings/Residential Buildings 009.obj",
		"Models/buildings/Residential Buildings 010.obj",
	};

	Shader skyboxShader("skybox.vert", "skybox.frag");
	Shader shaderProgram("MultipleLights.vert", "MultipleLights.frag");
	Shader lampShader("lampShader.vert", "lampShader.frag");
	Shader reflectiveShader("reflectiveShader.vert", "reflectiveShader.frag");
	Shader refractionShader("semi-transparent.vert", "semi-transparent.frag");

	Model models[]
	{
		Model(modelPaths[0]),
		Model(modelPaths[1]),
		Model(modelPaths[2]),
		Model(modelPaths[3]),
		Model(modelPaths[4]),
		Model(modelPaths[5]),
		Model(modelPaths[6]),
		Model(modelPaths[7]),
		Model(modelPaths[8]),
		Model(modelPaths[9]),
		Model(modelPaths[10]),
		Model(modelPaths[11]),
	};

	Model lapmModel("Models/minecraft_lamp/source/Redstone-lamp.obj");
	
	#pragma region Skybox Initialization
	float cubeVertices[] = {
		// positions          // normals
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// load textures
	// -------------
	vector<std::string> faces
	{
		"Skybox_Textures/skybox/right.jpg",
		"Skybox_Textures/skybox/left.jpg",
		"Skybox_Textures/skybox/top.jpg",
		"Skybox_Textures/skybox/bottom.jpg",
		"Skybox_Textures/skybox/front.jpg",
		"Skybox_Textures/skybox/back.jpg",
	};

	unsigned int cubemapTexture = loadCubemap(faces);

	skyboxShader.Activate();
	skyboxShader.setInt("skybox", 0);
	reflectiveShader.setInt("skybox", 0);
	#pragma endregion

	Model reflectiveModel("Models/minecraft_lamp/source/Redstone-lamp.obj");
	glm::vec3 reflectiveModelPos = glm::vec3(15.0f, 0.0f, 20.0f);

	Model semiReflectiveModel("Models/minecraft_lamp/source/Redstone-lamp.obj");
	glm::vec3 semiReflectiveModelPos = glm::vec3(25.0f, 0.0f, 20.0f);

 	 //------------------------//
	 //Main render loop...
    //------------------------//
   //While the window should not be closed...

	//Blinn Phong Shader Properties...
	std::vector<PointLightSettings> pointLightSettings(numberOfPointLights);
	PointLightSettings previousLightSettings;
	std::vector<glm::vec3> pAmbientValues(numberOfPointLights);
	std::vector<glm::vec3> pDiffuseValues(numberOfPointLights);
	std::vector<glm::vec3> pSpecularValues(numberOfPointLights);

	#pragma region Main Rendering Loop

	while (!glfwWindowShouldClose(window)) {
		//Poll keyboard events to make the window interactable...
		glfwPollEvents();
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrameTime;
		lastFrameTime = currentFrame;
		//Processing the user input...
		ProcessInput(window);

		//Decide to pause audio...
		MainThemeSoundEngine->setAllSoundsPaused(muteAudio);
		AmbienceSoundEngine->setAllSoundsPaused(muteAudio);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//Begin UI Draw & Callbacks...
		ImGui::Begin("SCENE INSPECTOR");

		#pragma region Processing Data Info
		if (ImGui::TreeNode("Processing Data")) {
			std::string fpsMsg = "MS/FPS : " + std::to_string(1000.0 / double(deltaTime));
			glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb);
			glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
				&cur_avail_mem_kb);
			std::string totalMemoryInfo = "Total Memory : " + std::to_string(total_mem_kb) + "MB";
			std::string currentMemoryInfo = "Current Available Memory : " + std::to_string(cur_avail_mem_kb) + "MB";
			float diff = (total_mem_kb - cur_avail_mem_kb) / 1000;
			std::string memoryUsage = "Memory Usage : " + std::to_string(diff) + "MB";
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 160, 155, 255));
			ImGui::Text(fpsMsg.c_str());
			ImGui::Text(totalMemoryInfo.c_str());
			ImGui::SameLine(0, 18.0f);
			ImGui::Text(currentMemoryInfo.c_str());
			ImGui::Text(memoryUsage.c_str());
			ImGui::PopStyleColor();
		}
		#pragma endregion
		ImGui::Spacing();
		//Initial Instructions...
		#pragma region Initial Instructions & Input Visualization
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		ImGui::Text("INPUT INSTRUCTIONS :");
		ImGui::Spacing();
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(100, 255, 100, 255));
		ImGui::Text("- Press C to hide the mouse cursor");
		ImGui::Text("- Press L to lock camera input");
		ImGui::Text("- Press M or click the 'Mute Audio' checkbox to mute the Audio");
		std::string lockstate = inGUI ? "true" : "false";
		std::string hidden = locked ? "true" : "false";
		std::string lockStateString = "Lock State : " + lockstate;
		std::string hiddenStateString = "Cursor Hidden : " + hidden;
		ImGui::Text(lockStateString.c_str());
		ImGui::Text(hiddenStateString.c_str());
		ImGui::Spacing();
		ImGui::PopStyleColor();
		#pragma endregion

		//Audio Configurations & ImGui Input Handling...
		#pragma region Audio Configs / ImGui Settings
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 100, 255, 255));
		ImGui::Text("AUDIO CONFIGS");
		static float musicVolume = 0.5f;
		ImGui::SliderFloat("Main Audio Volume", &musicVolume, 0.0f, 1.0f);
		static float ambienceVolume = 0.5f;
		ImGui::SliderFloat("Ambience Volume", &ambienceVolume, 0.0f, 1.0f);
		ImGui::Checkbox("Mute Audio", &muteAudio);
		MainThemeSoundEngine->setSoundVolume(musicVolume);
		AmbienceSoundEngine->setSoundVolume(ambienceVolume);
		ImGui::Spacing();
		ImGui::PopStyleColor();

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		#pragma endregion

		//Creating the point light position in the main rendering loop to be able to move in "update"...
		glm::vec3 pointLightPositions[] = {
			glm::vec3(0.0f + glm::cos(glfwGetTime()), glm::sin(glfwGetTime()), 2.0f),
			glm::vec3(3.0f + glm::cos(glfwGetTime()), glm::sin(glfwGetTime()), 2.0f),
			glm::vec3(6.0f + glm::cos(glfwGetTime()), glm::sin(glfwGetTime()), 2.0f),
			glm::vec3(12.0f + glm::cos(glfwGetTime()), glm::sin(glfwGetTime()), 2.0f),
			glm::vec3(18.0f + glm::cos(glfwGetTime()), glm::sin(glfwGetTime()), 3.0f),
			glm::vec3(24.0f + glm::cos(glfwGetTime()), glm::sin(glfwGetTime()), 3.0f),
			glm::vec3(30.0f + glm::cos(glfwGetTime()), glm::sin(glfwGetTime()), 3.0f),
			glm::vec3(36.0f + glm::cos(glfwGetTime()), glm::sin(glfwGetTime()), 3.5f),
			glm::vec3(42.0f + glm::cos(glfwGetTime()), glm::sin(glfwGetTime()), 3.5f),
			glm::vec3(48.0f + glm::cos(glfwGetTime()), glm::sin(glfwGetTime()), 3.5f)
		};
		
		shaderProgram.Activate();
		shaderProgram.setVec3("viewPos", camera.cameraTransform.position);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		//Position, direction (z), camera up (y)
		view = camera.GetViewMatrix();
		//FOV, ASPECT RATION, NEAR CLIPPING PLANE, FAR CLIPPING PLANE...
		projection = glm::perspective(glm::radians(camera.cameraParameters.fieldOfView), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
		shaderProgram.setMat4("view", view);
		shaderProgram.setMat4("projection", projection);
		shaderProgram.setVec3("spotLight.position", camera.cameraTransform.position);
		shaderProgram.setVec3("spotLight.direction", camera.cameraTransform.cameraFront);

		#pragma region Comic Effect
		if (ImGui::Checkbox("Comic Effect", &comicEffect)) {
			if (comicEffect) {
				if (canDrawComic) {
					pointLightSettings[0].constant = 0;
					pointLightSettings[0].linear = 0;
					pointLightSettings[0].quadratic = 0;
					canDrawComic = false;
				}
			}
			else {
				if (!canDrawComic) {
					pointLightSettings[0] = previousLightSettings;
					canDrawComic = true;
				}
			}
		}
		#pragma endregion

		#pragma region Model Material uniform & GUI values handling
		//Material Configs...
		static float shininess = 10.0f;
		static int matDiffuse = 0;
		static int matSpecular = 1;
		if (ImGui::TreeNode("Model Material Properties"))
		{
			//Material Shininess...
			ImGui::SliderFloat("Shininess", &shininess, 0.01f, 100.0f);
			ImGui::SliderInt("Diffuse", &matDiffuse, 0, 1);
			ImGui::SliderInt("Specular", &matSpecular, 0, 1);
		}
		shaderProgram.setFloat("material.shininess", shininess);
		shaderProgram.setInt("material.diffuse", matDiffuse);
		shaderProgram.setInt("material.specular", matSpecular);
		#pragma endregion

		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 100, 255, 255));
		ImGui::Text("LIGHT CONFIGS");
		ImGui::PopStyleColor();

		#pragma region Directional Light uniform & GUI values handling
	
		//Directional Light Configs...
		static float dirDirection[3] = { -0.2f, -1.0f, -0.3f };
		glm::vec3 dirLightDirection = glm::vec3(dirDirection[0], dirDirection[1], dirDirection[2]);
		static float ambientValues[3] = { 0.05f, 0.05f, 0.05f };
		glm::vec3 ambient = glm::vec3(ambientValues[0], ambientValues[1], ambientValues[2]);
		static float diffuseValues[3] = { 0.4f, 0.4f, 0.4f };
		glm::vec3 diffuse = glm::vec3(diffuseValues[0], diffuseValues[1], diffuseValues[2]);
		static float specularValues[3] = { 1.0f, 1.0f, 1.0f };
		glm::vec3 specular = glm::vec3(specularValues[0], specularValues[1], specularValues[2]);

		if (ImGui::TreeNode("Directional Light")) {

			//Direction
			ImGui::SliderFloat3("Dir Light Direction", dirDirection, -2.0f, 2.0f);
			//Ambient :
			ImGui::SliderFloat3("Dir Light Ambient", ambientValues, 0.0f, 1.0f);
			//Diffuse :
			ImGui::SliderFloat3("Dir Light Diffuse", diffuseValues, 0.0f, 1.0f);
			//Specular :
			ImGui::SliderFloat3("Dir Light Specular", specularValues, 0.0f, 2.0f);
		}
		shaderProgram.setVec3("dirLight.direction", dirLightDirection);
		shaderProgram.setVec3("dirLight.ambient", ambient);
		shaderProgram.setVec3("dirLight.diffuse", diffuse);
		shaderProgram.setVec3("dirLight.specular", specular);
		#pragma endregion

		#pragma region Spotlight uniform & GUI values handling
		//Spotlight Configs....
		static float spotAmbientValues[3] = { 1.0f, 1.0f, 1.0f };
		glm::vec3 spotAmbient = glm::vec3(spotAmbientValues[0], spotAmbientValues[1], spotAmbientValues[2]);
		static float spotDiffuseValues[3] = { 1.0f, 1.0f, 1.0f };
		glm::vec3 spotDiffuse = glm::vec3(spotDiffuseValues[0], spotDiffuseValues[1], spotDiffuseValues[2]);
		static float spotSpecularValues[3] = { 1.0f, 1.0f, 1.0f };
		glm::vec3 spotSpecular = glm::vec3(spotSpecularValues[0], spotSpecularValues[1], spotSpecularValues[2]);
		static float spotConstValue = 0.5f;
		static float spotLinearValue = 0.5f;
		static float spotQuadraticValue = 0.039f;
		static float radiansInner = 12.5f;
		static float radiansOuter = 15.0f;

		if (ImGui::TreeNode("Spotlight Configs"))
		{
			//Ambient...
			ImGui::SliderFloat3("Spotlight Ambient", spotAmbientValues, 0.0f, 2.0f);
			//Specular...
			ImGui::SliderFloat3("Spotlight Specular", spotSpecularValues, 0.0f, 2.0f);
			//Diffuse...
			ImGui::SliderFloat3("Spotlight Diffuse", spotDiffuseValues, 0.0f, 2.0f);
			//Const...
			ImGui::SliderFloat("Spotlight Constant", &spotConstValue, 0.0f, 2.0f);
			//Linear...
			ImGui::SliderFloat("Spotlight Linear", &spotLinearValue, 0.0f, 1.0f);
			//Quadratic 
			ImGui::SliderFloat("Spotlight Quadratic", &spotQuadraticValue, 0.0f, 0.5f);
			//Cut off (Inner)...
			ImGui::SliderAngle("Spotlight Inner Cutoff", &radiansInner);
			//Cut off (outer)....
			ImGui::SliderAngle("Spotlight Outer Cutoff", &radiansOuter);
		}

		shaderProgram.setVec3("spotLight.ambient", spotAmbient);
		shaderProgram.setVec3("spotLight.diffuse", spotDiffuse);
		shaderProgram.setVec3("spotLight.specular", spotSpecular);
		shaderProgram.setFloat("spotLight.constant", spotConstValue);
		shaderProgram.setFloat("spotLight.linear", spotLinearValue);
		shaderProgram.setFloat("spotLight.quadratic", spotQuadraticValue);
		shaderProgram.setFloat("spotLight.cutOff", glm::cos(glm::radians(radiansInner)));
		shaderProgram.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(radiansOuter)));
		#pragma endregion

		#pragma region Point Light uniform values & GUI values handling
		//Point light position assignment...
		for (int i = 0; i < numberOfPointLights; i++) {
			std::string pointLightString = "pointLights[]";
			std::string pointLightIndexString = pointLightString.insert(12, std::to_string(i).c_str());
			shaderProgram.setVec3(pointLightIndexString + ".position", pointLightPositions[i]);
		}

		for (int i = 0; i < pAmbientValues.size(); i++) {
			pAmbientValues[i] = glm::vec3(pointLightSettings[i].ambient[0], pointLightSettings[i].ambient[1], pointLightSettings[i].ambient[2]);
			pDiffuseValues[i] = glm::vec3(pointLightSettings[i].diffuse[0], pointLightSettings[i].diffuse[1], pointLightSettings[i].diffuse[2]);
			pSpecularValues[i] = glm::vec3(pointLightSettings[i].specular[0], pointLightSettings[i].specular[1], pointLightSettings[i].specular[2]);
			std::string title = "Point Light : " + std::to_string(i + 1) + " Configs";
			if (ImGui::TreeNode(title.c_str())) {
				if (pointLightSettings[0].linear != 0) {
					previousLightSettings = pointLightSettings[0];
				}
				//Ambient...
				ImGui::ColorEdit4("Ambient", pointLightSettings[i].ambient);
				//Specular...
				ImGui::ColorEdit4("Specular", pointLightSettings[i].specular);
				//Diffuse...
				ImGui::ColorEdit4("Diffuse", pointLightSettings[i].diffuse);
				//Const...
				ImGui::SliderFloat("Constant", &pointLightSettings[i].constant, 0.0f, 2.0f);
				//Linear...
				ImGui::SliderFloat("Linear", &pointLightSettings[i].linear, 0.0f, 1.0f);
				//Quadratic 
				ImGui::SliderFloat("Quadratic", &pointLightSettings[i].quadratic, 0.0f, 0.5f);
			}
			SetPointLightConfigs(i, shaderProgram, pAmbientValues[i], pSpecularValues[i], pDiffuseValues[i], pointLightSettings[i].constant, pointLightSettings[i].linear, pointLightSettings[i].quadratic);
		}

		#pragma endregion

		#pragma region Draw Main Scene Objects/Models

		glm::mat4 model;

		static int index = 10;
		if (ImGui::TreeNode("Model Configs")) {
			ImGui::SliderInt("Number Of Models", &index, 0.0f, std::size(cubePositions));
		}

		shaderProgram.Activate();
		for (int i = 0; i < index; i++) {
			shaderProgram.Activate();
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			shaderProgram.setMat4("model", model);
			// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
			stbi_set_flip_vertically_on_load(true);
			models[i].Draw(shaderProgram);
		}
		#pragma endregion

		#pragma region Draw "Lamp" Objects
		lampShader.Activate();
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);
		//Draw light emitter cube...
		glm::vec3 lampSize = glm::vec3(0.015f);
		for (unsigned int i = 0; i < numberOfPointLights; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, lampSize);
			lampShader.setMat4("model", model);
			lapmModel.Draw(lampShader);
		}
		#pragma endregion

		#pragma region Draw Skybox

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.Activate();
		//view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", glm::mat3(view));
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default
		#pragma endregion

		reflectiveShader.Activate();
		reflectiveShader.setMat4("view", view);
		reflectiveShader.setMat4("projection", projection);

		#pragma region Draw Reflective Model/Object
		reflectiveShader.Activate();
		model = glm::mat4(1.0f);
		model = glm::translate(model, reflectiveModelPos);
		model = glm::scale(model, glm::vec3(0.2f));
		reflectiveShader.setMat4("model", model);
		reflectiveShader.setVec3("cameraPos", camera.cameraTransform.position);
		reflectiveModel.Draw(reflectiveShader);
		#pragma endregion

		refractionShader.Activate();
		refractionShader.setMat4("view", view);
		refractionShader.setMat4("projection", projection);
		#pragma region SemiReflective - Refracting Model/Object

		static float reflectionRatio = 1.00f;
		static float refractionRatio = 1.52f;
		if (ImGui::TreeNode("Refraction Model Settings")) {
			ImGui::BulletText("Air : 1.00");
			ImGui::BulletText("Water : 1.33");
			ImGui::BulletText("Ice : 1.309");
			ImGui::BulletText("Glass : 1.52");
			ImGui::BulletText("Diamond : 2.42");
			ImGui::InputFloat("Reflective Object Ration", &reflectionRatio);
			ImGui::Spacing();
			ImGui::InputFloat("Refraction Object Ration", &refractionRatio);
		}
		refractionShader.setFloat("fromObject", reflectionRatio);
		refractionShader.setFloat("toObject", refractionRatio);

		refractionShader.Activate();
		model = glm::mat4(1.0f);
		model = glm::translate(model, semiReflectiveModelPos);
		model = glm::scale(model, glm::vec3(0.2f));
		refractionShader.setMat4("model", model);
		refractionShader.setVec3("cameraPos", camera.cameraTransform.position);
		semiReflectiveModel.Draw(refractionShader);
		#pragma endregion

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//Swap back buffer to front buffer to actually render the shit we want to see...
		glfwSwapBuffers(window);
	}
	#pragma endregion
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	shaderProgram.Delete();
	lampShader.Delete();
	skyboxShader.Delete();
	reflectiveShader.Delete();
	refractionShader.Delete();
	glfwTerminate();
	return 0;
}