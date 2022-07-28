#ifndef CAMERA_CLASS_HEADER
#define CAMERA_CLASS_HEADER
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
struct Transfrom
{
	glm::vec3 position;
	glm::vec3 cameraFront;
	glm::vec3 cameraRight;
	glm::vec3 cameraUp;
};
struct CameraParams {
	float yaw;
	float pitch;
	float movementSpeed;
	float fieldOfView;
};
enum CameraMovementMode {
	FORWARD,
	BACKWARD,
	RIGHT,
	LEFT,
	UP,
	DOWN
};
class Camera {
	
public:
	Camera();
	Camera(glm::vec3 startingPosition, glm::vec3 startingCameraFront);
	~Camera();

	Transfrom cameraTransform;
	glm::vec3 worldUp;
	CameraParams cameraParameters;
	//returs the view matrix...
	glm::mat4 GetViewMatrix();
	//Calculate camera transform values...
	void ProcessKeyboard(CameraMovementMode direction, float deltaTime);
	//Process Mouse Movement...
	void ProcessMouseMovement(float xOffset, float yOffset, float deltaTime);
	//Process Scroll Input / Zoom in - Zoom out...
	void ProcessMouseScroll(float yOffset);
	//Updates camera vectors after every camera/mouse movement...
	glm::vec3 UpdateCameraVectors();
};
#endif