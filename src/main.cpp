# include <glad/glad.h>
# include <GLFW/glfw3.h>

# include "game.hpp"

# include "imgui.h"
# include "imgui_impl_glfw.h"
# include "imgui_impl_opengl3.h"

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 720;

float delta_time = 0.0f;
float last_time = 0.0f;

Game* htb_engine = nullptr;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);

	if (htb_engine != nullptr and width > 0 and height > 0)
		htb_engine->resize_fbo(width, height);
}

int main () {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HighTierBeats", NULL, NULL);
	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	float font_size = 14.0f;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontFromFileTTF("assets/VCR_OSD_MONO_1.001.ttf", font_size, nullptr, io.Fonts->GetGlyphRangesDefault());
	
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	htb_engine = new Game(window, SCR_WIDTH, SCR_HEIGHT);
	htb_engine->mapmaker_font_size = font_size;

	while (not glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float current_time = glfwGetTime();
		delta_time = current_time - last_time;
		last_time = current_time;

		htb_engine->check_for_collisions();

		htb_engine->process_input(window, delta_time);
		htb_engine->update(delta_time);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		htb_engine->render();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	
	glfwTerminate();
	return 0;
}

