#include <Adafruit_CircuitPlayground.h>
#include <math.h>
//the score starts at zero and goes up to 5
const int rounds = 5;
int score = 0;
//shake the board to start the game
bool isShaking() {
  float x = CircuitPlayground.motionX();
  float y = CircuitPlayground.motionY();
  float z = CircuitPlayground.motionZ();
  float magnitude = sqrt(x * x + y * y + z * z);
  return magnitude > 15.0;  
}

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  randomSeed(analogRead(0)); //generates a random number

  Serial.println("Shake to start the game!");
  CircuitPlayground.clearPixels();
}

void loop() {
  // Wait for shake to start
  while (!isShaking()) {
    delay(100);
  }

  Serial.println("Game started!");//cerial monitor will print "game started"
  delay(1000);

  for (int i = 0; i < rounds; i++) {
    bool isGreen = random(0, 2); // 0 = red, 1 = green

    if (isGreen) {
      Serial.println("GREEN - Press the left button!");
      CircuitPlayground.setPixelColor(0, 0, 255, 0); // green
    } else {
      Serial.println("RED - Press the right button");
      CircuitPlayground.setPixelColor(0, 255, 0, 0); // red
    }
    CircuitPlayground.strip.show();

    bool correct = false;
    unsigned long startTime = millis();//the player has up to 3 seconds to respond
    while (millis() - startTime < 3000) {
      if (isGreen && CircuitPlayground.leftButton()) {
        correct = true;
        break;
      } else if (!isGreen && CircuitPlayground.rightButton()) {
        correct = true;
        break;
      }
    }

    CircuitPlayground.clearPixels();//once the score gets to 5 all pixles are cleared

    if (correct) {
      score++;
      CircuitPlayground.playTone(880, 200);
      Serial.print("Correct! Score: ");
      Serial.println(score);
    } else {
      CircuitPlayground.playTone(220, 500);
      Serial.println("Wrong or too slow! Game over.");
      break;
    }

    delay(500);
  }

  Serial.print("Final Score: ");
Serial.println(score);

// Light up all pixels blue
for (int i = 0; i < 10; i++) {
  CircuitPlayground.setPixelColor(i, 0, 0, 255);
}
CircuitPlayground.strip.show();

// Sound
CircuitPlayground.playTone(523, 300);
CircuitPlayground.playTone(659, 300);

// Wait for player to shake to restart
Serial.println("Shake to play again!");
score = 0;
while (!isShaking()) {
  delay(100);
}
}

