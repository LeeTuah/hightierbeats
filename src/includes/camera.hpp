# ifndef CAMERA_HPP_
# define CAMERA_HPP_

# include <glad/glad.h>
# include <GLFW/glfw3.h>

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>

# include <iostream>

class Camera {
public:
	glm::vec3 position;

	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	glm::vec3 world_up;

	float yaw;
	float pitch;

	float zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f), glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 get_lookat_matrix();

	void print_camera_coordinates();

private:
	void update_camera_vectors();
};

Camera::Camera(glm::vec3 position, glm::vec3 world_up) {
	this->position = position;
	this->world_up = world_up;

	yaw = 0.0f;
	pitch = 0.0f;
	zoom = 55.0f;

	update_camera_vectors();
}

glm::mat4 Camera::get_lookat_matrix() {
	return glm::lookAt(position, position + front, up);
}

void Camera::print_camera_coordinates() {
	std::cout << "Camera: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}

void Camera::update_camera_vectors() {
	glm::vec3 _front;

	_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	_front.y = sin(glm::radians(pitch));
	_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(_front);

	right = glm::normalize(glm::cross(front, world_up));
	up = glm::normalize(glm::cross(right, front));
}
# endif