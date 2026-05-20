#include <glew.h>
#include <glfw3.h>
#include <vector>
#include <utility>
#include "Objects/Scene/chessBoard.h"
#include "Objects/Pieces/queen.h"
#include "Objects/Pieces/pawn.h"
#include "Objects/Pieces/rook.h"
#include "Objects/Pieces/bishop.h"
#include "Objects/Pieces/horse.h"

const int WINDOW_SIZE = 640;
const int SQUARE_PX   = WINDOW_SIZE / 8;

// --- game state ---
static int  g_whiteCol   = 3, g_whiteRow = 0;   // white queen starts at d1
static int  g_blackCol   = 3, g_blackRow = 7;   // black queen starts at d8

// Rooks
static int  g_whiteRook1Col = 0, g_whiteRook1Row = 0;   // white rook left
static int  g_whiteRook2Col = 7, g_whiteRook2Row = 0;   // white rook right
static int  g_blackRook1Col = 0, g_blackRook1Row = 7;   // black rook left
static int  g_blackRook2Col = 7, g_blackRook2Row = 7;   // black rook right

// Pawn
static int  g_whitePawnCol = 6, g_whitePawnRow = 1;   // white pawn
static int  g_blackPawnCol = 4, g_blackPawnRow = 6;   // black pawn

// Knight
static int  g_whiteHorseCol = 1, g_whiteHorseRow = 0;   // white knight at b1
static int  g_blackHorseCol = 1, g_blackHorseRow = 7;   // black knight at b8

static int  g_whiteHorse2Col = 6, g_whiteHorse2Row = 0;   // white knight at b1
static int  g_blackHorse2Col = 6, g_blackHorse2Row = 7;   // black knight at b8

// Bishop
static int g_whiteBishopCol = 2, g_whiteBishopRow = 0;
static bool g_whiteBishopAlive = true;

static int g_blackBishopCol = 2, g_blackBishopRow = 7;
static bool g_blackBishopAlive = true;

static int g_whiteBishop2Col = 5, g_whiteBishop2Row = 0;
static bool g_whiteBishop2Alive = true;

static int g_blackBishop2Col = 5, g_blackBishop2Row = 7;
static bool g_blackBishop2Alive = true;



static bool g_whiteAlive = true;
static bool g_blackAlive = true;
static bool g_whiteTurn  = true;                 // white goes first
static bool g_gameOver   = false;

static bool g_queenSelected = false;
static std::vector<std::pair<int, int>> g_validMoves;

// --- chess logic ---

// Traces queen rays in all 8 directions.
// The opponent queen blocks the ray: her square is included (capture) but
// nothing beyond it is reachable.
static std::vector<std::pair<int, int>> computeValidMoves(
    int col, int row,
    int oppCol, int oppRow, bool oppAlive)
{
    std::vector<std::pair<int, int>> moves;
    const int dirs[8][2] = { {1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1} };

    for (const auto& d : dirs)
    {
        int c = col + d[0];
        int r = row + d[1];
        while (c >= 0 && c < 8 && r >= 0 && r < 8)
        {
            moves.push_back({ c, r });
            if (oppAlive && c == oppCol && r == oppRow)
                break;  // opponent queen blocks; include for capture, then stop
            c += d[0];
            r += d[1];
        }
    }
    return moves;
}

static bool isValidMove(int col, int row)
{
    for (const auto& m : g_validMoves)
        if (m.first == col && m.second == row) return true;
    return false;
}

// --- mouse callback ---

