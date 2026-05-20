#include "pawn.h"
#include <cmath>

static const char* VERTEX_SRC = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
uniform vec2 offset;
void main()
{
    gl_Position = vec4(aPos + offset, 0.0, 1.0);
}
)";

static const char* FRAGMENT_SRC = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 pieceColor;
void main()
{
    FragColor = vec4(pieceColor, 1.0);
}
)";

Pawn::Pawn(float r, float g, float b)
    : m_r(r), m_g(g), m_b(b)
    , m_shaderProgram(0), m_VAO(0), m_VBO(0), m_vertexCount(0)
{
}

Pawn::~Pawn()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteProgram(m_shaderProgram);
}

unsigned int Pawn::compileShader(unsigned int type, const char* src)
{
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    return id;
}

void Pawn::init()
{
    unsigned int vs = compileShader(GL_VERTEX_SHADER, VERTEX_SRC);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, FRAGMENT_SRC);
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vs);
    glAttachShader(m_shaderProgram, fs);
    glLinkProgram(m_shaderProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);

    std::vector<float> verts = buildVertices();
    m_vertexCount = static_cast<int>(verts.size() / 2);

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Pawn::draw(float x, float y)
{
    glUseProgram(m_shaderProgram);
    glUniform3f(glGetUniformLocation(m_shaderProgram, "pieceColor"), m_r, m_g, m_b);
    glUniform2f(glGetUniformLocation(m_shaderProgram, "offset"), x, y);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    glBindVertexArray(0);
}

// --- geometry helpers ---

void Pawn::addRect(std::vector<float>& v, float x0, float y0, float x1, float y1)
{
    v.insert(v.end(), {
        x0, y0,  x1, y0,  x1, y1,
        x0, y0,  x1, y1,  x0, y1
        });
}

void Pawn::addTrapezoid(std::vector<float>& v,
    float bx0, float bx1, float by,
    float tx0, float tx1, float ty)
{
    v.insert(v.end(), {
        bx0, by,  bx1, by,  tx1, ty,
        bx0, by,  tx1, ty,  tx0, ty
        });
}

void Pawn::addTriangle(std::vector<float>& v,
    float x0, float y0,
    float x1, float y1,
    float x2, float y2)
{
    v.insert(v.end(), { x0, y0, x1, y1, x2, y2 });
}

void Pawn::addCircle(std::vector<float>& v, float cx, float cy, float radius, int segments)
{
    const float PI = 3.14159265f;
    for (int i = 0; i < segments; i++)
    {
        float a0 = 2.0f * PI * i / segments;
        float a1 = 2.0f * PI * (i + 1) / segments;
        v.insert(v.end(), {
            cx, cy,
            cx + radius * cosf(a0), cy + radius * sinf(a0),
            cx + radius * cosf(a1), cy + radius * sinf(a1)
            });
    }
}

// --- Pawn silhouette geometry ---
// Coordinates in NDC local space, centered at origin, scaled to fit inside one
// board square (0.25 x 0.25 NDC).  Total size ~0.15 wide x 0.225 tall.
// Use draw(x, y) where (x, y) is the NDC centre of the target board square.
std::vector<float> Pawn::buildVertices()
{
    std::vector<float> v;

    // Wide base
    addRect(v, -0.075f, -0.110f, 0.075f, -0.075f);

    // Tapered body
    addTrapezoid(v, -0.045f, 0.045f, -0.075f, -0.020f, 0.020f, -0.005f);

    // Neck
    addRect(v, -0.060f, -0.005f, 0.060f, 0.025f);

    // Head
    addCircle(v, 0.000f, 0.055f, 0.0425f, 20);

    return v;
}
