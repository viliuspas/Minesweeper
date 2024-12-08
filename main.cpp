#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <ctime>
#include "minesweeper.h"

using namespace std;

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
void PaintComponents(HWND, HINSTANCE);
void PaintGrid(HDC);
void DrawColorText(HDC, RECT, char*, COLORREF);
void PaintValues(HDC, HINSTANCE);
void InitializeGrid(int, int);
void RevealCells(int, int);
bool areNeighbors(int, int, int , int);
void PrintGrid();

TCHAR szClassName[ ] = _T("Minesweeper");

struct Cell {
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    int adjacentMines;
};

vector<vector<Cell> > grid(GRID_SIZE, vector<Cell>(GRID_SIZE));
HWND gridButtons[GRID_SIZE][GRID_SIZE];
HWND mineButton;
bool isGridInitialized = false;

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (hThisInstance, MAKEINTRESOURCE(ID_ICON));
    wincl.hIconSm = LoadIcon (hThisInstance, MAKEINTRESOURCE(ID_ICON));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);

    if (!RegisterClassEx (&wincl))
        return 0;

   hwnd = CreateWindowEx(
        0,
        szClassName,
        _T("Minesweeper"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        GRID_SIZE * CELL_SIZE + 16 + BORDER_WIDTH * 2,
        GRID_SIZE * CELL_SIZE + 36 + TOOLBAR_HEIGHT + BORDER_WIDTH,
        HWND_DESKTOP,
        NULL,
        hThisInstance,
        NULL);

    ShowWindow (hwnd, nCmdShow);

    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HINSTANCE hInstance;

    switch (message)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_CREATE:
        {
            hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
            HICON hButtonIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ID_BUTTON_ICON));
            HICON hFlagIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ID_FLAG_ICON));

            for (int x = 0; x < GRID_SIZE; ++x)
            {
                for (int y = 0; y < GRID_SIZE; ++y)
                {
                    gridButtons[x][y] = CreateWindow(
                        _T("BUTTON"),
                        _T(""),
                        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_ICON,
                        x * CELL_SIZE + BORDER_WIDTH + 1,
                        y * CELL_SIZE + TOOLBAR_HEIGHT,
                        CELL_SIZE,
                        CELL_SIZE,
                        hwnd,
                        (HMENU)(x * GRID_SIZE + y), // ButtonID
                        hInstance,
                        NULL);

                    SendMessage(gridButtons[x][y], BM_SETIMAGE, IMAGE_ICON, (LPARAM)hButtonIcon);
                }
            }

            mineButton = CreateWindow(
                        _T("BUTTON"),
                        _T(""),
                        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_ICON,
                        0,
                        0,
                        CELL_SIZE,
                        CELL_SIZE,
                        hwnd,
                        (HMENU)ID_MINE_ICON, // ButtonID
                        hInstance,
                        NULL);

            SendMessage(mineButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hFlagIcon);
            break;
        }

        case WM_COMMAND:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                int buttonID = LOWORD(wParam);

                if(buttonID == ID_MINE_ICON)
                {
                    cout<<"Mine button clicked"<<endl;
                    break;
                }

                int x = buttonID / GRID_SIZE;
                int y = buttonID % GRID_SIZE;

                if (!isGridInitialized)
                {
                    InitializeGrid(x, y);
                    isGridInitialized = true;
                }

                if (!grid[x][y].isFlagged)
                {
                    RevealCells(x, y);
                }
            }
            break;

        case WM_PAINT:
            PaintComponents(hwnd, hInstance);
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

void PaintComponents(HWND hwnd, HINSTANCE hInstance)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    PaintGrid(hdc);
    PaintValues(hdc, hInstance);

    EndPaint(hwnd, &ps);
}

void PaintGrid(HDC hdc)
{
    HPEN hPen = CreatePen(PS_SOLID, 1, GRAY);
    SelectObject(hdc, hPen);

    for (int i = 0; i <= GRID_SIZE; ++i)
    {
        // Vertical lines
        double verticalFromX = i * CELL_SIZE + BORDER_WIDTH;
        double verticalFromY = TOOLBAR_HEIGHT;
        double verticalToX = i * CELL_SIZE  + BORDER_WIDTH;
        double verticalToY = GRID_SIZE * CELL_SIZE + TOOLBAR_HEIGHT;

        MoveToEx(hdc, verticalFromX, verticalFromY, NULL);
        LineTo(hdc, verticalToX, verticalToY);

        // Horizontal lines
        double horizontalFromX = BORDER_WIDTH;
        double horizontalFromY = i * CELL_SIZE + TOOLBAR_HEIGHT;
        double horizontalToX = GRID_SIZE * CELL_SIZE + BORDER_WIDTH;
        double horizontalToY = i * CELL_SIZE + TOOLBAR_HEIGHT;

        MoveToEx(hdc, horizontalFromX, horizontalFromY, NULL);
        LineTo(hdc, horizontalToX, horizontalToY);
    }

    DeleteObject(hPen);
}

