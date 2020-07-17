#include "glad.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <chrono>
#include <string>
#include <filesystem>
#include <thread>

#include "Shader.hpp"
#include "Camera.hpp"

#define  STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

struct Light {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

float SCREEN_WIDTH = 1920.0f;
float SCREEN_HEIGHT = 1080.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 800 / 2.0f;
float lastY = 600 / 2.0f;
bool firstMouse = true;

bool pause = false;

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, Camera camera, float deltaTime) {
	bool shiftPressed = false;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
		shiftPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.processKeyboard(FORWARD, deltaTime, shiftPressed);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.processKeyboard(BACKWARDS, deltaTime, shiftPressed);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.processKeyboard(LEFT, deltaTime, shiftPressed);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.processKeyboard(RIGHT, deltaTime, shiftPressed);
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.processKeyboard(UP, deltaTime, shiftPressed);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		camera.processKeyboard(DOWN, deltaTime, shiftPressed);
	}

	static bool pressed = false;

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		if (!pressed) {
			static bool kek = true;
			if (kek) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				kek = false;
				pressed = true;
			}
			else {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				kek = true;
				pressed = true;
			}
		}
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
		pressed = false;
	}


	static bool pressed2 = false;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		if (!pressed2) {
			if (pause) {
				pause = false;
				pressed2 = true;
				firstMouse = true;
			}
			else {
				pause = true;
				pressed2 = true;
			}
		}
	}

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
		pressed2 = false;
	}

	shiftPressed = false;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.processMouseMovement(xoffset, yoffset);
}

unsigned int loadTexture(const char* path) {
	unsigned int ID;
	glGenTextures(1, &ID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

	if (data) {
		GLenum format;

		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, ID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Failed to load texture: " << path << std::endl;
		stbi_image_free(data);
	}

	return ID;
}

float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};

int main() {
	int fpsCount = 1;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Honk openGL", /*glfwGetPrimaryMonitor()*/ NULL,
	                                      NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to init GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// Shader shaderModule("Shaders\\VertexShader.vert", "Shaders\\FragmentShader.frag");
	Shader lightingShader("Shaders\\LightningVertex.vert", "Shaders\\DirectionalLightFragment.frag");
	Shader lampShader("Shaders\\LightningVertex.vert", "Shaders\\LightSource.frag");

	glEnable(GL_MULTISAMPLE);
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int diffuseMap = loadTexture("Textures\\container2.png");
	unsigned int specularMap = loadTexture("Textures\\container2_specular.png");

	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);

	lightingShader.setInt("material.specular", 1);
	lightingShader.setVec3("light.direction", 1.2f, -1.0f, -0.3f); // light direction from bottom

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);


	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);

	auto frameCounterStart = std::chrono::high_resolution_clock::now();

	auto frameStart = std::chrono::high_resolution_clock::now();
	auto frameEnd = std::chrono::high_resolution_clock::now();
	float dt = std::chrono::duration<float>(frameEnd - frameCounterStart).count();

	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	glm::mat4 model = glm::mat4(1.0f);

	Material cubeMaterial;
	cubeMaterial.ambient = glm::vec3(1.0f, 0.5f, 0.31f);
	cubeMaterial.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
	cubeMaterial.specular = glm::vec3(0.5f, 0.5f, 0.5f);
	cubeMaterial.shininess = 32.0f;

	Light light;
	light.position = lightPos;
	light.ambient = glm::vec3(0.2, 0.2f, 0.2f);
	light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	light.specular = glm::vec3(1.0f, 1.0f, 1.0f);

	lightingShader.use();
	lightingShader.setVec3("material.ambient", cubeMaterial.ambient);
	lightingShader.setVec3("material.diffuse", cubeMaterial.diffuse);
	lightingShader.setVec3("material.specular", cubeMaterial.specular);
	lightingShader.setFloat("material.shininess", cubeMaterial.shininess);

	lightingShader.setVec3("light.position", light.position);
	lightingShader.setVec3("light.ambient", light.ambient);
	lightingShader.setVec3("light.diffuse", light.diffuse);
	lightingShader.setVec3("light.specular", light.specular);

	while (!glfwWindowShouldClose(window)) {

		// input
		processInput(window, camera, dt);

		if (!pause) {

			dt = std::chrono::duration<float>(frameEnd - frameStart).count();
			frameStart = std::chrono::high_resolution_clock::now();

			// rendering here
			// ...
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

			// be sure to activate shader when setting uniforms/drawing objects
			lightingShader.use();

			// view/projection transformations
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f,
			                                        100.0f);
			glm::mat4 view = camera.getViewMatrix();
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);
			lightingShader.setVec3("viewPos", camera.Position);

			// world transformation

			model = glm::rotate(model, glm::radians(30.0f) * dt, glm::vec3(0.0f, 1.0f, 0.0f));
			lightingShader.setMat4("model", model);

			// render the cube
			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// draw the lamp object
			lampShader.use();
			lampShader.setMat4("projection", projection);
			lampShader.setMat4("view", view);
			glm::mat4 modelLight = glm::mat4(1.0f);
			modelLight = glm::translate(modelLight, lightPos);
			modelLight = glm::scale(modelLight, glm::vec3(0.2f)); // a smaller cube
			lampShader.setMat4("model", modelLight);

			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// fps counting

			auto duration = std::chrono::duration<float>(frameEnd - frameCounterStart);
			std::string fps = "OpenGL KEK || FPS: ";
			int fpsINT = (int)(fpsCount / duration.count());
			fps.append(std::to_string(fpsINT));
			glfwSetWindowTitle(window, fps.c_str());

			if (fpsCount > 100) {
				fpsCount = 0;
				frameCounterStart = std::chrono::high_resolution_clock::now();
			}
			fpsCount++;
		}
		else {
			glfwPollEvents();
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			continue;
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
		frameEnd = std::chrono::high_resolution_clock::now();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &VBO);
	// glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}
