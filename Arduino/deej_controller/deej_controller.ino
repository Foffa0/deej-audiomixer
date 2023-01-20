#include <RotaryEncoder.h> //library for the rotarty encoder, can be installed from the arduino library manager
#include <FastLED.h>
//fastLED

const int LED_PIN = A3;
const int NUM_LEDS = 5;
const int brightness = 80;
CRGB leds[NUM_LEDS];
// you might have to flip the following two variable values, if your controls are inverted
//                                    {{LED1},          {LED2},        {LED3},      {LED4},      {LED5}};
const int low_colors[NUM_LEDS][3] = {{131, 0, 255}, {0, 0, 255}, {0, 197, 255}, {0, 255, 205}, {26, 220, 3}};
const int high_colors[NUM_LEDS][3] = {{200, 0, 0 }, {200, 0, 0}, {200, 0, 0}, {200, 0, 0}, {200, 0, 0}};


// Deej
// you can tweak following values for you needs
const int amountSliders = 5; // amount of sliders you want, also name them in the array below

const int PIN_ENCODER_A[5] = {10, 3, 4, 6, 9};
const int PIN_ENCODER_B[5] = {16, 2, 5, 7, 8};
const int SW[5] = {A2, A1, A0, 15, 14};

const String sliderNames[5] = {
    "Master",
    "Games",
    "Browser",
    "Discord",
    "Spotify",
};
const int increment[5] = {5, 2, 2, 2, 2};                // choose you're increment for each slider 1,2,4,5,10,20,25,50,100
int displayValue[5] = {0, 0, 0, 0, 0}; // start values for every slider

// leave following values at their default
RotaryEncoder encoders[5] = {RotaryEncoder(PIN_ENCODER_A[0], PIN_ENCODER_B[0], RotaryEncoder::LatchMode::FOUR3), RotaryEncoder(PIN_ENCODER_A[1], PIN_ENCODER_B[1], RotaryEncoder::LatchMode::FOUR3), RotaryEncoder(PIN_ENCODER_A[2], PIN_ENCODER_B[2], RotaryEncoder::LatchMode::FOUR3), RotaryEncoder(PIN_ENCODER_A[3], PIN_ENCODER_B[3], RotaryEncoder::LatchMode::FOUR3), RotaryEncoder(PIN_ENCODER_A[4], PIN_ENCODER_B[4], RotaryEncoder::LatchMode::FOUR3)};

bool prev_a[5] = {false, false, false, false, false};
bool prev_b[5] = {false, false, false, false, false};

int previousValue[5] = {100, 100, 100, 100, 100}; // extra values to see if it changed compared to last cycle
int sliderNumber = 0;                             // variable which numbers all the sliders
unsigned long lastButtonPress[5] = {0, 0, 0, 0, 0};

byte reading = 0;
const int amountIndicator = 5;

bool cur_a[5] = {false, false, false, false, false,};
bool cur_b[5] = {false, false, false, false, false,};

void setup()
{
  //FastLed Init
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);

  for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB(low_colors[i][0], low_colors[i][1], low_colors[i][2]);
  }
  
  for (int i=0; i<amountSliders; i++) {
    pinMode(SW[i], INPUT_PULLUP);
  }
  for (int i=0; i<amountSliders; i++) {
    pinMode(PIN_ENCODER_A[i], INPUT_PULLUP);
    pinMode(PIN_ENCODER_B[i], INPUT_PULLUP);
    prev_a[i] = digitalRead(PIN_ENCODER_A[i]);
    prev_b[i] = digitalRead(PIN_ENCODER_B[i]);
  }
  
  Serial.begin(9600);
  Serial.setTimeout(1);
  while (!Serial)
  {
    //  wait for Serial connection
  }
  FastLED.show();
  // request current slider values from deej
  Serial.println("getSliderValues");
}

int colortransition(int index, int color, int multip){  //Function to do the actual transition for the color channels using the differential between the high and low values with 20 steps
  int value = 0;
  int steps = 0;
  if (low_colors[index][color] <= high_colors[index][color]){
    steps = (high_colors[index][color] - low_colors[index][color]) / 20;
        if ((low_colors[index][color] + (steps * multip)) <= high_colors[index][color]){
            value = low_colors[index][color] + (steps * multip);
            //Serial.println(String("R Done ") + r);
        }
        else {
            value = high_colors[index][color];
        }
    }
  else {
    steps = (low_colors[index][color] - high_colors[index][color]) / 20;
      if ((low_colors[index][color] - (steps * multip)) <= high_colors[index][color]){
          value = high_colors[index][color];            
      }
      else {
           value = low_colors[index][color] - (steps * multip);
      }
    }

return value;
}


