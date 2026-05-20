#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <utility>
#include "../Source/Objects/Scene/chessBoard.h"
#include "../Source/Objects/Pieces/Queen.h"
#include "../Source/Objects/Pieces/bishop.h"

const int WINDOW_SIZE = 640;
const int SQUARE_PX = WINDOW_SIZE / 8;

// --- game state ---
static int  g_whiteCol = 3, g_whiteRow = 0;   // white Queen starts at d1
static int  g_blackCol = 3, g_blackRow = 7;   // black Queen starts at d8
static bool g_whiteAlive = true;
static bool g_blackAlive = true;
static bool g_whiteTurn = true;                 // white goes first
static bool g_gameOver = false;

static bool g_QueenSelected = false;
static std::vector<std::pair<int, int>> g_validMoves;

// Bishop

static int g_whiteBishopCol = 2, g_whiteBishopRow = 0;
static bool g_whiteBishopAlive = true;

static int g_blackBishopCol = 2, g_blackBishopRow = 7;
static bool g_blackBishopAlive = true;

static int g_whiteBishop2Col = 5, g_whiteBishop2Row = 0;
static bool g_whiteBishop2Alive = true;

static int g_blackBishop2Col = 5 , g_blackBishop2Row = 7;
static bool g_blackBishop2Alive = true;

// --- entry point ---

int main()
{
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(
        WINDOW_SIZE, WINDOW_SIZE,
        "Chess Queens  |  White's turn",
        nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) return -1;
    glViewport(0, 0, WINDOW_SIZE, WINDOW_SIZE);

    ChessBoard board;
    board.init();

    Queen whiteQueen(0.95f, 0.90f, 0.75f);  // ivory
    Queen blackQueen(0.10f, 0.08f, 0.05f);  // dark ebon

    Bishop whiteBishop(0.95f, 0.90f, 0.75f);
    Bishop blackBishop(0.10f, 0.08f, 0.05f);
    whiteQueen.init();
    blackQueen.init();

    whiteBishop.init();
    blackBishop.init();

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Reflect game state in the title bar
        if (g_gameOver)
            glfwSetWindowTitle(window, g_whiteAlive ? "White wins!" : "Black wins!");
        else
            glfwSetWindowTitle(window, g_whiteTurn
                ? "Chess Queens  |  White's turn — click Queen to select"
                : "Chess Queens  |  Black's turn — click Queen to select");

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Highlight the active Queen's square (gold) and valid destinations (green)
        int hlCol = g_whiteTurn ? g_whiteCol : g_blackCol;
        int hlRow = g_whiteTurn ? g_whiteRow : g_blackRow;
        board.draw(hlCol, hlRow, g_QueenSelected, g_validMoves);

        if (g_whiteAlive)
        {
            float wx = -1.0f + (g_whiteCol + 0.5f) * 0.25f;
            float wy = -1.0f + (g_whiteRow + 0.5f) * 0.25f;
            whiteQueen.draw(wx, wy);
        }

        if (g_blackAlive)
        {
            float bx = -1.0f + (g_blackCol + 0.5f) * 0.25f;
            float by = -1.0f + (g_blackRow + 0.5f) * 0.25f;
            blackQueen.draw(bx, by);
        }

        if (g_whiteBishopAlive) {
            float bx = -1.0f + (g_whiteBishopCol + 0.5f) * 0.25f;
            float by = -1.0f + (g_whiteBishopRow + 0.5f) * 0.25f;

            whiteBishop.draw(bx, by);
        }

        if (g_blackBishopAlive) {
            float bx = -1.0f + (g_blackBishopCol + 0.5f) * 0.25f;
            float by = -1.0f + (g_blackBishopRow + 0.5f) * 0.25f;

            blackBishop.draw(bx, by);
        }

        if (g_whiteBishop2Alive) {
            float bx = -1.0f + (g_whiteBishop2Col + 0.5f) * 0.25f;
            float by = -1.0f + (g_whiteBishop2Row + 0.5f) * 0.25f;

            whiteBishop.draw(bx, by);
        }

        if (g_blackBishop2Alive) {
            float bx = -1.0f + (g_blackBishop2Col + 0.5f) * 0.25f;
            float by = -1.0f + (g_blackBishop2Row + 0.5f) * 0.25f;

            blackBishop.draw(bx, by);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
