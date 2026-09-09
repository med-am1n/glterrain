#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "camera.h"
#include <stb_image.h>
#include <filesystem.h>
#include <mesh.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void window_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
GLFWwindow *startGLFW(int width, int height, const char *title, GLFWframebuffersizefun fb_cb, GLFWcursorposfun mouse_cb, GLFWscrollfun scroll_cb);
unsigned int LoadTexture(const char *path);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int windowWidth = SCR_WIDTH;
int windowHeight = SCR_HEIGHT;
// camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
glm::vec3 lightPosInit = glm::vec3(1.0f, 1.0f, 0.0f);
glm::vec3 lightPos = lightPosInit;
float lastX = windowWidth / 2.0f;
float lastY = windowHeight / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{

    std::cout << "Hello, World!" << std::endl;

    glfwInit();

    GLFWwindow *window = startGLFW(windowWidth, windowHeight, "OpenGL",
                                   framebuffer_size_callback,
                                   mouse_callback,
                                   scroll_callback);

    if (!window)
        return -1;

    // initialize GLAD
    if (!gladLoadGL(glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }
    // build and compile the shader program
    Shader floorShader("../src/shaders/shader.vs", "../src/shaders/shader.fs");
    Shader lightShader("../src/shaders/lightShader.vs", "../src/shaders/lightShader.fs");

    std::vector<Vertex> vertices =
        {
            {
                glm::vec3(-10.0f, 0.0f, 10.0f), // Position
                glm::vec3(0.0f, 1.0f, 0.0f),    // Normal
                glm::vec2(0.0f, 0.0f),          // TexCoords
                glm::vec3(0.0f),
                glm::vec3(0.0f),
                {0},   // m_BoneIDs
                {0.0f} // m_Weights
            },
            {glm::vec3(-10.0f, 0.0f, -10.0f),
             glm::vec3(0.0f, 1.0f, 0.0f),
             glm::vec2(0.0f, 10.0f),
             glm::vec3(0.0f),
             glm::vec3(0.0f),
             {0},
             {0.0f}},
            {glm::vec3(10.0f, 0.0f, -10.0f),
             glm::vec3(0.0f, 1.0f, 0.0f),
             glm::vec2(10.0f, 10.0f),
             glm::vec3(0.0f),
             glm::vec3(0.0f),
             {0},
             {0.0f}},
            {glm::vec3(10.0f, 0.0f, 10.0f),
             glm::vec3(0.0f, 1.0f, 0.0f),
             glm::vec2(10.0f, 0.0f),
             glm::vec3(0.0f),
             glm::vec3(0.0f),
             {0},
             {0.0f}}};

    std::vector<unsigned int> indices =
        {
            0, 1, 2,
            0, 2, 3};

    std::vector<Vertex> lightVertices = {
        {{-0.5f, -0.5f, -0.5f}},
        {{0.5f, -0.5f, -0.5f}},
        {{0.5f, 0.5f, -0.5f}},
        {{-0.5f, 0.5f, -0.5f}},

        {{-0.5f, -0.5f, 0.5f}},
        {{0.5f, -0.5f, 0.5f}},
        {{0.5f, 0.5f, 0.5f}},
        {{-0.5f, 0.5f, 0.5f}}};

    std::vector<unsigned int> lightIndices = {
        // back
        0, 1, 2,
        2, 3, 0,
        // front
        4, 5, 6,
        6, 7, 4,
        // left
        0, 3, 7,
        7, 4, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // bottom
        0, 4, 5,
        5, 1, 0,
        // top
        3, 2, 6,
        6, 7, 3};

    std::vector<Texture> textures;
    Texture diffuseTexture(
        LoadTexture(FileSystem::getPath("resources/textures/planks.png").c_str()),
        "texture_diffuse",
        FileSystem::getPath("resources/textures/planks.png"));

    Texture specularTexture(
        LoadTexture(FileSystem::getPath("resources/textures/planksSpec.png").c_str()),
        "texture_specular",
        FileSystem::getPath("resources/textures/planksSpec.png"));

    textures.push_back(diffuseTexture);
    textures.push_back(specularTexture);

    Mesh floor(vertices, indices, textures);
    Mesh light(lightVertices, lightIndices, {});

    glEnable(GL_DEPTH_TEST);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        processInput(window);

        // rendering commands here

        // Whenever we call glClear and clear the color buffer,
        // the entire color buffer will be filled with the color asconfigured by glClearColor.
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        floorShader.use();
        floorShader.setVec3("viewPos", camera.Position);
        // material properties
        floorShader.setFloat("shininess", 64.0f);
        // light source properties
        floorShader.setVec3("pointLight.position", lightPos);
        floorShader.setVec3("pointLight.ambient", 0.2f, 0.2f, 0.2f);
        floorShader.setVec3("pointLight.diffuse", 0.5f, 0.5f, 0.5f);
        floorShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        floorShader.setFloat("pointLight.a", 0.1f);
        floorShader.setFloat("pointLight.b", 0.7f);
        floorShader.setFloat("pointLight.c", 1.0f);

        // create transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        floorShader.setMat4("projection", projection);
        // camera
        glm::mat4 view = camera.GetViewMatrix();
        floorShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        float angle = 45.0f;
        // model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        floorShader.setMat4("model", model);

        floor.Draw(floorShader);

        // draw the lamp object
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        // model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, lightPosInit);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightPos = glm::vec3(model[3]);
        lightShader.setMat4("model", model);
        light.Draw(lightShader);

        // Double buffering:
        // Rendering is done in a hidden back buffer while the front buffer is displayed on screen.
        // Once rendering is complete, buffers are swapped (glfwSwapBuffers) to avoid flickering
        // and ensure the user only sees fully rendered frames -no flickering issues.
        glfwSwapBuffers(window);

        // check and call events
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

GLFWwindow *startGLFW(int width, int height, const char *title, GLFWframebuffersizefun fb_cb, GLFWcursorposfun mouse_cb, GLFWscrollfun scroll_cb)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window)
    {
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

unsigned int LoadTexture(const char *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data)
    {
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

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void window_size_callback(GLFWwindow *window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
