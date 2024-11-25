#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Bounce2.h>

#include "hardware/pio.h"
#include "quadrature.pio.h"
#include "ui.h"
#include "heater.h"
#include "thermistor.h"

const int I2C_SDA_PIN = 18;
const int I2C_SCL_PIN = 19;
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_ADDRESS = 0x3C;

const int ENCODER_PIN_A = 0;
const int ENCODER_PIN_B = 1;
const int ENCODER_BUTTON_PIN = 2;

const int HEATER_PIN = 15;
const int HEATER_MAX_POWER = 2300;

const int THERMISTOR_PIN = 28;
const int THERMISTOR_SERIES_RESISTOR = 1000;

uint offset, sm;
PIO pio = pio0;
TwoWire I2C(I2C_SDA_PIN, I2C_SCL_PIN);
Bounce encoderButton = Bounce();
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C);
Heater heater(HEATER_PIN);
Thermistor thermistor(THERMISTOR_PIN, THERMISTOR_SERIES_RESISTOR);

ThermometerUI thermometer(&display);

int getEncoderValue()
{
  pio_sm_exec_wait_blocking(pio, sm, pio_encode_in(pio_x, 32));
  return -(int)pio_sm_get_blocking(pio, sm) / 2;
}

void setup()
{
  Serial.begin(9600);
  pinMode(ENCODER_BUTTON_PIN, INPUT_PULLDOWN);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  offset = pio_add_program(pio, &quadratureA_program);
  sm = pio_claim_unused_sm(pio, true);
  quadratureA_program_init(pio, sm, offset, ENCODER_PIN_A, ENCODER_PIN_B);
  pinMode(ENCODER_PIN_A, INPUT_PULLDOWN);
  pinMode(ENCODER_PIN_B, INPUT_PULLDOWN);
  encoderButton.attach(ENCODER_BUTTON_PIN, INPUT_PULLDOWN);
  encoderButton.interval(5);
}

bool stateChanged = false;
int pressBegin = 0;
int lastEncoderValue = 0;

int targetTemperature = 100;

void loop()
{
  float measuredTemperature = thermistor.getTemperatureC();
  if(measuredTemperature < -50 || measuredTemperature > 1000)
  {
    heater.disable();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.println("Thermistor error");
    display.println("Check connections");
    display.display();
    sleep_ms(100);
    return;
  }

  // Encoder button
  encoderButton.update();
  if (encoderButton.changed() && encoderButton.rose())
  {
    pressBegin = millis();
    stateChanged = false;
  }
    
  if (encoderButton.read() == HIGH && !stateChanged)
  {
    if(heater.isEnabled())
    {
      heater.disable();
      stateChanged = true;
    }
    if(!heater.isEnabled() && millis() - pressBegin > 2000)
    {
      heater.enable();
      stateChanged = true;
    }
  }

  // Encoder
  int encoderValue = getEncoderValue();
  int encoderDelta = encoderValue - lastEncoderValue;
  lastEncoderValue = encoderValue;
  targetTemperature += encoderDelta * 5;
  if(targetTemperature < 0) targetTemperature = 0;
  if(targetTemperature > 500) targetTemperature = 500;
  heater.setTemperature(targetTemperature);
  heater.updateTemperature(measuredTemperature);
  heater.update();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Hotplate control");

  display.setCursor(16, 20);
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.println(targetTemperature);

  if(heater.isEnabled())
  {
    display.fillRect(0, 54, 43, 9, WHITE);
    display.setCursor(1, 55);
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.print("ENABLED");
    display.setTextColor(WHITE);
    display.print((" " + std::to_string((int)round(heater.getPowerUsage(HEATER_MAX_POWER))) + "W").c_str());
  }
  else
  {
    display.setCursor(1, 55);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.println("DISABLED");
  }
  
  thermometer.draw(SCREEN_WIDTH - THERMOMETER_IMG_WIDTH, 0, thermistor.getTemperatureC());
  display.display();
}