void PaintValues(HDC hdc, HINSTANCE hInstance)
{
    RECT rect;
    HFONT hfont = CreateFont(CELL_SIZE, 0, 0, 0, 0, 0, 0, 0, OEM_CHARSET, 0, 0, 0, 0, TEXT("Terminal"));

    HGDIOBJ oldFont = SelectObject(hdc, hfont);
    SetBkMode(hdc, TRANSPARENT);

    for (int x = 0; x < GRID_SIZE; ++x)
    {
        for (int y = 0; y < GRID_SIZE; ++y)
        {
            SetRect(&rect,
                x * CELL_SIZE + BORDER_WIDTH,
                y * CELL_SIZE + TOOLBAR_HEIGHT,
                (x + 1) * CELL_SIZE + BORDER_WIDTH,
                (y + 1) * CELL_SIZE + TOOLBAR_HEIGHT);

            if (grid[x][y].isMine)
            {
                HICON hMineIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ID_MINE_ICON));
                DrawIcon(hdc, rect.left, rect.top, hMineIcon);
                continue;
            }

            SetRect(&rect,
                x * CELL_SIZE + BORDER_WIDTH + 2,
                y * CELL_SIZE + TOOLBAR_HEIGHT + 1,
                (x + 1) * CELL_SIZE + BORDER_WIDTH + 2,
                (y + 1) * CELL_SIZE + TOOLBAR_HEIGHT + 1);

            switch (grid[x][y].adjacentMines)
            {
                case 1:
                    DrawColorText(hdc, rect, "1", BLUE);
                    break;
                case 2:
                    DrawColorText(hdc, rect, "2", GREEN);
                    break;
                case 3:
                    DrawColorText(hdc, rect, "3", RED);
                    break;
                case 4:
                    DrawColorText(hdc, rect, "4", DARK_BLUE);
                    break;
                case 5:
                    DrawColorText(hdc, rect, "5", MAROON);
                    break;
                case 6:
                    DrawColorText(hdc, rect, "6", TEAL);
                    break;
                case 7:
                    DrawColorText(hdc, rect, "7", BLACK);
                    break;
                case 8:
                    DrawColorText(hdc, rect, "8", GRAY);
                    break;
                case 9:
                    DrawColorText(hdc, rect, "9", BLACK);
                    break;
                default:
                    break;
            }
        }
    }

    SelectObject(hdc, oldFont);
    DeleteObject(hfont);
}

void DrawColorText(HDC hdc, RECT rect, char* text, COLORREF color)
{
    SetTextColor(hdc, color);
    DrawText(hdc, TEXT(text), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void InitializeGrid(int startX, int startY)
{
    srand((unsigned)time(NULL));
    int minesPlaced = 0;
    while (minesPlaced < NUM_MINES)
    {
        int x = rand() % GRID_SIZE;
        int y = rand() % GRID_SIZE;
        if (!grid[x][y].isMine && !areNeighbors(startX, startY, x, y) && x != startX && y != startY)
        {
            grid[x][y].isMine = true;
            ++minesPlaced;
        }
    }

    for (int x = 0; x < GRID_SIZE; ++x)
    {
        for (int y = 0; y < GRID_SIZE; ++y)
        {
            if (grid[x][y].isMine) continue;
            int mineCount = 0;
            for (int dx = -1; dx <= 1; ++dx)
            {
                for (int dy = -1; dy <= 1; ++dy)
                {
                    int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && ny >= 0 && nx < GRID_SIZE && ny < GRID_SIZE && grid[nx][ny].isMine)
                    {
                        ++mineCount;
                    }
                }
            }
            grid[x][y].adjacentMines = mineCount;
        }
    }
}

void RevealCells(int x, int y)
{
    if (x < 0 || y < 0 || x >= GRID_SIZE || y >= GRID_SIZE) return;
    if (grid[x][y].isRevealed || grid[x][y].isFlagged) return;

    grid[x][y].isRevealed = true;
    ShowWindow(gridButtons[x][y], SW_HIDE);

    if (grid[x][y].adjacentMines != 0 || grid[x][y].isMine) return;

    for (int nx = x - 1; nx <= x + 1; ++nx)
    {
        for (int ny = y - 1; ny <= y + 1; ++ny)
        {
            if (nx == x && ny == y) continue;
            RevealCells(nx, ny);
        }
    }
}

bool areNeighbors(int x1, int y1, int x2, int y2)
{
    int dx = abs(x1 - x2);
    int dy = abs(y1 - y2);

    return (dx <= 1 && dy <= 1 && (dx + dy > 0));
}

void PrintGrid()
{
    for(int y = 0; y < GRID_SIZE; ++y)
    {
        for(int x = 0; x < GRID_SIZE; ++x)
        {
            if (grid[x][y].isMine)
            {
                cout<<"M ";
                continue;
            }
            cout<<grid[x][y].adjacentMines<<" ";
        }
        cout<<endl;
    }
    cout<<endl;
}