static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (g_gameOver || button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) return;

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);

    int col = static_cast<int>(mx / SQUARE_PX);
    int row = 7 - static_cast<int>(my / SQUARE_PX);
    if (col < 0 || col > 7 || row < 0 || row > 7) return;

    // References to whichever queen is active this turn
    int& aCol   = g_whiteTurn ? g_whiteCol   : g_blackCol;
    int& aRow   = g_whiteTurn ? g_whiteRow   : g_blackRow;
    bool& oAlive = g_whiteTurn ? g_blackAlive : g_whiteAlive;
    int   oCol  = g_whiteTurn ? g_blackCol   : g_whiteCol;
    int   oRow  = g_whiteTurn ? g_blackRow   : g_whiteRow;

    if (!g_queenSelected)
    {
        if (col == aCol && row == aRow)
        {
            g_queenSelected = true;
            g_validMoves    = computeValidMoves(aCol, aRow, oCol, oRow, oAlive);
        }
    }
    else
    {
        if (col == aCol && row == aRow)
        {
            // Click the queen again → deselect
            g_queenSelected = false;
            g_validMoves.clear();
        }
        else if (isValidMove(col, row))
        {
            aCol = col;
            aRow = row;

            // Capture: active queen landed on opponent's square
            if (oAlive && col == oCol && row == oRow)
            {
                oAlive     = false;
                g_gameOver = true;
            }

            g_queenSelected = false;
            g_validMoves.clear();

            if (!g_gameOver)
                g_whiteTurn = !g_whiteTurn;
        }
        else
        {
            // Click elsewhere → deselect without moving
            g_queenSelected = false;
            g_validMoves.clear();
        }
    }
}

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
    glfwSetMouseButtonCallback(window, mouseCallback);

    if (glewInit() != GLEW_OK) return -1;
    glViewport(0, 0, WINDOW_SIZE, WINDOW_SIZE);

    ChessBoard board;
    board.init();

    Queen whiteQueen(0.95f, 0.90f, 0.75f);  // ivory
    Queen blackQueen(0.10f, 0.08f, 0.05f);  // dark ebony
    whiteQueen.init();
    blackQueen.init();

    // Rooks
    Rook whiteRook1(0.95f, 0.90f, 0.75f);
    Rook blackRook1(0.10f, 0.08f, 0.05f);  
    Rook whiteRook2(0.95f, 0.90f, 0.75f); 
    Rook blackRook2(0.10f, 0.08f, 0.05f); 
    whiteRook1.init();
    blackRook1.init();
    whiteRook2.init();
    blackRook2.init();

    // Pawns
    Pawn whitePawn(0.95f, 0.90f, 0.75f);
    Pawn blackPawn(0.10f, 0.08f, 0.05f);
    whitePawn.init();
    blackPawn.init();

    // newly added horses
    Horse whiteHorse(0.95f, 0.90f, 0.75f);
    Horse blackHorse(0.10f, 0.08f, 0.05f);
    whiteHorse.init();
    blackHorse.init();

    // newly added bishop
    Bishop whiteBishop(0.95f, 0.90f, 0.75f);
    Bishop blackBishop(0.10f, 0.08f, 0.05f);
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
                ? "Chess Queens  |  White's turn — click queen to select"
                : "Chess Queens  |  Black's turn — click queen to select");

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Highlight the active queen's square (gold) and valid destinations (green)
        int hlCol = g_whiteTurn ? g_whiteCol : g_blackCol;
        int hlRow = g_whiteTurn ? g_whiteRow : g_blackRow;
        board.draw(hlCol, hlRow, g_queenSelected, g_validMoves);

        if (g_whiteAlive)
        {
            float wx = -1.0f + (g_whiteCol + 0.5f) * 0.25f;
            float wy = -1.0f + (g_whiteRow + 0.5f) * 0.25f;
            whiteQueen.draw(wx, wy);

            //white rook 1
            float wr1x = -1.0f + (g_whiteRook1Col + 0.5f) * 0.25f;
            float wr1y = -1.0f + (g_whiteRook1Row + 0.5f) * 0.25f;
            whiteRook1.draw(wr1x, wr1y);

            //white rook 2
            float wr2x = -1.0f + (g_whiteRook2Col + 0.5f) * 0.25f;
            float wr2y = -1.0f + (g_whiteRook2Row + 0.5f) * 0.25f;
            whiteRook2.draw(wr2x, wr2y);

            //white pawn
            float wpx = -1.0f + (g_whitePawnCol + 0.5f) * 0.25f;
            float wpy = -1.0f + (g_whitePawnRow + 0.5f) * 0.25f;
            whitePawn.draw(wpx, wpy);

            // newly added White horse
            float hx = -1.0f + (g_whiteHorseCol + 0.5f) * 0.25f;
            float hy = -1.0f + (g_whiteHorseRow + 0.5f) * 0.25f;
            whiteHorse.draw(hx, hy);

            // newly added White horse
            float hx2 = -1.0f + (g_whiteHorse2Col + 0.5f) * 0.25f;
            float hy2 = -1.0f + (g_whiteHorse2Row + 0.5f) * 0.25f;
            whiteHorse.draw(hx2, hy2);
        }

        if (g_blackAlive)
        {
            float bx = -1.0f + (g_blackCol + 0.5f) * 0.25f;
            float by = -1.0f + (g_blackRow + 0.5f) * 0.25f;
            blackQueen.draw(bx, by);

            //black rook 1
            float br1x = -1.0f + (g_blackRook1Col + 0.5f) * 0.25f;
            float br1y = -1.0f + (g_blackRook1Row + 0.5f) * 0.25f;
            blackRook1.draw(br1x, br1y);

            //black rook 2
            float br2x = -1.0f + (g_blackRook2Col + 0.5f) * 0.25f;
            float br2y = -1.0f + (g_blackRook2Row + 0.5f) * 0.25f;
            blackRook2.draw(br2x, br2y);

            //white pawn
            float bpx = -1.0f + (g_blackPawnCol + 0.5f) * 0.25f;
            float bpy = -1.0f + (g_blackPawnRow + 0.5f) * 0.25f;
            blackPawn.draw(bpx, bpy);

            // newly added Black horse
            float hx = -1.0f + (g_blackHorseCol + 0.5f) * 0.25f;
            float hy = -1.0f + (g_blackHorseRow + 0.5f) * 0.25f;
            blackHorse.draw(hx, hy);

            float hx2 = -1.0f + (g_blackHorse2Col + 0.5f) * 0.25f;
            float hy2 = -1.0f + (g_blackHorse2Row + 0.5f) * 0.25f;
            blackHorse.draw(hx2, hy2);
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
