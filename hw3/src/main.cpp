#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void drawLine(int x1, int y1, int x2, int y2);
void drawCircle(int x, int y, int radius);
void drawEightPoint(int x, int y, int xi, int yi);
void setPoint(int x, int y);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

unsigned int view_width = SCR_WIDTH;
unsigned int view_height = SCR_HEIGHT;

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"}\0";
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

float vertices[10000];
int pointCount = 362;

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Draw Line", NULL, NULL);
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
	

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*pointCount, vertices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glUseProgram(shaderProgram);

	int pointT[3][2] = { 0, 0, 0, 0, 0, 0 };
	int pointC[1][2] = { 0, 0 };
	int radius = 0;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);

		// render
		// ------
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw our first triangle
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		

		// draw points
		glPointSize(2.0f);
		glDrawArrays(GL_POINTS, 0, pointCount);

		//设置Gui
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("Draw trangle/circle by Bresenham algorithm");
		ImGui::Text("Parameter of trangle：");
		ImGui::Text("first point：");
		ImGui::SliderInt("width1", &pointT[0][0], 0, view_width);
		ImGui::SliderInt("height1", &pointT[0][1], 0, view_height);
		ImGui::Text("second point：");
		ImGui::SliderInt("width2", &pointT[1][0], 0, view_width);
		ImGui::SliderInt("height2", &pointT[1][1], 0, view_height);
		ImGui::Text("third point：");
		ImGui::SliderInt("width3", &pointT[2][0], 0, view_width);
		ImGui::SliderInt("height3", &pointT[2][1], 0, view_height);
		if (ImGui::Button("Draw trangle!")) {
			pointCount = 0;
			drawLine(pointT[0][0], pointT[0][1], pointT[1][0], pointT[1][1]);
			drawLine(pointT[1][0], pointT[1][1], pointT[2][0], pointT[2][1]);
			drawLine(pointT[0][0], pointT[0][1], pointT[2][0], pointT[2][1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * pointCount, vertices, GL_STATIC_DRAW);
		}

		ImGui::Text("\nParameter of circle：");
		ImGui::Text("center of the circle：");
		ImGui::SliderInt("width", &pointC[0][0], 0, view_height);
		ImGui::SliderInt("height", &pointC[0][1], 0, view_width);
		ImGui::SliderInt("radius：", &radius, 0, 800);
		if (ImGui::Button("Draw circle!")) {
			pointCount = 0;
			drawCircle(pointC[0][0], pointC[0][1], radius);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * pointCount, vertices, GL_STATIC_DRAW);
		}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
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
	view_height = height;
	view_width = width;
}

void drawLine(int x1, int y1, int x2, int y2) {
	// 判断斜率是否大于1，角度是否大于45°
	int isKbig = (abs(y2 - y1) > abs(x2 - x1)) ? 1 : 0;
	int temp;
	// 如果斜率大于1，交换x和y
	if (isKbig) {
		temp = x1;
		x1 = y1;
		y1 = temp;
		temp = x2;
		x2 = y2;
		y2 = temp;
	}
	// 让point1在point2的左边
	if (x1 > x2) {
		temp = x1;
		x1 = x2;
		x2 = temp;
		temp = y1;
		y1 = y2;
		y2 = temp;
	}
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int dy2 = 2 * dy;
	int dx2 = 2 * dx;
	int dy2_dx2 = dy2 - dx2;
	int yi = y1;
	int p;
	int addnum = (y1 < y2) ? 1 : -1;
	p = 2 * dy - dx;
	// 使用Bresenham算法计算哪些是要画的顶点
	for (int xi = x1; xi <= x2; xi++) {
		if (isKbig)
			setPoint(yi, xi);
		else
			setPoint(xi, yi);
		if (p <= 0) {
			p += dy2;
		}
		else {
			yi += addnum;
			p += dy2_dx2;
		}
	}
}

void drawCircle(int x, int y, int r) {
	int p = 3 - 2 * r;
	int yi = r;
	for (int xi = 0; xi < yi; xi++) {
		drawEightPoint(x, y, xi, yi);
		if (p < 0) {
			p = p + 2 * xi + 3;
		}
		else {
			p = p + 2 * (xi - yi) + 5;
			yi--;
		}
	}
}
//(x0,y0)是相对于（0,0）点的圆，之后将其中心平移到（x,y）
void drawEightPoint(int x, int y, int x0, int y0) {
	setPoint(x0 + x, y0 + y);
	setPoint(y0 + x, x0 + y);
	setPoint(-y0 + x, x0 + y);
	setPoint(-x0 + x, y0 + y);
	setPoint(-x0 + x, -y0 + y);
	setPoint(-y0 + x, -x0 + y);
	setPoint(y0 + x, -x0 + y);
	setPoint(x0 + x, -y0 + y);
}

void setPoint(int x, int y) {
	float flnum = x - (float(view_width)) / 2;
	vertices[2 * pointCount] = flnum / (float(view_width) / 2);
	flnum = y - (float(view_height)) / 2;
	vertices[2 * pointCount + 1] = flnum / (float(view_height) / 2);
	pointCount++;
}