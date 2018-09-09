#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <algorithm>
#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <shader_s.h>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Transformation", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Setup ImGui binding
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(window, true);

	// Setup style
	ImGui::StyleColorsDark();

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader ourShader("vertexShaderSource.txt", "fragmentShaderSource.txt");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	float vertices[] = {
		0.2f,  0.2f, 0.2f,  0.2, 0.1, 0.6,// 右前上
		0.2f, -0.2f, 0.2f,  0.5, 0.1, 0.3,// 右前下
		-0.2f, -0.2f, 0.2f, 0.7, 0.6, 0.1,// 左前下
		-0.2f, 0.2f, 0.2f,  0.6, 0.9, 0.5,// 左前上

		0.2f, 0.2f, -0.2f,  0.1, 0.4, 0.4,// 右后上
		0.2f, -0.2f, -0.2f, 0.1, 0.5, 0.9,// 右后下
		-0.2f, -0.2f, -0.2f,0.3, 0.2, 0.2,// 左后下
		-0.2f, 0.2f, -0.2f, 0.8, 0.3, 0.3// 左后上
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 2,
		0, 2, 3,

		0, 1, 5,
		0, 4, 5,

		0, 3, 4,
		3, 4, 7,

		2, 3, 6,
		3, 6, 7,

		4, 6, 7,
		4, 5, 6,

		1, 2, 5,
		2, 5, 6
	};
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	ourShader.use();

	bool isRotation = false;
	bool isScaling = false;
	bool isTranslation_h = false;
	bool isTranslation_v = false;
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);

		// render
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_DEPTH_TEST);

		// draw our first triangle
		ourShader.use();
		glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
								//glDrawArrays(GL_TRIANGLES, 0, 6);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glm::mat4 transform;
		float timeValue = glfwGetTime();
		// 设置Gui
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("Transformation");
		if(ImGui::Button("horizontal translation")) {
			isTranslation_h = true;
			isTranslation_v = false;
			isRotation = false;
			isScaling = false;
		}
		if (ImGui::Button("vertical translation")) {
			isTranslation_v = true;
			isTranslation_h = false;
			isRotation = false;
			isScaling = false;
		}
		if (ImGui::Button("Rotation")) {
			isRotation = true;
			isTranslation_h = false;
			isTranslation_v = false;
			isScaling = false;
		}
		if(ImGui::Button("Scaling")) {
			isScaling = true;
			isRotation = false;
			isTranslation_h = false;
			isTranslation_v = false;
		}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		if (isTranslation_h) {
			transform = glm::translate(transform, glm::vec3(sin(timeValue * 1.5) / 2, 0.0f, 0.0f));
		}
		if (isTranslation_v) {
			transform = glm::translate(transform, glm::vec3(0.0f, sin(timeValue * 1.5) / 2, 0.0f));
		}
		if (isRotation) {
			float angleValue = timeValue * 90;
			transform = glm::rotate(transform, glm::radians(-angleValue), glm::vec3(0.0, 1.0, 1.0));
		}
		if (isScaling) {
			transform = glm::scale(transform, glm::vec3(abs(sin(timeValue)) * 2 + 0.5, abs(sin(timeValue)) * 2 + 0.5, abs(sin(timeValue)) * 2 + 0.5));
		}
		transform = glm::rotate(transform, glm::radians(-45.0f), glm::vec3(1.0, 1.0, 1.0));
		unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		

		// glBindVertexArray(0); // no need to unbind it every time 

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}