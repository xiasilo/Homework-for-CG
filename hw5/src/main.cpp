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

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"	ourColor = aColor;\n"
"}\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Projection", NULL, NULL);
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

	// build and compile our shader program
	// vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// link shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

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

	float left = -4.0f;
	float right = 4.0f;
	float up = 4.0f;
	float down = -4.0f;
	float Near = 0.1f;
	float Far = 100.0f;
	float angle = 45.0;
	bool isOrtho = false;
	bool isPers = false;
	bool isView = false;
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
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
								//glDrawArrays(GL_TRIANGLES, 0, 6);


		glm::mat4 model;
		model = translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		glm::mat4 view;
		view = translate(view, glm::vec3(0.0f, 0.0f, 0.0f));
		glm::mat4 projection;
		projection = translate(projection, glm::vec3(0.0f, 0.0f, 0.0f));
		
		//正交投影
		if (isOrtho) {
			model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5f));
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0));
			projection = glm::ortho(left, right, down, up, Near, Far);
		}
		//透视投影
		if (isPers) {
			model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5f));
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0));
			projection = glm::perspective(glm::radians(angle), (float)SCR_WIDTH / (float)SCR_HEIGHT, Near, Far);
		}
		//转圈
		if (isView) {
			float radius = 3.0f;
			float camX = sin(glfwGetTime()) * radius;
			float camZ = cos(glfwGetTime()) * radius;
			view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
			//view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
			projection = glm::perspective(glm::radians(angle), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		}
		//传递给shader
		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
		unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//画cubic
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		// 设置Gui
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("Projection");
		ImGui::SliderFloat("left", &left, -5.0, 0.0);
		ImGui::SliderFloat("right", &right, 0.0, 5.0);
		ImGui::SliderFloat("down", &down, -5.0, 0.0);
		ImGui::SliderFloat("up", &up, 0.0, 5.0);
		ImGui::SliderFloat("near", &Near, 0.1, 10.0);
		ImGui::SliderFloat("far", &Far, 50.0, 100.0);
		ImGui::SliderFloat("angle", &angle, -180.0, 180.0);
		if (ImGui::Button("Orthographic")) {
			isOrtho = true;
			isPers = false;
			isView = false;
		}
		if (ImGui::Button("Perspective")) {
			isOrtho = false;
			isPers = true;
			isView = false;
		}
		if (ImGui::Button("view change")) {
			isView = true;
			isOrtho = false;
			isPers = false;
		}
		if (ImGui::Button("reset")) {
			isOrtho = false;
			isPers = false;
			isView = false;
		}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
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