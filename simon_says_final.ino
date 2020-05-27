#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

// define the wiring of the LED screen
const uint8_t CLK  = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;

// define the wiring of the inputs
const int RED_BUTTON_PIN_NUMBER = 10;
const int BLUE_BUTTON_PIN_NUMBER = 11;
const int GREEN_BUTTON_PIN_NUMBER = 12;
const int YELLOW_BUTTON_PIN_NUMBER = A5;

//global constant for the number of possible
const int NUM_LEVELS = 32;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// the following functions are for printing messages
void print_level(int level);
void print_record(int record);
void game_over();

class Color {
  public:
    int red;
    int green;
    int blue;
    Color(){
      red = 0;
      green = 0;
      blue = 0;
    }
    
    Color(int r, int g, int b){
      red = r;
      green = g;
      blue = b;
    }
    
    uint16_t to_333()const{
      return matrix.Color333(red, green, blue);
    }
};

const Color BLACK(0, 0, 0);
const Color RED(4, 0, 0);
const Color YELLOW(4, 4, 0);
const Color GREEN(0, 4, 0);
const Color BLUE(0, 0, 4);
const Color WHITE(4, 4, 4);

// 0 corresponds with red
// 1 corresponds with blue
// 2 corresponds with green
// 3 corresponds with yellow
// z is our color number
class Rectangle{
  public:
    Rectangle(){
      x = 0;
      y = 0;
      color = BLACK;
    }
    
    Rectangle(int z){
      if(z == 0){
        x = 3;
        y = 0;
        color = RED;
      }
      else if (z == 1){
        x = 17;
        y = 0;
        color = BLUE;
      }
      else if (z == 2){
        x = 3;
        y = 9;
        color = GREEN;
      }
      else if (z == 3){
        x = 17;
        y = 9;
        color = YELLOW;
      }
    }
    
    void initialize(int x_arg, int y_arg, Color color_arg){
      x = x_arg;
      y = y_arg;
      color = color_arg;
    }
    
    int get_x(){
      return x;
    }
    
    int get_y(){
      return y;
    }
    
    void light_up(Color rectangle_color){
      for(int i = x; i < x + 12; ++i){
        for(int j = y; j < y + 7; ++j){
          matrix.drawPixel(i, j, rectangle_color.to_333());
        }
      }
    }
  
  private:
    // x and y values will be for the top left point of each rectangle
    int x = 0;
    int y = 0;
    Color color;
};

//GAME PLAN
// have the four sqares covering the whole board while the pattern is being created and while the user is pressing buttons
// to "light up" a square, have the singular square go black for .1-.3 seconds (we will decide when we play)

class Game{
  public:
    Game(){
      level = 1;
    }
    
    void initialize_pattern(){
      int x = 0;
      int y = 0;
      int z = 0; 
      for(int i = 0; i < NUM_LEVELS; ++i){
        x = random(0, 4);
        y = random(4, 19);
        z = (x * y) % 4; 
        pattern[i] = z;   
      }
    }
    
    void setupGame(){
      red.initialize(3, 0, RED);
      blue.initialize(17, 0, BLUE);
      green.initialize(3, 9, GREEN);
      yellow.initialize(17, 9, YELLOW);
    }

    void counting_levels(){
      int j = 0;
      int k = 0;
      for(int i = 0; i < level - 1; ++i){
        matrix.drawPixel(j, k, WHITE.to_333());
        matrix.drawPixel(j + 30, k, WHITE.to_333());
        j++;
        if(j > 1){
          j = 0;
          ++k;
        }
        if(k > 15){
          k = 0;
        }
      }
    }
    
    void start_level(){
      // display level and rectangles
      print_level(level);
      red.light_up(RED);
      blue.light_up(BLUE);
      green.light_up(GREEN);
      yellow.light_up(YELLOW);
      counting_levels(); 

      // empty the user array so that new things can be stored
      for(int i = 0; i < NUM_LEVELS; ++i){
        user[i] = 0;
      }

      delay(150);

      // flash pattern
      for(int i = 0; i < level; ++i){
        int s;
        s = pattern[i];
        if (s == 0){
          red.light_up(BLACK);
          delay(500);
          red.light_up(RED);
          delay(500);
        }
        else if (s == 1){
          blue.light_up(BLACK);
          delay(500);
          blue.light_up(BLUE);
          delay(500);
        }
        else if (s == 2){
          green.light_up(BLACK);
          delay(500);
          green.light_up(GREEN);
          delay(500);
        }
        else if (s == 3){
          yellow.light_up(BLACK);
          delay(500);
          yellow.light_up(YELLOW);
          delay(500);
        }
      }
    }

