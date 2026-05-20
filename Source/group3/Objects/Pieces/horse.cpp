// horse.cpp
#include "horse.h"
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

Horse::Horse(float r, float g, float b)
    : m_r(r), m_g(g), m_b(b)
    , m_shaderProgram(0), m_VAO(0), m_VBO(0), m_vertexCount(0)
{
}

Horse::~Horse()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteProgram(m_shaderProgram);
}

unsigned int Horse::compileShader(unsigned int type, const char* src)
{
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    return id;
}

void Horse::init()
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

void Horse::draw(float x, float y)
{
    glUseProgram(m_shaderProgram);
    glUniform3f(glGetUniformLocation(m_shaderProgram, "pieceColor"), m_r, m_g, m_b);
    glUniform2f(glGetUniformLocation(m_shaderProgram, "offset"), x, y);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    glBindVertexArray(0);
}

// --- geometry helpers (identical API to queen.cpp) ---

void Horse::addRect(std::vector<float>& v, float x0, float y0, float x1, float y1)
{
    v.insert(v.end(), {
        x0, y0,  x1, y0,  x1, y1,
        x0, y0,  x1, y1,  x0, y1
        });
}

void Horse::addTrapezoid(std::vector<float>& v,
    float bx0, float bx1, float by,
    float tx0, float tx1, float ty)
{
    v.insert(v.end(), {
        bx0, by,  bx1, by,  tx1, ty,
        bx0, by,  tx1, ty,  tx0, ty
        });
}

void Horse::addTriangle(std::vector<float>& v,
    float x0, float y0,
    float x1, float y1,
    float x2, float y2)
{
    v.insert(v.end(), { x0, y0, x1, y1, x2, y2 });
}

void Horse::addCircle(std::vector<float>& v, float cx, float cy, float radius, int segments)
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

// Fan-triangulate a convex (or reasonably convex) polygon from its centroid.
void Horse::addPolygon(std::vector<float>& v, const std::vector<std::pair<float, float>>& pts)
{
    if (pts.size() < 3) return;
    // compute centroid
    float cx = 0, cy = 0;
    for (const auto& p : pts) { cx += p.first; cy += p.second; }
    cx /= pts.size(); cy /= pts.size();

    for (size_t i = 0; i < pts.size(); i++)
    {
        size_t j = (i + 1) % pts.size();
        v.insert(v.end(), {
            cx,           cy,
            pts[i].first, pts[i].second,
            pts[j].first, pts[j].second
            });
    }
}

// --- knight silhouette geometry ---
// Coordinates in NDC local space, centred at origin, sized to match the queen
// (~0.15 wide x 0.225 tall).  The silhouette reads unmistakably as a chess
// knight: wide base → tapered pedestal → rectangular neck → head canted
// forward → protruding snout → ear spike on top.
//
// The piece faces RIGHT (the traditional "white knight" direction).
// All shapes are broken into the same primitives the queen uses so the two
// implementations look like they came from the same hand.
std::vector<float> Horse::buildVertices()
{
    std::vector<float> v;

    // ── 1. Wide base (matches queen base proportions) ──────────────────────
    addRect(v, -0.075f, -0.110f, 0.075f, -0.075f);

    // ── 2. Tapered pedestal (wider at bottom, narrower at top) ─────────────
    //    bottom edge: -0.050 .. +0.050   top edge: -0.030 .. +0.030
    addTrapezoid(v,
        -0.050f, 0.050f, -0.075f,   // bottom
        -0.030f, 0.030f, -0.040f);  // top

    // ── 3. Vertical neck – slim rectangle rising from pedestal ─────────────
    addRect(v, -0.022f, -0.040f, 0.022f, 0.020f);

    // ── 4. Head – large polygon canted forward (to the right) ──────────────
    // The head tilts right: its bottom-left is near the neck top, its
    // bottom-right juts forward, giving the characteristic "leaning" posture.
    //
    //  Bottom-left  (-0.030,  0.020)
    //  Bottom-right ( 0.065,  0.020)   ← snout floor
    //  Right-mid    ( 0.070,  0.048)   ← tip of snout
    //  Right-top    ( 0.045,  0.070)   ← forehead / snout top
    //  Top-mid      ( 0.010,  0.090)   ← poll (top of head)
    //  Top-left     (-0.040,  0.075)   ← back of head
    //  Left-mid     (-0.042,  0.040)   ← back of neck
    addPolygon(v, {
        {-0.030f,  0.020f},
        { 0.065f,  0.020f},
        { 0.070f,  0.048f},
        { 0.045f,  0.070f},
        { 0.010f,  0.090f},
        {-0.040f,  0.075f},
        {-0.042f,  0.040f}
        });

    // ── 5. Ear – a single upward-pointing triangle on top of the head ───────
    addTriangle(v,
        -0.018f, 0.085f,
        0.008f, 0.085f,
        -0.005f, 0.112f);   // ear tip

    // ── 6. Eye – small filled circle ────────────────────────────────────────
    addCircle(v, 0.035f, 0.058f, 0.010f, 16);

    // ── 7. Nostril notch – tiny filled triangle at snout tip ────────────────
    addTriangle(v,
        0.055f, 0.025f,
        0.072f, 0.030f,
        0.060f, 0.040f);

    return v;
}
