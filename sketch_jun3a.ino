#include <Adafruit_CircuitPlayground.h>

enum GameState { START, READY, CUE, SUCCESS, GAME_OVER };
GameState state = START;

unsigned long cueStartTime = 0;
float reactionWindow = 1500;       // 1.5 s to start
int score = 0;
int level = 1;

bool ledIsGreen = true;
int  activeLED  = 0;
bool tapCorrect = false;

void setup() {
  CircuitPlayground.begin();
  Serial.begin(9600);
  CircuitPlayground.setAccelRange(LIS3DH_RANGE_8_G);  
  CircuitPlayground.setAccelTap(1, 20);

  attachInterrupt(digitalPinToInterrupt(27),tapTime, FALLING);

  //randomSeed(analogRead(A0));       // better random LED selection
  showStartAnimation();
 
}

void tapTime(){
  tapCorrect=true;

}

// -------------------- main FSM ------------------------
void loop() {
  switch (state) {

    case START:
      if (CircuitPlayground.leftButton() && CircuitPlayground.rightButton()) {
        resetGame();
        state = READY;
      }
      break;

    case READY:
      delay(1000);
      CircuitPlayground.clearPixels();
      delay(500);
      state = CUE;
      break;

    case CUE:
      generateCue();
      cueStartTime = millis();
      state = SUCCESS;
      break;

    case SUCCESS:
      tapCorrect=false;
      if (checkCorrectInput()) {
        score++;
        Serial.print("Score: "); Serial.println(score);
        playSuccess();

        if (score % 3 == 0) {              // every 3 points → new level
          level++;
          reactionWindow = max(500.0f, reactionWindow - 100);   // floor @ 0.5 s
        
        }
        delay(500);
        state = CUE;

      } else if (millis() - cueStartTime > reactionWindow) {
        playFailure();
        state = GAME_OVER;
      }
      break;

    case GAME_OVER:
      for (int i = 0; i < 5; i++) {        // flash red 5×
        CircuitPlayground.setPixelColor(0, 255, 0, 0);
        delay(100);
        CircuitPlayground.clearPixels();
        delay(100);
      }
      // CircuitPlayground.speaker.say("Game over");
      delay(1500);
      state = START;
      break;
  }
}

void resetGame() {
  score = 0;
  level = 1;
  reactionWindow = 1500;
  CircuitPlayground.clearPixels();
  Serial.println("Game started!");
}

void showStartAnimation() {
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 0, 255);  // blue wipe
    delay(100);
    CircuitPlayground.clearPixels();
  }
}

void generateCue() {
  activeLED  = random(0, 10);         // choose LED
  ledIsGreen = random(0, 2);          // 0 = red, 1 = green

  if (ledIsGreen) {
    CircuitPlayground.setPixelColor(activeLED, 0, 255, 0);   // green → press left button
  } else {
    CircuitPlayground.setPixelColor(activeLED, 255, 0, 0);   // red → tap board
  }
}

// TRUE when the player does the correct action in time
bool checkCorrectInput() {
  if (ledIsGreen && CircuitPlayground.leftButton()){
    tapCorrect=false;
    return true;
  }          // green → left button

  // red → single tap (getAccelTap returns 1 on a single tap, 2-3 on double)
  if (!ledIsGreen && tapCorrect){
    tapCorrect=false;
    return true;
  }
  tapCorrect = false;
  return false;
}

void playSuccess() { CircuitPlayground.playTone(800, 100); }
void playFailure() { CircuitPlayground.playTone(200, 300); }