    int get_level(){
      return level;
    }
    
    void red_button(){
      red.light_up(BLACK);
      delay(150);
      red.light_up(RED);
    }
    
    void yellow_button(){
      yellow.light_up(BLACK);
      delay(150);
      yellow.light_up(YELLOW);
    }
    
    void green_button(){
      green.light_up(BLACK);
      delay(150);
      green.light_up(GREEN);
    }
    
    void blue_button(){
      blue.light_up(BLACK);
      delay(150);
      blue.light_up(BLUE);
    }
    
    bool button_pressed(bool red_button_pressed, bool yellow_button_pressed, bool green_button_pressed, bool blue_button_pressed){
      if (red_button_pressed || yellow_button_pressed || green_button_pressed || blue_button_pressed){
        return true;
      }
      else{
        return false;
      }
    }
    
    void store_user_move(int move, bool red_button_pressed, bool yellow_button_pressed, bool green_button_pressed, bool blue_button_pressed){
      if (red_button_pressed){
        user[move] = 0;
      }
      else if (blue_button_pressed){
        user[move] = 1;
      }
      else if (green_button_pressed){
        user[move] = 2;
      }
      else if (yellow_button_pressed){
        user[move] = 3;
      }
    }
    
    void reset_pattern(){
      for(int i = 0; i < NUM_LEVELS; ++i){
        pattern[i] = 0;
      }
    }
    
    void update(bool red_button_pressed, bool blue_button_pressed, bool green_button_pressed, bool yellow_button_pressed){
      // display button pressed
      if (red_button_pressed){
        red_button();
      }
      else if (blue_button_pressed){
        blue_button();
      }
      else if (yellow_button_pressed){
        yellow_button();
      }
      else if (green_button_pressed){
        green_button();
      }
      
      if (button_pressed(red_button_pressed, yellow_button_pressed, green_button_pressed, blue_button_pressed)){
        store_user_move(move, red_button_pressed, yellow_button_pressed, green_button_pressed, blue_button_pressed);
        if (user[move] != pattern[move]){
          game_over();
        }
        else{
          ++move;
        }
        delay(300);
      }
      
      // check when the player has made the correct number of moves to pass the level
      if(move == level){
        ++level;
        move = 0;
        start_level();
      }
    }
    
  private:
    int move = 0;
    int level;
    int pattern[NUM_LEVELS] = {0}; 
    int user[NUM_LEVELS] = {0};
    Rectangle red;
    Rectangle blue;
    Rectangle green;
    Rectangle yellow; 
};

Game game;

void setup() {
  Serial.begin(9600);
  pinMode(RED_BUTTON_PIN_NUMBER, INPUT);
  pinMode(YELLOW_BUTTON_PIN_NUMBER, INPUT);
  pinMode(GREEN_BUTTON_PIN_NUMBER, INPUT);
  pinMode(BLUE_BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
  game.initialize_pattern();
  game.setupGame();
  game.start_level();
}

void loop() {
  bool red_button_pressed = (digitalRead(RED_BUTTON_PIN_NUMBER) == HIGH);
  bool blue_button_pressed = (digitalRead(BLUE_BUTTON_PIN_NUMBER) == HIGH);
  bool green_button_pressed = (digitalRead(GREEN_BUTTON_PIN_NUMBER) == HIGH);
  bool yellow_button_pressed = (digitalRead(YELLOW_BUTTON_PIN_NUMBER) == HIGH);
  game.update(red_button_pressed, blue_button_pressed, green_button_pressed, yellow_button_pressed);
}

//always have level showing in top right
void print_level(int level){
  matrix.fillScreen(BLACK.to_333());
  matrix.setCursor(2,0);
  matrix.print("Level ");
  matrix.setCursor(13, 8);
  matrix.print(level);
  delay(1500);
  matrix.fillScreen(BLACK.to_333());
}

void game_over(){
  matrix.fillScreen(BLACK.to_333());
  matrix.setCursor(0,0);
  matrix.print("Fail Lvl: ");
  matrix.setCursor(25, 8);
  matrix.print(game.get_level());
  delay(1500);
  matrix.fillScreen(BLACK.to_333());
  game.reset_pattern();
  while(0 == 0){
    matrix.fillScreen(BLACK.to_333());
    delay(300);
    matrix.setTextColor(RED.to_333());
    matrix.setCursor(4,0);
    matrix.print("GAME OVER"); 
    delay(300);
  }
}
