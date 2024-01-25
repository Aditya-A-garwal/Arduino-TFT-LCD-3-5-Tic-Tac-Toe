// display constants

const uint16_t RED     = 0xF800;
const uint16_t GREEN   = 0x07E0;
const uint16_t BLUE    = 0x001F;
const uint16_t CYAN    = 0x07FF;
const uint16_t MAGENTA = 0xF81F;
const uint16_t YELLOW  = 0xFFE0;
const uint16_t WHITE   = 0xFFFF;
const uint16_t GRAY    = 0x520A;
const uint16_t BLACK   = 0x0000;

const uint16_t DISPLAY_WIDTH   = 320;
const uint16_t DISPLAY_HEIGHT  = 480;

// touch constants

const uint16_t PRESSURE_LEFT = 10;
const uint16_t PRESSURE_RIGHT = 1200;

const int XP = 8;
const int XM = A2;
const int YP = A3;
const int YM = 9;

//  UNO R3
const int XBEGIN = 129;
const int XEND = 902;
const int YBEGIN = 94;
const int YEND = 961;

//  UNO R4 MINIMA
// const int XBEGIN = 177;
// const int XEND = 863;
// const int YBEGIN = 121;
// const int YEND = 950;

// application constants

const uint16_t GRID_W = 240;
const uint16_t CELL_W = GRID_W/3;

const uint16_t GRID_X = (DISPLAY_WIDTH - GRID_W)/2;
const uint16_t GRID_Y = (DISPLAY_HEIGHT - GRID_W)/2;

const int16_t NONE = -1;
const int16_t NAUGHT = 1;
const int16_t CROSS = 0;

const uint16_t CROSS_COLOR = RED;
const uint16_t NAUGHT_COLOR = BLUE;
const uint16_t TIE_COLOR = GRAY;

const uint16_t MARK_W = CELL_W - 40;
const uint16_t MARK_THICKNESS = 9;

const uint16_t MENU_W = 240;
const uint16_t MENU_HEIGHT = 320;

const uint16_t MENU_X = (DISPLAY_WIDTH - MENU_W)/2;
const uint16_t MENU_Y = (DISPLAY_HEIGHT - MENU_HEIGHT)/2;

const uint16_t ITEM_W = 200;
const uint16_t ITEM_H = 40;

const uint16_t ITEM_X = MENU_X + (MENU_W - ITEM_W)/2;

const uint16_t GAME_OVER_X = 80;
const uint16_t GAME_OVER_Y = 40;

const uint16_t WINNER_X = 25;
const uint16_t WINNER_Y = GRID_Y + GRID_W + 40;

const uint16_t TIE_X = GRID_X + CELL_W + 10;
const uint16_t TIE_Y = GRID_Y + GRID_W + 40;
