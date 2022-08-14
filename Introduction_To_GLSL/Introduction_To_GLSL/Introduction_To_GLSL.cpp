#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <irrKlang/irrKlang.h>
#include <iostream>
#include "shaderClass.h"
#include "VertexBufferClass.h"
#include "VertexArrayClass.h"
#include "ElementBufferObject.h"
#include "TextureClass.h"
#include "CameraClass.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

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
	//Toggle for cursor lock mode...
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

void GUIInitializer(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
}

void SetPointLightConfigs(int pointLightIndex, Shader shader, glm::vec3 pointAmbient, glm::vec3 pointDiffuse, glm::vec3 pointSpecular,
	float constant, float linear, float quadratic) {
	std::string pointLightString = "pointLights[]";
	std::string pointLightIndexString = pointLightString.insert(12, std::to_string(pointLightIndex).c_str());
	if (GLFW_KEY_SPACE == GLFW_PRESS)
	{
		std::cout << "INITIALING : " << " " << pointLightIndexString << std::endl;
	}
	shader.setVec3(pointLightIndexString + ".ambient", pointAmbient);
	shader.setVec3(pointLightIndexString + ".diffuse", pointDiffuse);
	shader.setVec3(pointLightIndexString + ".specular", pointSpecular);
	shader.setFloat(pointLightIndexString + ".constant", constant);
	shader.setFloat(pointLightIndexString + ".linear", linear);
	shader.setFloat(pointLightIndexString + ".quadratic", quadratic);
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

	float vertices[] = {
		// positions         // normals           // texture
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};

	Shader shaderProgram("MultipleLights.vert", "MultipleLights.frag");
	Shader lampShader("lampShader.vert", "lampShader.frag");
	VBO VBO(vertices, sizeof(vertices));
	VBO.Bind();
	VAO CrateVAO(1, shaderProgram.ID);
	VAO LightVAO(1, lampShader.ID);
	CrateVAO.Bind();
	//Arguments : ( Starting with the VBO to be linked ) & then
	//Arguments : 1st: layout location, 2nd: elements, 3rd: element stride, 4th: starting position
	CrateVAO.LinkVBO(VBO, 0, 3, 8, 0);
	CrateVAO.LinkVBO(VBO, 1, 3, 8, 3);
	CrateVAO.LinkVBO(VBO, 2, 2, 8, 6);

	LightVAO.Bind();
	LightVAO.LinkVBO(VBO, 0, 3, 8, 0);

	Texture diffuseMap("Textures/diffuseMap.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE,false);
	Texture specularMap("Textures/specularMap.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_UNSIGNED_BYTE, false);

	shaderProgram.Activate();
	
 	 //------------------------//
	 //Main render loop...
    //------------------------//
   //While the window should not be closed...


	while (!glfwWindowShouldClose(window)) {
		//Poll keyboard events to make the window interactable...
		glfwPollEvents();
 		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrameTime;
		lastFrameTime = currentFrame;
		//Processing the user input...
		ProcessInput(window);

		MainThemeSoundEngine->setAllSoundsPaused(muteAudio);
		AmbienceSoundEngine->setAllSoundsPaused(muteAudio);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//Begin UI Draw & Callbacks...
		ImGui::Begin("Light/Material Configs");
		
		//Initial Instructions...
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

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f + glm::cos(glfwGetTime()),  0.2f + glm::sin(glfwGetTime()),  2.0f),
		glm::vec3(2.3f + glm::cos(glfwGetTime()), -3.3f + glm::sin(glfwGetTime()), -4.0f),
		glm::vec3(-4.0f + glm::cos(glfwGetTime()),  2.0f + glm::sin(glfwGetTime()), -12.0f),
		glm::vec3(0.0f + glm::cos(glfwGetTime()),  0.0f + glm::sin(glfwGetTime()), -3.0f)
		};

		shaderProgram.Activate();
		shaderProgram.setVec3("pointLights[0].position", pointLightPositions[0]);
		shaderProgram.setVec3("pointLights[1].position", pointLightPositions[1]);
		shaderProgram.setVec3("pointLights[2].position", pointLightPositions[2]);
		shaderProgram.setVec3("pointLights[3].position", pointLightPositions[3]);
		shaderProgram.setVec3("viewPos", camera.cameraTransform.position);

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		//Position, direction (z), camera up (y)
		view = camera.GetViewMatrix();
		//FOV, ASPECT RATION, NEAR CLIPPING PLANE, FAR CLIPPING PLANE...
		projection = glm::perspective(glm::radians(camera.cameraParameters.fieldOfView), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

		shaderProgram.setMat4("view", view);
		shaderProgram.setMat4("projection", projection);

		CrateVAO.Bind();

		shaderProgram.setVec3("spotLight.position", camera.cameraTransform.position);
		shaderProgram.setVec3("spotLight.direction", camera.cameraTransform.cameraFront);

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
		//Point light Configs...
		// point light 1
		static float pointLight1AmbientValues[3] = { 0.05f, 0.05f, 0.05f };
		glm::vec3 light1Ambient = glm::vec3(pointLight1AmbientValues[0], pointLight1AmbientValues[1], pointLight1AmbientValues[2]);
		static float pointLight1DiffuseValues[3] = { 0.05f, 0.05f, 0.05f };
		glm::vec3 light1Diffuse = glm::vec3(pointLight1DiffuseValues[0], pointLight1DiffuseValues[1], pointLight1DiffuseValues[2]);
		static float pointLight1SpecularValues[3] = { 0.05f, 0.05f, 0.05f };
		glm::vec3 light1Specular = glm::vec3(pointLight1SpecularValues[0], pointLight1SpecularValues[1], pointLight1SpecularValues[2]);
		static float light1ConstValue = 1.0f;
		static float light1LinearValue = 0.09f;
		static float light1QuadraticValue = 0.032f;
		if (ImGui::TreeNode("POINT LIGHT 1 Configs :"))
		{
			//Ambient...
			ImGui::SliderFloat3("P_Light 1 Ambient", pointLight1AmbientValues, 0.0f, 2.0f);
			//Specular...
			ImGui::SliderFloat3("P_Light 1 Specular", pointLight1SpecularValues, 0.0f, 2.0f);
			//Diffuse...
			ImGui::SliderFloat3("P_Light 1 Diffuse", pointLight1DiffuseValues, 0.0f, 2.0f);
			//Const...
			ImGui::SliderFloat("P_Light 1 Constant", &light1ConstValue, 0.0f, 2.0f);
			//Linear...
			ImGui::SliderFloat("P_Light 1 Linear", &light1LinearValue, 0.0f, 1.0f);
			//Quadratic 
			ImGui::SliderFloat("P_Light 1 Quadratic", &light1QuadraticValue, 0.0f, 0.5f);
		}
		SetPointLightConfigs(0, shaderProgram, light1Ambient, light1Diffuse, light1Specular, light1ConstValue, light1LinearValue, light1QuadraticValue);

		//point light 2 :
		static float pointLight2AmbientValues[3] = { 0.05f, 0.05f, 0.05f };
		glm::vec3 light2Ambient = glm::vec3(pointLight2AmbientValues[0], pointLight2AmbientValues[1], pointLight2AmbientValues[2]);
		static float pointLight2DiffuseValues[3] = { 0.05f, 0.05f, 0.05f };
		glm::vec3 light2Diffuse = glm::vec3(pointLight2DiffuseValues[0], pointLight2DiffuseValues[1], pointLight2DiffuseValues[2]);
		static float pointLight2SpecularValues[3] = { 0.05f, 0.05f, 0.05f };
		glm::vec3 light2Specular = glm::vec3(pointLight2SpecularValues[0], pointLight2SpecularValues[1], pointLight2SpecularValues[2]);
		static float light2ConstValue = 1.0f;
		static float light2LinearValue = 0.09f;
		static float light2QuadraticValue = 0.032f;
		if (ImGui::TreeNode("POINT LIGHT 2 Configs :"))
		{
			//Ambient...
			ImGui::SliderFloat3("P_Light 2 Ambient", pointLight2AmbientValues, 0.0f, 2.0f);
			//Specular...
			ImGui::SliderFloat3("P_Light 2 Specular", pointLight2SpecularValues, 0.0f, 2.0f);
			//Diffuse...
			ImGui::SliderFloat3("P_Light 2 Diffuse", pointLight2DiffuseValues, 0.0f, 2.0f);
			//Const...
			ImGui::SliderFloat("P_Light 2 Constant", &light2ConstValue, 0.0f, 2.0f);
			//Linear...
			ImGui::SliderFloat("P_Light 2 Linear", &light2LinearValue, 0.0f, 1.0f);
			//Quadratic 
			ImGui::SliderFloat("P_Light 2 Quadratic", &light2QuadraticValue, 0.0f, 0.5f);
		}
		SetPointLightConfigs(1, shaderProgram, light2Ambient, light2Diffuse, light2Specular, light2ConstValue, light2LinearValue, light2QuadraticValue);

		//point light 3 :
		static float pointLight3AmbientValues[3] = { 0.05f, 0.05f, 0.05f };
		glm::vec3 light3Ambient = glm::vec3(pointLight3AmbientValues[0], pointLight3AmbientValues[1], pointLight3AmbientValues[2]);
		static float pointLight3DiffuseValues[3] = { 0.05f, 0.05f, 0.05f };
		glm::vec3 light3Diffuse = glm::vec3(pointLight3DiffuseValues[0], pointLight3DiffuseValues[1], pointLight3DiffuseValues[2]);
		static float pointLight3SpecularValues[3] = { 0.05f, 0.05f, 0.05f };
		glm::vec3 light3Specular = glm::vec3(pointLight3SpecularValues[0], pointLight3SpecularValues[1], pointLight3SpecularValues[2]);
		static float light3ConstValue = 1.0f;
		static float light3LinearValue = 0.09f;
		static float light3QuadraticValue = 0.032f;
		if (ImGui::TreeNode("POINT LIGHT 3 Configs :"))
		{
			//Ambient...
			ImGui::SliderFloat3("P_Light 3 Ambient", pointLight3AmbientValues, 0.0f, 2.0f);
			//Specular...
			ImGui::SliderFloat3("P_Light 3 Specular", pointLight3SpecularValues, 0.0f, 2.0f);
			//Diffuse...
			ImGui::SliderFloat3("P_Light 3 Diffuse", pointLight3DiffuseValues, 0.0f, 2.0f);
			//Const...
			ImGui::SliderFloat("P_Light 3 Constant", &light3ConstValue, 0.0f, 2.0f);
			//Linear...
			ImGui::SliderFloat("P_Light 3 Linear", &light3LinearValue, 0.0f, 1.0f);
			//Quadratic 
			ImGui::SliderFloat("P_Light 3 Quadratic", &light3QuadraticValue, 0.0f, 0.5f);
		}
		SetPointLightConfigs(2, shaderProgram, light3Ambient, light3Diffuse, light3Specular, light3ConstValue, light3LinearValue, light3QuadraticValue);

		//point light 4 : 
		static float pointLight4AmbientValues[3] = { 0.05f, 0.05f, 0.05f };
		glm::vec3 light4Ambient = glm::vec3(pointLight4AmbientValues[0], pointLight4AmbientValues[1], pointLight4AmbientValues[2]);
		static float pointLight4DiffuseValues[3] = { 0.05f, 0.05f, 0.05f };
		glm::vec3 light4Diffuse = glm::vec3(pointLight4DiffuseValues[0], pointLight4DiffuseValues[1], pointLight4DiffuseValues[2]);
		static float pointLight4SpecularValues[3] = { 0.05f, 0.05f, 0.05f };
		glm::vec3 light4Specular = glm::vec3(pointLight4SpecularValues[0], pointLight4SpecularValues[1], pointLight4SpecularValues[2]);
		static float light4ConstValue = 1.0f;
		static float light4LinearValue = 0.09f;
		static float light4QuadraticValue = 0.032f;
		if (ImGui::TreeNode("POINT LIGHT 4 Configs :"))
		{
			//Ambient...
			ImGui::SliderFloat3("P_Light 4 Ambient", pointLight4AmbientValues, 0.0f, 2.0f);
			//Specular...
			ImGui::SliderFloat3("P_Light 4 Specular", pointLight4SpecularValues, 0.0f, 2.0f);
			//Diffuse...
			ImGui::SliderFloat3("P_Light 4 Diffuse", pointLight4DiffuseValues, 0.0f, 2.0f);
			//Const...
			ImGui::SliderFloat("P_Light 4 Constant", &light4ConstValue, 0.0f, 2.0f);
			//Linear...
			ImGui::SliderFloat("P_Light 4 Linear", &light4LinearValue, 0.0f, 1.0f);
			//Quadratic 
			ImGui::SliderFloat("P_Light 4s Quadratic", &light4QuadraticValue, 0.0f, 0.5f);
		}
		SetPointLightConfigs(3, shaderProgram, light4Ambient, light4Diffuse, light4Specular, light4ConstValue, light4LinearValue, light4QuadraticValue);
	#pragma endregion

		glm::mat4 model;

		diffuseMap.Bind(GL_TEXTURE0);
		specularMap.Bind(GL_TEXTURE1);

		static int index = 2;
		if (ImGui::TreeNode("Primitive Model Configs")) {
			ImGui::SliderInt("Number Of Models", &index, 0.0f, std::size(cubePositions));
		}
		for (int i = 0; i < index; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.3f, 1.0f, 0.2f));

			shaderProgram.setMat4("model", model);

			CrateVAO.DrawTriangleArrays(0, 36);
		}

		lampShader.Activate();
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);
		//Draw light emitter cube...
		LightVAO.Bind();

		//Light color configs...
		static float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		if (ImGui::TreeNodeEx("Point Light General Configs")) {
			ImGui::ColorEdit4("Point Light Colors", color);
		}

		lampShader.setVec3("lightColor", color[0], color[1], color[2]);

		for (unsigned int i = 0; i < 4; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			lampShader.setMat4("model", model);
			LightVAO.DrawTriangleArrays(0, 36);
		}

		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//Swap back buffer to front buffer to actually render the shit we want to see...
		glfwSwapBuffers(window);
	}
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	shaderProgram.Delete();
	lampShader.Delete();
	diffuseMap.Delete();
	specularMap.Delete();
	glfwTerminate();
	return 0;
}