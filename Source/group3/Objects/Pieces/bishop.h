#pragma once
#include <glew.h>
#include <vector>

class Bishop {
public:
	Bishop(float r, float g, float b);
	~Bishop();

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
	void addTriangle(std::vector<float>& v,
		float x0, float y0,
		float x1, float y1,
		float x2, float y2);
	void addTrapezoid(std::vector<float>& v,
		float bx0, float bx1, float by,
		float tx0, float tx1, float ty);
	void addCircle(std::vector<float>& v,
		float cx, float cy,
		float radius, int segments);
};