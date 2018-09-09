#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "shader_s.h"
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void drawBezier();
void setPoint(int x, int y);
float calQ(float t, int isx);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Bezier
float vertices[4020];
unsigned int bezierVBO, bezierVAO;
int lineCount = 0;

// four points
float points[8];
int pcount = 0;
unsigned int fourVBO = 0, fourVAO = 0;
int nearstPoint;

// pos of mouse
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

bool isMouseLeftPress = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	 // glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bezier", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Setup ImGui binding
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(window, true);

	// Setup style
	ImGui::StyleColorsDark();

	// set callback
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader bezier("points.vert", "points.frag");
	// different color
	Shader fourPoints("points.vert", "points2.frag");

	// four points
	glGenVertexArrays(1, &fourVAO);
	glGenBuffers(1, &fourVBO);
	glBindVertexArray(fourVAO);
	glBindBuffer(GL_ARRAY_BUFFER, fourVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * pcount, points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	fourPoints.use();

	// bezier
	glGenVertexArrays(1, &bezierVAO);
	glGenBuffers(1, &bezierVBO);
	glBindVertexArray(bezierVAO);
	glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	bezier.use();

	float color[3] = { 1.0f, 0.5f, 0.2f };

	// render loop
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//input
		processInput(window);

		//render
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		bezier.use();
		glPointSize(4.0f);
		glBindVertexArray(bezierVAO);
		bezier.setVec3("ourColor", glm::vec3(color[0], color[1], color[2]));
		glDrawArrays(GL_POINTS, 0, lineCount);

		// four points
		fourPoints.use();
		glBindVertexArray(fourVAO);
		glPointSize(3.0f);
		glDrawArrays(GL_POINTS, 0, pcount);
		glLineWidth(2.0f);
		glDrawArrays(GL_LINE_STRIP, 0, pcount);

		// ImGui
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("Set Color");
		ImGui::ColorEdit3("Bezier Curve", color);
		ImGui::End();
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	glDeleteVertexArrays(1, &fourVAO);
	glDeleteVertexArrays(1, &bezierVAO);
	glDeleteBuffers(1, &fourVBO);
	glDeleteBuffers(1, &bezierVBO);
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	lastX = xpos;
	lastY = ypos;

	if (isMouseLeftPress) {
		// 单位转换
		float x = float(lastX), y = float(SCR_HEIGHT - lastY);
		float flnum = x - (float(SCR_WIDTH)) / 2;
		x = flnum / (float(SCR_WIDTH) / 2);
		flnum = y - (float(SCR_HEIGHT)) / 2;
		y = flnum / (float(SCR_HEIGHT) / 2);
		points[2 * nearstPoint] = x;
		points[2 * nearstPoint + 1] = y;
		glBindBuffer(GL_ARRAY_BUFFER, fourVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * pcount, points, GL_STATIC_DRAW);
		drawBezier();
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (pcount < 4) {
				setPoint(lastX, SCR_HEIGHT - lastY);
				glBindBuffer(GL_ARRAY_BUFFER, fourVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * pcount, points, GL_STATIC_DRAW);
				if (pcount == 4)
					drawBezier();
			}
			else {
				isMouseLeftPress = true;
				cout << "is mouse left button press = true" << endl;
				// 单位转换
				float x = float(lastX), y = float(SCR_HEIGHT - lastY);
				float flnum = x - (float(SCR_WIDTH)) / 2;
				x = flnum / (float(SCR_WIDTH) / 2);
				flnum = y - (float(SCR_HEIGHT)) / 2;
				y = flnum / (float(SCR_HEIGHT) / 2);
				// 计算最近点
				nearstPoint = 0;
				float mindistance = pow(x - points[0], 2) + pow(y - points[1], 2);
				for (int i = 1; i < 4; i++) {
					float d = pow(x - points[2 * i], 2) + pow(y - points[2 * i + 1], 2);
					if (d < mindistance) {
						nearstPoint = i;
						mindistance = d;
					}
				}
			}
		}
	}
	if (action == GLFW_RELEASE) {
		isMouseLeftPress = false;
		cout << "is mouse left button press = false" << endl;
	}
}

void drawBezier() {
	float t = 0.0;
	for (int i = 0; i < 2000; i++) {
		vertices[2 * i] = calQ(t, 0);
		vertices[2 * i + 1] = calQ(t, 1);
		t += 0.0005;
	}
	vertices[4000] = points[6];
	vertices[4001] = points[7];
	lineCount = 2001;
	glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * lineCount, vertices, GL_STATIC_DRAW);
}

void setPoint(int x, int y) {
	float flnum = x - (float(SCR_WIDTH)) / 2;
	points[2 * pcount] = flnum / (float(SCR_WIDTH) / 2);
	//vertices[2 * pcount] = points[2 * pcount];
	flnum = y - (float(SCR_HEIGHT)) / 2;
	points[2 * pcount + 1] = flnum / (float(SCR_HEIGHT) / 2);
	//points[2 * pcount + 1] = vertices[2 * pcount + 1];
	pcount++;
}

float calQ(float t, int isx) {
	float result = pow(1 - t, 3) * points[isx + 0] + 3 * t * pow(1 - t, 2) * points[isx + 2];
	result += 3 * t * t * (1 - t) * points[isx + 4] + pow(t, 3) * points[isx + 6];
	return result;
}