void colorchange(int index, float input){
  int r = 0;
  int g = 0;
  int b = 0;
  float multipf = (input / 100) * 23;
  int multip = multipf;
  r = colortransition(index, 0, multip);
  g = colortransition(index, 1, multip);
  b = colortransition(index, 2, multip);

  leds[index] = CRGB(r, g, b);
}

void loop()
{
  for (int i=0; i<amountSliders; i++) {
    cur_a[i] = digitalRead(PIN_ENCODER_A[i]);
    cur_b[i] = digitalRead(PIN_ENCODER_B[i]);

    if(prev_a[i]!= cur_a[i] || prev_b[i]!=cur_b[i]){
      encoders[i].tick();
    //Serial.println("tick");
    }
    prev_a[i] = cur_a[i];
    prev_b[i] = cur_b[i];

    RotaryEncoder::Direction direction = encoders[i].getDirection(); // get direction from encoder
    if (direction != RotaryEncoder::Direction::NOROTATION)
    { // do something if there is a rotation
      if (direction == RotaryEncoder::Direction::CLOCKWISE)
      { // direction is CW
          //Serial.println("rotateRight");
        RotateRight(i);
      }
      if (direction == RotaryEncoder::Direction::COUNTERCLOCKWISE)
      { // direction is CCW
          //Serial.println("rotateLeft");
        RotateLeft(i);  
      }
      colorchange(i, displayValue[i]);
      FastLED.show();
    }
    

    // Read the button state
    int btnState = digitalRead(SW[i]);
  
    // If we detect LOW signal, button is pressed
    if (btnState == LOW)
    {
      // if 50ms have passed since last LOW pulse, it means that the
      // button has been pressed, released and pressed again
      if (millis() - lastButtonPress[i] > 50)
      {
        ButtonPress(i);
        // Serial.println("buttonPress");
      }
  
      // Remember last button press event
      lastButtonPress[i] = millis();
    }
  };

  //if (Serial) {
    String line = Serial.readString();
    if (line.startsWith("Windows:")) {
      line.replace("Windows:", "");
      line.trim();
      int sliderId = line.substring(0, line.indexOf(";")).toInt();
      int sliderVal = line.substring(line.indexOf(";")+1, line.length()).toInt();

      //change to  displayValue[sliderId] = sliderVal;  if axis are not inverted
      displayValue[sliderId] = map(sliderVal, 0, 100, 100, 0);
      previousValue[sliderId] = displayValue[sliderId];
  
      colorchange(sliderId, displayValue[sliderId]);
    }
  //}
  
  FastLED.show();
}

void UpdateSliders()
{
  String builtString = String("");
  for (int index = 0; index < amountSliders; index++)
  {
    builtString += String(displayValue[index]);
    if (index < amountSliders - 1)
    {
      builtString += String("|");
    }
  }
  if (Serial)
  {
    Serial.println(builtString); // combining every slider values seperated by | and sending it through the serial console
  }
}

void RotateLeft(int slider)
{
  if (displayValue[slider] >= increment[slider])
  { // decreasing slider
    displayValue[slider] = displayValue[slider] - increment[slider];
    previousValue[slider] = displayValue[slider];
    UpdateSliders();
  }
}

void RotateRight(int slider)
{
  if ((100 - displayValue[slider]) >= increment[slider])
  { // increasing slider
    displayValue[slider] = displayValue[slider] + increment[slider];
    previousValue[slider] = displayValue[slider];
    UpdateSliders();
  }
}

void ButtonPress(int button)
{
  //change to  displayValue[button] > 0  if axis are not inverted (mine were so I had to change this)
  if (displayValue[button] < 100)
  { 
    previousValue[button] = displayValue[button];
    //change to displayValue[button] = 0; if axis are not inverted
    displayValue[button] = 100;
    UpdateSliders();
  } else {
    displayValue[button] = previousValue[button];
    UpdateSliders();
  }
  
  colorchange(button, displayValue[button]);
  FastLED.show();
}
