#include <GLFW/glfw3.h>
#include <glad/gl.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <iostream>
#include <string>
#include <chrono>


int main() {
	//Init Window
	if (!glfwInit) return1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Viewer (Client)", nullptr, nullptr);
	if (!window) { glfwTerminate(); return 1; }
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	if (!gladLoadGL(glfwGetProcAddress)) {
		std::cerr << "Failed to init GL\n"; return 1;
	}

	//Init Renderer
	GLRenderer renderer;
	renderer.Init();
	
	//Timing / FPS
	using clock = std::chrono::high_resolution_clock;
	auto last = clock::now();
	double fps = 0.0;
	double frame_time_avg = 0.0;

	AppendLog("App Started");
	auto lastSampleTime = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(window)) {
		//Timing
		auto now = clock::now();
		double dt = std::chrono::duration<double>(now - last).count();
		last = now;

		//Poll Events
		glfwPollEvents();

		int display_w = 1280, display_h = 720;
		glfwGetFramebufferSize(window, &display_w, &display_h);

		//Render
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
	}
}