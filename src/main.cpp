#include "camera.h"
#include "shader.h"
#include <GLFW/glfw3.h>
#include <cstddef>
#include <filesystem.h>
#include <glad/glad.h>
#include <iostream>
#include <mesh.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void window_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
GLFWwindow *startGLFW(int width, int height, const char *title,
                      GLFWframebuffersizefun fb_cb, GLFWcursorposfun mouse_cb,
                      GLFWscrollfun scroll_cb);
unsigned int LoadTexture(const char *path);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int windowWidth = SCR_WIDTH;
int windowHeight = SCR_HEIGHT;
unsigned int cameraSpeed = 100;
const unsigned int NUM_PATCH_PTS = 4;
// camera
Camera camera(glm::vec3(67.0f, 627.5f, 169.9f), glm::vec3(0.0f, 1.0f, 0.0f),
              -128.1f, -42.4f);
glm::vec3 lightPosInit = glm::vec3(1.0f, 1.0f, 0.0f);
glm::vec3 lightPos = lightPosInit;
float lastX = windowWidth / 2.0f;
float lastY = windowHeight / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// imgui
int visualizationMode = 0;
float noiseScale = 500.0f;
float heightScale = 600.0f;
float heightOffset = 600.0f;

int main() {

  std::cout << "Hello, World!" << std::endl;

  glfwInit();

  GLFWwindow *window =
      startGLFW(windowWidth, windowHeight, "OpenGL", framebuffer_size_callback,
                mouse_callback, scroll_callback);

  if (!window)
    return -1;

  // initialize GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD\n";
    return -1;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 410");
  
  // build and compile the shader program
  Shader terrainShader(
      "../src/shaders/terrain.vs", "../src/shaders/terrain.fs", nullptr,
      "../src/shaders/terrain.tcs", "../src/shaders/terrain.tes");
  
  const float terrainSize = 1024.0f;
  const int terrainWidth = 1024;
  const int terrainHeight = 1024;

  // vertex generation
  std::vector<float> vertices;
  unsigned rez = 20;
  for (unsigned i = 0; i <= rez - 1; i++) {
    for (unsigned j = 0; j <= rez - 1; j++) {
      vertices.push_back(-terrainWidth / 2.0f + terrainWidth * i / (float)rez); // v.x
      vertices.push_back(0.0f);                          // v.y
      vertices.push_back(-terrainHeight / 2.0f + terrainHeight * j / (float)rez); // v.z
      vertices.push_back(i / (float)rez);                 // u
      vertices.push_back(j / (float)rez);                 // v

      vertices.push_back(-terrainWidth / 2.0f + terrainWidth * (i + 1) / (float)rez); // v.x
      vertices.push_back(0.0f);                                // v.y
      vertices.push_back(-terrainHeight / 2.0f + terrainHeight * j / (float)rez); // v.z
      vertices.push_back((i + 1) / (float)rez);           // u
      vertices.push_back(j / (float)rez);                 // v

      vertices.push_back(-terrainWidth / 2.0f + terrainWidth * i / (float)rez); // v.x
      vertices.push_back(0.0f);                          // v.y
      vertices.push_back(-terrainHeight / 2.0f + terrainHeight * (j + 1) / (float)rez); // v.z
      vertices.push_back(i / (float)rez);                       // u
      vertices.push_back((j + 1) / (float)rez);                 // v

      vertices.push_back(-terrainWidth / 2.0f + terrainWidth * (i + 1) / (float)rez); // v.x
      vertices.push_back(0.0f);                                // v.y
      vertices.push_back(-terrainHeight / 2.0f + terrainHeight * (j + 1) / (float)rez); // v.z
      vertices.push_back((i + 1) / (float)rez);                 // u
      vertices.push_back((j + 1) / (float)rez);                 // v
    }
  }

  unsigned int terrainVAO, terrainVBO;
  glGenVertexArrays(1, &terrainVAO);
  glBindVertexArray(terrainVAO);

  glGenBuffers(1, &terrainVBO);
  glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // texCoord attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(sizeof(float) * 3));
  glEnableVertexAttribArray(1);

  glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
  glEnable(GL_DEPTH_TEST);

  // render loop
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    // input
    processInput(window);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Terrain Controls");

    ImGui::DragFloat("Noise Scale", &noiseScale, 1.0f, 10.0f, 2000.0f);
    ImGui::DragFloat("Height Scale", &heightScale, 1.0f, 0.0f, 2000.0f);
    ImGui::DragFloat("Height Offset", &heightOffset, 1.0f, -1000.0f, 2000.0f);

    ImGui::Separator();

    const char* modes[] = {"vertices", "Normals", "Lighting"};

    ImGui::Combo("Visualization", &visualizationMode, modes, IM_ARRAYSIZE(modes));

    ImGui::End();

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    terrainShader.use();
    
    if (visualizationMode == 0)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    // light
    float angle = glfwGetTime() * 0.5f; // rotation speed
    glm::vec3 lightDir(cos(angle), -1.0f, sin(angle));
    lightDir = glm::normalize(lightDir);
    terrainShader.setVec3("lightDir", lightDir);   
    
    terrainShader.setInt("visualizationMode", visualizationMode);
    terrainShader.setFloat("terrainSize", terrainSize);
    terrainShader.setFloat("noiseScale", noiseScale);
    terrainShader.setFloat("heightScale", heightScale);
    terrainShader.setFloat("heightOffset", heightOffset);

    terrainShader.setVec3("viewPos", camera.Position);
    
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    terrainShader.setMat4("projection", projection);
    terrainShader.setMat4("view", view);

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    terrainShader.setMat4("model", model);

    // render the cube
    glBindVertexArray(terrainVAO);
    glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glfwSwapBuffers(window);

    // check and call events
    glfwPollEvents();
  }
  
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  
  glfwTerminate();

  return 0;
}

GLFWwindow *startGLFW(int width, int height, const char *title,
                      GLFWframebuffersizefun fb_cb, GLFWcursorposfun mouse_cb,
                      GLFWscrollfun scroll_cb) {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window) {
    std::cout << "Failed to create GLFW window\n";
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);

  glfwSetFramebufferSizeCallback(window, fb_cb);
  glfwSetCursorPosCallback(window, mouse_cb);
  glfwSetScrollCallback(window, scroll_cb);
  glfwSetWindowSizeCallback(window, window_size_callback);
  return window;
}

unsigned int LoadTexture(const char *path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrChannels;
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

  if (data) {
    GLenum format;

    if (nrChannels == 1)
      format = GL_RED;
    else if (nrChannels == 3)
      format = GL_RGB;
    else if (nrChannels == 4)
      format = GL_RGBA;
    else
      format = GL_RGB;

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Failed to load texture: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime * cameraSpeed);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime * cameraSpeed);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime * cameraSpeed);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime * cameraSpeed);
}
// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

void window_size_callback(GLFWwindow *window, int width, int height) {
  windowWidth = width;
  windowHeight = height;
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
