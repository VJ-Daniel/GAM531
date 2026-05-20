#include "king.h"
#include <cmath>

// -----------------------------------------------------------------------
// Shaders — identical to Queen so a shared program would also work,
// but keeping them self-contained matches the existing project style.
// -----------------------------------------------------------------------
static const char* VERTEX_SRC = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
uniform vec2  offset;
uniform float scale;
void main()
{
    gl_Position = vec4(aPos * scale + offset, 0.0, 1.0);
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

// -----------------------------------------------------------------------
// Constructor / Destructor
// -----------------------------------------------------------------------
King::King(float r, float g, float b)
    : m_r(r), m_g(g), m_b(b)
    , m_shaderProgram(0), m_VAO(0), m_VBO(0), m_vertexCount(0)
{}

King::~King()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteProgram(m_shaderProgram);
}

// -----------------------------------------------------------------------
// Shader compilation
// -----------------------------------------------------------------------
unsigned int King::compileShader(unsigned int type, const char* src)
{
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    return id;
}

// -----------------------------------------------------------------------
// init — compile shaders, upload geometry once
// -----------------------------------------------------------------------
void King::init()
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
    glBufferData(GL_ARRAY_BUFFER,
        verts.size() * sizeof(float),
        verts.data(),
        GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
        2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(m_shaderProgram);
    glUniform1f(glGetUniformLocation(m_shaderProgram, "scale"), 1.0f);
    glUseProgram(0);
}

// -----------------------------------------------------------------------
// draw — called every frame with the NDC centre of the target square
// -----------------------------------------------------------------------
void King::draw(float x, float y)
{
    glUseProgram(m_shaderProgram);
    glBindVertexArray(m_VAO);

    GLint colorLoc = glGetUniformLocation(m_shaderProgram, "pieceColor");
    GLint offsetLoc = glGetUniformLocation(m_shaderProgram, "offset");
    GLint scaleLoc = glGetUniformLocation(m_shaderProgram, "scale");

    glUniform2f(offsetLoc, x, y);

    // --- Pass 1: outline (slightly larger, dark colour) ---
    glUniform3f(colorLoc, 0.0f, 0.0f, 0.0f);   // black outline
    glUniform1f(scaleLoc, 1.08f);               // 8% bigger
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);

    // --- Pass 2: piece fill (normal size) ---
    glUniform3f(colorLoc, m_r, m_g, m_b);
    glUniform1f(scaleLoc, 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);

    glBindVertexArray(0);
}

// -----------------------------------------------------------------------
// Geometry helpers (identical to Queen)
// -----------------------------------------------------------------------
void King::addRect(std::vector<float>& v,
    float x0, float y0, float x1, float y1)
{
    v.insert(v.end(), {
        x0, y0,  x1, y0,  x1, y1,
        x0, y0,  x1, y1,  x0, y1
        });
}

void King::addTrapezoid(std::vector<float>& v,
    float bx0, float bx1, float by,
    float tx0, float tx1, float ty)
{
    v.insert(v.end(), {
        bx0, by,  bx1, by,  tx1, ty,
        bx0, by,  tx1, ty,  tx0, ty
        });
}

void King::addTriangle(std::vector<float>& v,
    float x0, float y0,
    float x1, float y1,
    float x2, float y2)
{
    v.insert(v.end(), { x0, y0, x1, y1, x2, y2 });
}

void King::addCircle(std::vector<float>& v,
    float cx, float cy, float radius, int segments)
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

// -----------------------------------------------------------------------
// King silhouette geometry
//
// Coordinates are in NDC local space, centered at the origin, sized to
// fit inside one board square (0.25 x 0.25 NDC).
//
// Layers (bottom → top):
//   1. Wide foot bar            y: -0.110 → -0.075
//   2. Tapered body (trapezoid) y: -0.075 → -0.005
//   3. Shoulder band            y: -0.005 →  0.020
//   4. Neck                     y:  0.020 →  0.040
//   5. Crown base ring          y:  0.040 →  0.058
//   6. Cross — vertical bar     y:  0.058 →  0.110
//   6. Cross — horizontal bar   y:  0.068 →  0.085  (wider, centred)
//
// The cross is the classic king distinguisher.
// -----------------------------------------------------------------------
std::vector<float> King::buildVertices()
{
    std::vector<float> v;

    // 1. Wide foot bar
    addRect(v, -0.075f, -0.110f, 0.075f, -0.075f);

    // 2. Tapered body — wider at the base, narrower at the shoulder
    addTrapezoid(v,
        -0.055f, 0.055f, -0.075f,   // bottom edge
        -0.025f, 0.025f, -0.005f);  // top edge

    // 3. Shoulder band
    addRect(v, -0.050f, -0.005f, 0.050f, 0.020f);

    // 4. Neck — slight taper upward
    addTrapezoid(v,
        -0.025f, 0.025f, 0.020f,   // bottom
        -0.018f, 0.018f, 0.042f);  // top

    // 5. Crown base ring (wider than the neck, sits just below the cross)
    addRect(v, -0.038f, 0.042f, 0.038f, 0.060f);

    // 6. Cross — vertical bar
    addRect(v, -0.013f, 0.060f, 0.013f, 0.112f);

    // 6. Cross — horizontal bar (overlaps the vertical so the join is solid)
    addRect(v, -0.038f, 0.073f, 0.038f, 0.094f);

    return v;
}