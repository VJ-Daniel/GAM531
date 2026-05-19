#pragma once
#include <glew.h>
#include <vector>

class Queen
{
public:
    Queen(float r, float g, float b);
    ~Queen();

    void init();
    void draw(float x, float y);

private:
    float m_r, m_g, m_b;
    unsigned int m_shaderProgram;
    unsigned int m_VAO;
    unsigned int m_VBO;
    int m_vertexCount;

    unsigned int compileShader(unsigned int type, const char* src);
    std::vector<float> buildVertices();
    void addRect(std::vector<float>& v, float x0, float y0, float x1, float y1);
    void addTrapezoid(std::vector<float>& v, float bx0, float bx1, float by, float tx0, float tx1, float ty);
    void addTriangle(std::vector<float>& v, float x0, float y0, float x1, float y1, float x2, float y2);
    void addCircle(std::vector<float>& v, float cx, float cy, float radius, int segments);
};
