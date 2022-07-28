#include "CameraClass.h"
Camera::Camera() {
	cameraTransform.position = glm::vec3(0.0f, 0.0f, -3.0f);
	cameraTransform.cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	cameraParameters.yaw = -90.0f;
	cameraParameters.pitch = 0.0f;
	cameraParameters.fieldOfView = 45.0f;
	cameraParameters.movementSpeed = 5.0f;
}
Camera::Camera(glm::vec3 startingCameraPosition, glm::vec3 startingCameraFront)
{
	cameraTransform.position = startingCameraPosition;
	cameraTransform.cameraFront = startingCameraFront;
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	cameraParameters.yaw = -90.0f;
	cameraParameters.pitch = 0.0f;
	cameraParameters.fieldOfView = 45.0f;
	cameraParameters.movementSpeed = 5.0f;

	UpdateCameraVectors();
}
Camera::~Camera(){}
glm::mat4 Camera::GetViewMatrix() {
	return glm::lookAt(cameraTransform.position, cameraTransform.position + cameraTransform.cameraFront, cameraTransform.cameraUp);
}
void Camera::ProcessKeyboard(CameraMovementMode direction, float deltaTime) {
	float velocity = cameraParameters.movementSpeed * deltaTime;
	if (direction == FORWARD) 
		cameraTransform.position += cameraTransform.cameraFront * velocity;
	if (direction == BACKWARD)
		cameraTransform.position -= cameraTransform.cameraFront * velocity;
	if (direction == LEFT)
		cameraTransform.position -= cameraTransform.cameraRight * velocity;
	if (direction == RIGHT)
		cameraTransform.position += cameraTransform.cameraRight * velocity;
	if (direction == UP)
		cameraTransform.position += cameraTransform.cameraUp * velocity;
	if (direction == DOWN)
		cameraTransform.position -= cameraTransform.cameraUp * velocity;
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, float deltaTime) {
	//TODO(constraining pitch, if pitch > 90 or pitch pitch < 90 clamp it)...
	cameraParameters.yaw += xOffset * deltaTime;
	cameraParameters.pitch -= yOffset * deltaTime;

	UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset) {
	cameraParameters.fieldOfView -= yOffset;
	if (cameraParameters.fieldOfView < 1.0f) {
		cameraParameters.fieldOfView = 1.0f;
	}
	if (cameraParameters.fieldOfView >= 90.0f) {
		cameraParameters.fieldOfView = 90.0f;
	}
}

glm::vec3 Camera::UpdateCameraVectors() {
	glm::vec3 direction;

	direction.x = glm::cos(glm::radians(cameraParameters.yaw));
	direction.y = glm::sin(glm::radians(cameraParameters.pitch));
	direction.z = glm::sin(glm::radians(cameraParameters.yaw));

	cameraTransform.cameraFront = glm::normalize(direction);
	cameraTransform.cameraRight = glm::normalize(glm::cross(cameraTransform.cameraFront,worldUp));
	cameraTransform.cameraUp = glm::normalize(glm::cross(cameraTransform.cameraRight, cameraTransform.cameraFront));
	return direction;
}