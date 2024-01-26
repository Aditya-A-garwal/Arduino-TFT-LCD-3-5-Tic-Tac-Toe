#include <Arduino.h>

#include "Adafruit_GFX.h"
#include "MCUFRIEND_kbv.h"
#include "TouchScreen.h"

#include "constants.h"

MCUFRIEND_kbv tft;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

int16_t player = NONE;
int16_t turn = CROSS;
uint16_t moves = 0;

int16_t grid[3][3] = {
    {NONE, NONE, NONE},
    {NONE, NONE, NONE},
    {NONE, NONE, NONE},
};

// ----- begin helper math functions -----

bool inRange(uint16_t value, uint16_t lo, uint16_t hi) {
    return lo <= value && value <= hi;
}

// ----- begin functions to handle touch input -----

void toDisplayMode() {

    pinMode(XM, OUTPUT);
    pinMode(XP, OUTPUT);
    pinMode(YM, OUTPUT);
    pinMode(YP, OUTPUT);
}

void convertTouchCoors(uint16_t tx, uint16_t ty, uint16_t *xptr, uint16_t *yptr) {

    tx = constrain(tx, XBEGIN, XEND);
    ty = constrain(ty, YBEGIN, YEND);

    tx = map(tx, XBEGIN, XEND, 0, 319);
    ty = map(ty, YBEGIN, YEND, 479, 0);

    *xptr = tx;
    *yptr = ty;
}

void getTouchCoors(uint16_t *xptr, uint16_t *yptr) {

    TSPoint p;

    for (;;) {

        p = ts.getPoint();
        if (inRange(p.z, PRESSURE_LEFT, PRESSURE_RIGHT)) {
            break;
        }
    }
    convertTouchCoors(p.x, p.y, xptr, yptr);
    toDisplayMode();
}

bool getGridTouchCoors(uint16_t x, uint16_t y, uint16_t *grid_x, uint16_t *grid_y) {

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {

            if (inRange(x - GRID_X - i*CELL_W, 0, CELL_W)
                && inRange(y - GRID_Y - j*CELL_W, 0, CELL_W)) {

                if (grid[i][j] == NONE) {

                    *grid_x = i, *grid_y = j;
                    return true;
                }
                break;
            }
        }
    }

    return false;
}

// ----- begin functions to draw complex shapes -----

void drawThickHLine(uint16_t x, uint16_t y, uint16_t d, uint16_t clr) {

    const int LINE_WIDTH = 7;
    tft.fillRect(x, y - LINE_WIDTH/2, d, LINE_WIDTH, clr);
    tft.fillCircle(x, y, LINE_WIDTH/2, clr);
    tft.fillCircle(x + d, y, LINE_WIDTH/2, clr);
}

void drawThickVLine(uint16_t x, uint16_t y, uint16_t d, uint16_t clr) {

    const int LINE_WIDTH = 7;
    tft.fillRect(x - LINE_WIDTH/2, y, LINE_WIDTH, d, clr);
    tft.fillCircle(x, y, LINE_WIDTH/2, clr);
    tft.fillCircle(x, y + d, LINE_WIDTH/2, clr);
}

void drawThickDownDiagonal(uint16_t x, uint16_t y, uint16_t d, uint16_t clr) {

    const int LINE_WIDTH = 5;
    for (uint16_t i = 0; i < LINE_WIDTH/2; ++i) {
        tft.drawLine(x + i, y - i, x + d + i, y + d - i, clr);
    }
    for (uint16_t i = 0; i < LINE_WIDTH/2; ++i) {
        tft.drawLine(x - i, y + i, x + d - i, y + d + i, clr);
    }

    tft.fillCircle(x, y, LINE_WIDTH/2, clr);
    tft.fillCircle(x + d, y + d, LINE_WIDTH/2, clr);
}

