#pragma once
#include <glew.h>
#include <vector>
#include <utility>

class ChessBoard
{
public:
    ChessBoard();
    ~ChessBoard();

    void init();
    void draw(int queenCol, int queenRow, bool queenSelected,
              const std::vector<std::pair<int, int>>& validMoves);

private:
    unsigned int m_shaderProgram;
    unsigned int m_VAO;
    unsigned int m_VBO;

    unsigned int compileShader(unsigned int type, const char* src);
    bool isInList(int col, int row, const std::vector<std::pair<int, int>>& list);
};
