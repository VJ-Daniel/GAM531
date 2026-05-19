#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

const int WIDTH = 800;
const int HEIGHT = 600;

float circleX = 0.0f;
float circleY = 0.0f;
float speed = 0.01f;

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;

uniform vec2 offset;

void main()
{
    gl_Position = vec4(aPos + offset, 0.0, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.4, 0.4, 1.0);
}
)";

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        circleY += speed;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        circleY -= speed;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        circleX -= speed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        circleX += speed;
}

unsigned int createShaderProgram()
{
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

std::vector<float> createCircleVertices(float radius, int segments)
{
    std::vector<float> vertices;

    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * 3.14159f * i / segments;

        float x = cos(angle) * radius;
        float y = sin(angle) * radius;

        vertices.push_back(x);
        vertices.push_back(y);
    }

    return vertices;
}

int main()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    GLFWwindow* window = glfwCreateWindow(
        mode->width,
        mode->height,
        "WASD Circle Movement",
        monitor,
        NULL
    );

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        return -1;

    glViewport(0, 0, mode->width, mode->height);

    unsigned int shaderProgram = createShaderProgram();

    std::vector<float> vertices = createCircleVertices(0.1f, 100);
    int vertexCount = vertices.size() / 2;

    unsigned int VAO;
    unsigned int VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        int offsetLocation = glGetUniformLocation(shaderProgram, "offset");
        glUniform2f(offsetLocation, circleX, circleY);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertexCount);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}