void drawThickUpDiagonal(uint16_t x, uint16_t y, uint16_t d, uint16_t clr) {

    const int LINE_WIDTH = 5;
    for (uint16_t i = 0; i < LINE_WIDTH/2; ++i) {
        tft.drawLine(x - i, y - i, x + d - i, y - d - i, clr);
    }
    for (uint16_t i = 0; i < LINE_WIDTH/2; ++i) {
        tft.drawLine(x + i, y + i, x + d + i, y - d + i, clr);
    }

    tft.fillCircle(x, y, LINE_WIDTH/2, clr);
    tft.fillCircle(x + d, y - d, LINE_WIDTH/2, clr);
}

void drawNaught(uint16_t x, uint16_t y, uint16_t clr) {

    tft.fillCircle(x, y, MARK_W/2, clr);
    tft.fillCircle(x, y, (MARK_W - MARK_THICKNESS)/2, BLACK);
}

void drawCross(uint16_t x, uint16_t y, uint16_t clr) {

    tft.fillRoundRect(x - MARK_W/2, y - MARK_W/2, MARK_W, MARK_W, MARK_THICKNESS/2, clr);

    // left-side triangle
    tft.fillTriangle(
        x - MARK_W/2, y - MARK_W/2 + MARK_THICKNESS/2,
        x - MARK_W/2, y + MARK_W/2 - MARK_THICKNESS/2,
        x - MARK_THICKNESS/2, y,
        BLACK
    );

    // right-side triangle
    tft.fillTriangle(
        x + MARK_W/2, y - MARK_W/2 + MARK_THICKNESS/2,
        x + MARK_W/2, y + MARK_W/2 - MARK_THICKNESS/2,
        x + MARK_THICKNESS/2, y,
        BLACK
    );

    // top-side triangle
    tft.fillTriangle(
        x - MARK_W/2 + MARK_THICKNESS/2, y - MARK_W/2,
        x + MARK_W/2 - MARK_THICKNESS/2, y - MARK_W/2,
        x, y - MARK_THICKNESS/2,
        BLACK
    );

    // bottom-side triangle
    tft.fillTriangle(
        x - MARK_W/2 + MARK_THICKNESS/2, y + MARK_W/2,
        x + MARK_W/2 - MARK_THICKNESS/2, y + MARK_W/2,
        x, y + MARK_THICKNESS/2,
        BLACK
    );
}

// ----- begin functions to draw widgets -----

void drawMenuItem(uint16_t offset, uint16_t clr, const char msg[]) {

    tft.drawRect(ITEM_X, MENU_Y + offset, ITEM_W, ITEM_H, clr);

    tft.setTextSize(3);
    tft.setTextColor(clr, BLACK);
    tft.setCursor(ITEM_X, MENU_Y + offset);
    tft.println(msg);
}

void drawMenu() {

    // draw the boundary and show the title
    tft.drawRect(MENU_X, MENU_Y, MENU_W, MENU_HEIGHT, YELLOW);
    tft.setTextSize(3);
    tft.setTextColor(YELLOW);
    tft.setCursor(60, 20);
    tft.print("TIC-TAC-TOE");

    // show the starting options
    drawMenuItem(ITEM_H*1, RED, "PLAY AS X");
    drawMenuItem(ITEM_H*3, BLUE, "PLAY AS O");
}

void drawGrid() {

    drawThickHLine(GRID_X, GRID_Y + CELL_W, GRID_W, WHITE);
    drawThickHLine(GRID_X, GRID_Y + 2*CELL_W, GRID_W, WHITE);

    drawThickVLine(GRID_X + CELL_W, GRID_Y, GRID_W, WHITE);
    drawThickVLine(GRID_X + 2*CELL_W, GRID_Y, GRID_W, WHITE);
}

void drawWinner(int16_t winner) {

    tft.setTextSize(3);
    tft.setTextColor((winner == CROSS) ? RED : BLUE);

    tft.setCursor(WINNER_X, WINNER_Y);
    tft.print((winner == player) ? "PLAYER " : "ARDUINO ");
    tft.print((winner == CROSS)  ? "(X) " : "(O) ");
    tft.print("WINS");

    tft.setCursor(80, 40);
    tft.print("GAME OVER");
}

void drawTie() {

    tft.setTextSize(3);
    tft.setTextColor(GRAY);

    tft.setCursor(TIE_X, TIE_Y);
    tft.print("TIE");

    tft.setCursor(80, 40);
    tft.print("GAME OVER");
}

