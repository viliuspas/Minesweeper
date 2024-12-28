#define ID_ICON 9003
#define ID_BUTTON_ICON 9004
#define ID_FLAG_ICON 9005
#define ID_MINE_ICON 9006
#define ID_MINE_FAILED_ICON 9007
#define ID_SMILEY_HAPPY_ICON 9008
#define ID_SMILEY_COOL_ICON 9009
#define ID_SMILEY_SHOCKED_ICON 9010
#define ID_SMILEY_SAD_ICON 9011

#define IDR_MYMENU 1001
#define ID_GAME_BEGINNER 10001
#define ID_GAME_INTERMEDIATE 10002
#define ID_GAME_EXPERT 10003
#define ID_GAME_NEW 9000
#define ID_GAME_EXIT 9001
#define ID_HELP 9002
#define ID_SCORES_BEGINNER 10004
#define ID_SCORES_INTERMEDIATE 10005
#define ID_SCORES_EXPERT 10006

#define SCORE_PATH "scores.txt"
#define GRID_SIZE 10
#define GRID_SIZE_BEGINNER 10
#define GRID_SIZE_INTERNEDIATE 15
#define GRID_SIZE_EXPERT 20
#define CELL_SIZE 30
#define NUM_MINES 10
#define NUM_MINES_BEGINNER 10
#define NUM_MINES_INTERMEDIATE 40
#define NUM_MINES_EXPERT 99
#define TOOLBAR_HEIGHT 50
#define BORDER_WIDTH 20
#define WINDOW_HEIGHT GRID_SIZE + TOOLBAR_HEIGHT
#define NAV_BAR_HEIGHT 20

#define BLUE RGB(0, 1, 253)
#define GREEN RGB(1, 126, 0)
#define RED RGB(254, 0, 1)
#define DARK_BLUE RGB(1, 1, 128)
#define MAROON RGB(129, 1, 1)
#define TEAL RGB(0, 128, 128)
#define BLACK RGB(0, 0, 0)
#define GRAY RGB(128, 128, 128)


#define TUTORIAL_TEXT \
    "To start the game, left-click on any cell on the board. This will clear that cell and reveal what’s underneath.\
 If the cell contains a number, that number indicates how many mines are in the eight adjacent cells (above, below, left, right, and diagonally).\
 For example, if a cell shows the number \"2,\" it means two of its neighboring cells contain mines.\n\n\
If you suspect a cell contains a mine, you can right-click on it to mark it with a flag. Use the numbers revealed on cleared cells to deduce where the mines are located.\
 For instance, if a cell shows a \"1\" and one of its neighbors is flagged, you know the other adjacent cells are safe to click.\n\n\
The total number of flags you can place is displayed in the top left corner, while a timer in the top right tracks how long you’ve been playing.\
 Your goal is to clear all the cells that are not mines while avoiding the mined cells. Be careful: clicking on a mine ends the game.\n\n\
You win when all non-mine cells are cleared. To add more variety and challenge, use the navigation menu to select different difficulty levels.\
 Try to achieve the fastest time possible by clearing the board efficiently!"
