#include "esd.c"
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

const uint8_t ROWS = 4;
const uint8_t COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '/', 'D' }
};
uint8_t colPins[COLS] = { 26, 25, 33, 32 }; // Pins connected to C1, C2, C3, C4
uint8_t rowPins[ROWS] = { 13, 12, 14, 27 };  // Pins connected to R1, R2, R3, R4

#define I2C_ADDR    0x27
#define LCD_COLUMNS 20
#define LCD_LINES   4

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Entry point
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.cursor();
}

typedef enum {
  State_Prompt,
  State_Eval,
  State_Waiting,
} State;

State state = State_Prompt;
char prompt[256] = { 0 };
int prompt_i = 0;
bool shift = false;
#define PROMPT_WRITE(c) prompt[prompt_i]=c;prompt_i++;lcd.print(c);
void loop() {
  delay(100);
  char key = keypad.getKey();
  switch (state) {
    case State_Prompt: {
        if (key != NO_KEY) {
          switch (key) {
            case 'A':
              state = State_Eval;
              break;
            case 'B':              
              shift = true;
              break;
            case 'C':
              if (shift) {
                PROMPT_WRITE('(');
                shift = false;
              } else {
                PROMPT_WRITE('-');
              }
              break;
            case 'D':
              if (shift) {
                PROMPT_WRITE(')');
                shift = false;
              } else {
                PROMPT_WRITE('+');
              }
              break;
            default:
              PROMPT_WRITE(key);
              shift = false;
          }
        }
        break;
      }
    case State_Eval: {
        Parser parser = {{prompt, strlen(prompt)}, 0};
        Expr * expr = parse(&parser);
        Value result = eval(expr);
        lcd.clear();
        lcd.home();
        switch (result.type) {
          case ValueType_Int:
            lcd.print(result._int);
            break;
          case ValueType_Float:
            lcd.print(result._float);
            break;
        }
        free_expr(expr);
        state = State_Waiting;
        break;
      }
    case State_Waiting: {
      if (key == 'A') {
        state = State_Prompt;
        for (int i = 0; i < 256; i++) prompt[i] = 0;
        prompt_i = 0;
        lcd.clear();
        lcd.home();
      }
      break;
    }
  }

}