// ----- begin functions to update widgets -----

void updateMenu() {

    uint16_t x, y;

    for (;;) {

        // wait till a valid touch is detected
        getTouchCoors(&x, &y);

        // Touch not within menu
        if (!inRange(x, ITEM_X, ITEM_X + ITEM_W)) {
            continue;
        }

        // player has chosen cross
        if (inRange(y, MENU_Y + ITEM_H*1, MENU_Y + ITEM_H*2)) {
            player = CROSS;
            break;
        }

        // player has chosen naught
        if (inRange(y, MENU_Y + ITEM_H*3, MENU_Y + ITEM_H*4)) {
            player = NAUGHT;
            break;
        }
    }
}


uint16_t getWinner() {

    // check for vertical groups
    for (uint16_t i = 0; i < 3; ++i) {

        if (grid[i][0] != NONE && grid[i][0] == grid[i][1] && grid[i][1] == grid[i][2]) {

            drawThickVLine(GRID_X + CELL_W/2 + CELL_W * i, GRID_Y + CELL_W/2, CELL_W * 2, GREEN);
            return grid[i][0];
        }
    }

    // check for horizontal groups
    for (uint16_t i = 0; i < 3; ++i) {

        if (grid[0][i] != NONE && grid[0][i] == grid[1][i] && grid[1][i] == grid[2][i]) {

            drawThickHLine(GRID_X + CELL_W/2, GRID_Y  + CELL_W/2 + CELL_W * i, CELL_W * 2, GREEN);
            return grid[0][i];
        }
    }

    // check diagonals
    if (grid[0][0] != NONE && grid[0][0] == grid[1][1] && grid[1][1] == grid[2][2]) {

        drawThickDownDiagonal(GRID_X + CELL_W/2, GRID_Y + CELL_W/2, CELL_W * 2, GREEN);
        return grid[0][0];
    }

    if (grid[2][0] != NONE && grid[2][0] == grid[1][1] && grid[1][1] == grid[0][2]) {

        drawThickUpDiagonal(GRID_X + CELL_W/2, GRID_Y + CELL_W/2 + CELL_W * 2, CELL_W * 2, GREEN);
        return grid[2][0];
    }

    return NONE;
}

bool getTie() {

    return ++moves == 9;
}

void setup() {

    // Use Serial Monitor for debugging
    Serial.begin(115200);
    tft.begin(0x9486);

    // Fill the screen black for a fresh start
    tft.fillScreen(BLACK);

    // draw the menu and wait for a selection
    drawMenu();
    updateMenu();

    // fill the screen black to clear the menu
    tft.fillScreen(BLACK);

    delay(300); // eases transition from menu

    // draw the grid
    drawGrid();
}

void loop() {

    uint16_t grid_x, grid_y;
    uint16_t x, y;
    int16_t winner;

    if (turn != player) { // the "computer" should play a move

        delay(1000); // simulate the act of "thinking"

        for (;;) {

            grid_x = random(0, 3), grid_y = random(0, 3);

            if (grid[grid_x][grid_y] == NONE) {
                break;
            }
        }
    }

    else if (turn == player) { // the player should play a move

        for (;;) {

            getTouchCoors(&x, &y);

            if (getGridTouchCoors(x, y, &grid_x, &grid_y)) {
                break;
            }
        }
    }

    // update the grid and display the move
    grid[grid_x][grid_y] = turn;
    (turn == CROSS)
    ? drawCross(GRID_X + CELL_W/2 + grid_x*CELL_W, GRID_Y + CELL_W/2 + grid_y*CELL_W, RED)
    : drawNaught(GRID_X + CELL_W/2 + grid_x*CELL_W, GRID_Y + CELL_W/2 + grid_y*CELL_W, BLUE);

    if ((winner = getWinner()) != NONE) {
        drawWinner(winner);
        for (;;);
    }

    if (getTie()) {
        drawTie();
        for (;;);
    }

    // toggle the turn
    turn = NAUGHT + CROSS - turn;
}
