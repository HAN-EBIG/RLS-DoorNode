#include <DallasTemperature.h>
#include <OneWire.h>


// Enable debug prints to serial monitor
#define MY_DEBUG

#define MY_RADIO_NRF24
#define MY_NODE_ID 50


#define CHILD_ID_TEMP_MIDDLE 1


#include <MySensors.h>

#define SKETCH_NAME "Binary Sensor"
#define SKETCH_MAJOR_VER "1"
#define SKETCH_MINOR_VER "0"

#define PRIMARY_CHILD_ID 3

static const uint8_t FORCE_UPDATE_N_READS = 5;

#define PRIMARY_BUTTON_PIN 2   // Arduino Digital I/O pin for button/reed switch

// Temperature sensor
#define ONE_WIRE_BUS 4 // Pin where dallase sensor is connected 
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire); 

float lastTemperature;
uint8_t nNoUpdatesTemp;
MyMessage tempMsg(CHILD_ID_TEMP_MIDDLE,V_TEMP);


// Change to V_LIGHT if you use S_LIGHT in presentation below
MyMessage msg(PRIMARY_CHILD_ID, V_TRIPPED);

void before()
{
    sensors.begin();

}

void setup()
{
  // Setup the buttons
  pinMode(PRIMARY_BUTTON_PIN, INPUT);

  // Activate internal pull-ups
  digitalWrite(PRIMARY_BUTTON_PIN, HIGH);
}

void presentation()
{
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);

  // Register binary input sensor to sensor_node (they will be created as child devices)
  present(PRIMARY_CHILD_ID, S_DOOR);
    present(CHILD_ID_TEMP_MIDDLE,S_TEMP);

}

// Loop will iterate on changes on the BUTTON_PINs
void loop()
{
  uint8_t value;
  static uint8_t sentValue=2;

  // Short delay to allow buttons to properly settle
  sleep(5);

  value = digitalRead(PRIMARY_BUTTON_PIN);

  if (value != sentValue) {
    // Value has changed from last transmission, send the updated value
    send(msg.set(value==HIGH));
    sentValue = value;
  }

  processTemperature();

  // Sleep until something happens with the sensor
  sleep(PRIMARY_BUTTON_PIN-2, CHANGE, 10000);
}


void processTemperature()
{
  sensors.requestTemperatures();
  float temperature =  sensors.getTempCByIndex(0); 

  if ((lastTemperature != temperature && temperature != -127.00 && temperature != 85.00) || nNoUpdatesTemp == FORCE_UPDATE_N_READS) {
    lastTemperature=temperature;  
    nNoUpdatesTemp = 0;
    send(tempMsg.set(temperature,1));
  } else {
    nNoUpdatesTemp++;  
  }
}
