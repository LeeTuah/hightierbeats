# ifndef _SHADERS_HPP__
# define _SHADERS_HPP__

# include <glad/glad.h>
# include <GLFW/glfw3.h>

# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>

void print(std::string message) {
	std::cout << "[!] " << message << std::flush;
}

class Shader {
public:
	unsigned int ID;

	Shader(std::string vertex_shader_path, std::string fragment_shader_path);

	void use();

	void set_int(std::string name, int value);
	void set_bool(std::string name, bool value);
	void set_float(std::string name, float value);

	void set_float2(std::string name, float x, float y);
	void set_float2(std::string name, glm::vec2 value);

	void set_float3(std::string name, float x, float y, float z);
	void set_float3(std::string name, glm::vec3 value);

	void set_float4(std::string name, float x, float y, float z, float w);
	void set_float4(std::string name, glm::vec4 value);

	void set_mat4(std::string name, glm::mat4 value);
};

Shader::Shader(std::string vertex_shader_path, std::string fragment_shader_path) {
	std::string vertex_shader_str, fragment_shader_str;
	std::ifstream v_file, f_file;

	try {
		std::stringstream v_str, f_str;

		v_file.open(vertex_shader_path.c_str());
		f_file.open(fragment_shader_path.c_str());

		v_str << v_file.rdbuf();
		f_str << f_file.rdbuf();

		v_file.close();
		f_file.close();

		vertex_shader_str = v_str.str();
		fragment_shader_str = f_str.str();
	} catch (std::ifstream::failure &e) {
		print("Could not read the shader files!");
	}

	const char* vertex_str = vertex_shader_str.c_str();
	const char* fragment_str = fragment_shader_str.c_str();

	unsigned int vertex_shader, fragment_shader;
	int success;
	char info_log[512];

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_str, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (not success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		print("Vertex Shader failed to compile!\n" + std::string(info_log) + "\n" + vertex_shader_path + "\n");
	}

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_str, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (not success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		print("Fragment Shader failed to compile!\n" + std::string(info_log) + "\n" + fragment_shader_path + "\n");
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertex_shader);
	glAttachShader(ID, fragment_shader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (not success) {
		glGetProgramInfoLog(ID, 512, NULL, info_log);
		print("Shaders failed to link!\n" + std::string(info_log) + "\n" + vertex_shader_path + " and " + fragment_shader_path + " \n");
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::set_bool(std::string name, bool value) {
	int location = glGetUniformLocation(ID, name.c_str());
	glUniform1i(location, value);
}

void Shader::set_int(std::string name, int value) {
	int location = glGetUniformLocation(ID, name.c_str());
	glUniform1i(location, value);
}

void Shader::set_float(std::string name, float value) {
	int location = glGetUniformLocation(ID, name.c_str());
	glUniform1f(location, value);
}

void Shader::set_float2(std::string name, float x, float y) {
	int location = glGetUniformLocation(ID, name.c_str());
	glUniform2f(location, x, y);
}

void Shader::set_float2(std::string name, glm::vec2 value) {
	int location = glGetUniformLocation(ID, name.c_str());
	glUniform2f(location, value.x, value.y);
}

void Shader::set_float3(std::string name, float x, float y, float z) {
	int location = glGetUniformLocation(ID, name.c_str());
	glUniform3f(location, x, y, z);
}

void Shader::set_float3(std::string name, glm::vec3 value) {
	int location = glGetUniformLocation(ID, name.c_str());
	glUniform3f(location, value.x, value.y, value.z);
}

void Shader::set_float4(std::string name, float x, float y, float z, float w) {
	int location = glGetUniformLocation(ID, name.c_str());
	glUniform4f(location, x, y, z, w);
}

void Shader::set_float4(std::string name, glm::vec4 value) {
	int location = glGetUniformLocation(ID, name.c_str());
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::set_mat4(std::string name, glm::mat4 value) {
	int location = glGetUniformLocation(ID, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

# endif