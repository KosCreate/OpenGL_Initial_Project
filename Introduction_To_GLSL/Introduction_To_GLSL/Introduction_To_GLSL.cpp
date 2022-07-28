#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <irrKlang/irrKlang.h>
#include <stb_image.h>
#include <iostream>
#include "shaderClass.h"
#include "VertexBufferClass.h"
#include "VertexArrayClass.h"
#include "ElementBufferObject.h"
#include "CameraClass.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 1024;

glm::vec3 cameraStartingPosition = glm::vec3(0.0f, 0.0f, -3.0f);
glm::vec3 cameraStartingFront = glm::vec3(0.0f, 0.0f, 1.0f);

Camera camera(cameraStartingPosition, cameraStartingFront);

bool initiated = false;
bool locked = false;
float cursorCounter = 0;
//Mouse variables...
//Last positions...
float last_x = 0.0f;
float last_y = 0.0f;

float deltaTime = 0.0f;
float lastFrameTime = 0.0f; 
unsigned int loadTexture(char const* path, bool flipVertically)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, numOfChannels;

	stbi_set_flip_vertically_on_load(flipVertically);

	unsigned char* data = stbi_load(path, &width, &height, &numOfChannels, 0);

	if (data)
	{
		GLenum format;
		if (numOfChannels == 1)
			format = GL_RED;
		else if (numOfChannels == 3)
			format = GL_RGB;
		else if (numOfChannels == 4)
			format = GL_RGBA;
		else
		{
			std::cout << "FAILED TO LOAD TEXTURE (BAD CHANNELS) AT PATH " << path << std::endl;
			stbi_image_free(data);
			return textureID;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		std::cout << "FAILED TO LOAD TEXTURE AT PATH " << path << std::endl;
	}

	stbi_image_free(data);

	return textureID;
}
void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); }
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { camera.ProcessKeyboard(FORWARD, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { camera.ProcessKeyboard(BACKWARD, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { camera.ProcessKeyboard(RIGHT, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { camera.ProcessKeyboard(LEFT, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {	camera.ProcessKeyboard(UP, deltaTime); }
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { camera.ProcessKeyboard(DOWN, deltaTime); }
	//Toggle for cursor lock mode...
	if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		cursorCounter += deltaTime;
		if (cursorCounter >= 0.1f) {	
			//Grab / Lock Cursor...
			if (locked) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				locked = !locked;
			}
			else {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				locked = !locked;
			}
			cursorCounter = 0;
		}
	}
}
void set_frame_buffer_size(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	//Update screen width & height to update the aspect ratio in the projection matrix...
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}
void mouse_input_callback(GLFWwindow* window, double xPos, double yPos) {
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
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	camera.ProcessMouseScroll(yOffset);
}
int main() {
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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LIGHTMAP_TESTING", NULL, NULL);

	//if the window has not been created terminate glfw...
	if (window == NULL) {
		std::cout << "Window is null << TERMINATING WINDOW >>" << std::endl;
		glfwTerminate();
		return -1;
	}

	//Make the current window current the main thread context...
	glfwMakeContextCurrent(window);
	//Handle window resizing...
	glfwSetFramebufferSizeCallback(window, set_frame_buffer_size);
	glfwSetCursorPosCallback(window, mouse_input_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//Initialize GLAD function pointers...
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

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

	Shader shaderProgram("default.vert", "default.frag");
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

	unsigned int diffuseMap = loadTexture("Textures/diffuseMap.png", false);
	unsigned int specularMap = loadTexture("Textures/specularMap.png", false);

	shaderProgram.Activate();
	shaderProgram.setVec3("cubeMaterial.ambient", glm::vec3(0.5, 0.5, 0.5));
	shaderProgram.setInt("cubeMaterial.diffuse", 0);
	shaderProgram.setInt("cubeMaterial.specular", 1);
	shaderProgram.setFloat("cubeMaterial.shininess", 0.6 * 128);

	//Point Light Usage...
	
	//shaderProgram.SetBool("usePointLight", true);
	//shaderProgram.setFloat("pointLightData.aConst", 0.7);
	//shaderProgram.setFloat("pointLightData.aConst", 0.04);

	//Directional Light Usage...
	
	//shaderProgram.SetBool("useDirectionalLight", true);
	//shaderProgram.setVec3("directionalLightDirection", glm::vec3(1.0f, 1.0f, 0.0f));
	
	//Spot Light  Usage...
	 
	shaderProgram.SetBool("useSpotLight", true);
	shaderProgram.setFloat("spotLightData.outerCone", 0.98);
	shaderProgram.setFloat("spotLightData.innerCone", 1.0f);
	shaderProgram.setVec3("spotLightData.spotLightPosition", camera.cameraTransform.position);
	shaderProgram.setVec3("spotLightData.spotLightDirection", glm::vec3(camera.cameraTransform.cameraFront.x, camera.cameraTransform.cameraFront.y, camera.cameraTransform.cameraFront.z));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

 	 //------------------------//
	 //Main render loop...
    //------------------------//
   //While the window should not be closed...

	irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
	std::string audioClipPath = "Audio_Clips/PIANO_THEME.mp3";
	SoundEngine->play2D(audioClipPath.c_str(), true);

	while (!glfwWindowShouldClose(window)) {
 		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrameTime;
		lastFrameTime = currentFrame;
		//Processing the user input...
		ProcessInput(window);
		//Render...
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram.Activate();

		glm::vec3 lightPos = glm::vec3(glm::sin(glfwGetTime()), glm::cos(glfwGetTime()), 2.0f);
		glm::vec3 lightCol = glm::vec3(1.0f);

		shaderProgram.setVec3("cameraPosition", camera.cameraTransform.position);
		
		//SPOTLIGHT SETTINGS...
		shaderProgram.setVec3("lightPosition", camera.cameraTransform.position);
		shaderProgram.setVec3("lightColor", lightCol);
		//Re assigning the camera position & direction (camera front) values to the spotlight...
		shaderProgram.setVec3("spotLightData.spotLightPosition", camera.cameraTransform.position);
		shaderProgram.setVec3("spotLightData.spotLightDirection", glm::vec3(camera.cameraTransform.cameraFront.x, camera.cameraTransform.cameraFront.y, camera.cameraTransform.cameraFront.z));


		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		//Position, direction (z), camera up (y)
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.cameraParameters.fieldOfView), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
		
		shaderProgram.setMat4("view", view);
		shaderProgram.setMat4("projection", projection);
		//FOV, ASPECT RATION, NEAR CLIPPING PLANE, FAR CLIPPING PLANE...
		CrateVAO.Bind(); 
		glm::mat4 model;

		for (int i = 0; i < 10; i++) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.3f, 1.0f, 0.2f));

			shaderProgram.setMat4("model", model);

			CrateVAO.DrawTriangleArrays(0, 36);
		}

		lampShader.Activate();
		lampShader.setMat4("view", view);
		lampShader.setMat4("projection", projection);

		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lampShader.setMat4("model", model);

		lampShader.setVec3("lightColor", lightCol);

		//Draw light emitter cube...
		LightVAO.Bind();
		LightVAO.DrawTriangleArrays(0, 36);

		//Swap back buffer to front buffer to actually render the shit we want to see...
		glfwSwapBuffers(window);
		//Poll keyboard events to make the window interactable...
		glfwPollEvents();
	}
	shaderProgram.Delete();
	glfwTerminate();
	return 0;
}