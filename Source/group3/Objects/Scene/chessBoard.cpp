// chessBoard.cpp
#include "chessBoard.h"

static const char* BOARD_VERT = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
uniform vec2 offset;
void main()
{
    gl_Position = vec4(aPos + offset, 0.0, 1.0);
}
)";

static const char* BOARD_FRAG = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 squareColor;
void main()
{
    FragColor = vec4(squareColor, 1.0);
}
)";

ChessBoard::ChessBoard()
    : m_shaderProgram(0), m_VAO(0), m_VBO(0)
{}

ChessBoard::~ChessBoard()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteProgram(m_shaderProgram);
}

unsigned int ChessBoard::compileShader(unsigned int type, const char* src)
{
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    return id;
}

void ChessBoard::init()
{
    unsigned int vs = compileShader(GL_VERTEX_SHADER,   BOARD_VERT);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, BOARD_FRAG);
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vs);
    glAttachShader(m_shaderProgram, fs);
    glLinkProgram(m_shaderProgram);
    glDeleteShader(vs);
    glDeleteShader(fs);

    // One reusable square: 0.25 x 0.25, bottom-left at origin.
    // Offset uniform repositions it each draw call.
    float verts[] = {
        0.0f,  0.0f,
        0.25f, 0.0f,
        0.25f, 0.25f,
        0.0f,  0.0f,
        0.25f, 0.25f,
        0.0f,  0.25f
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool ChessBoard::isInList(int col, int row, const std::vector<std::pair<int, int>>& list)
{
    for (const auto& p : list)
        if (p.first == col && p.second == row) return true;
    return false;
}

void ChessBoard::draw(int queenCol, int queenRow, bool queenSelected,
                      const std::vector<std::pair<int, int>>& validMoves)
{
    glUseProgram(m_shaderProgram);
    int colorLoc  = glGetUniformLocation(m_shaderProgram, "squareColor");
    int offsetLoc = glGetUniformLocation(m_shaderProgram, "offset");

    glBindVertexArray(m_VAO);

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            bool isLight     = (col + row) % 2 == 0;
            bool isQueenHere = (col == queenCol && row == queenRow);
            bool isValidDest = isInList(col, row, validMoves);

            float r, g, b;
            if (queenSelected && isQueenHere)
            {
                // Queen's current square highlighted in gold
                r = 0.90f; g = 0.78f; b = 0.15f;
            }
            else if (queenSelected && isValidDest)
            {
                // Reachable squares highlighted in green
                r = 0.22f; g = 0.62f; b = 0.22f;
            }
            else if (isLight)
            {
                r = 0.93f; g = 0.87f; b = 0.73f;  // cream
            }
            else
            {
                r = 0.46f; g = 0.30f; b = 0.16f;  // brown
            }

            // Square (col, row) bottom-left corner in NDC
            float offX = -1.0f + col * 0.25f;
            float offY = -1.0f + row * 0.25f;

            glUniform3f(colorLoc,  r, g, b);
            glUniform2f(offsetLoc, offX, offY);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    glBindVertexArray(0);
}
