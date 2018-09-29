// Enable debug prints to serial monitor
#define MY_DEBUG
 
// Enable serial gateway
#define MY_GATEWAY_SERIAL
 
// Define a lower baud rate for Arduino's running on 8 MHz (Arduino Pro Mini 3.3V & SenseBender)
#if F_CPU == 8000000L
#define MY_BAUD_RATE 38400
#endif
 
#define NUMBER_OF_EXPANDERS 1
#define RELAYS_PER_EXPANDER 12
#define NUMBER_OF_RELAYS NUMBER_OF_EXPANDERS * RELAYS_PER_EXPANDER
#define RELAY_ON 1  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0 // GPIO value to write to turn off attached relay
 
#include <SPI.h>
#include <MySensors.h>
#include <OneButton.h>
#include <Adafruit_MCP23017.h>
 
Adafruit_MCP23017 expanders[NUMBER_OF_EXPANDERS];
 
void expanderDigitalWrite(int sensor, bool state) {
 int expander = sensor / RELAYS_PER_EXPANDER;
 int relay = sensor % RELAYS_PER_EXPANDER;
 expanders[expander].digitalWrite(relay, state);
}
 
void toggleRelayState(int sensor) {
  bool newState = !loadState(sensor);
  expanderDigitalWrite(sensor, newState?RELAY_ON:RELAY_OFF);
  saveState(sensor, newState);
  send(MyMessage(sensor, V_LIGHT).set(newState));
}
 
void changeRelayState(int sensor, bool state) {
    expanderDigitalWrite(sensor, state?RELAY_ON:RELAY_OFF);
    saveState(sensor, state);
    send(MyMessage(sensor, V_LIGHT).set(state));
}
 
void before() {
 for(int expander=0; expander<NUMBER_OF_EXPANDERS; expander++) {
   Adafruit_MCP23017 mcp;
   mcp.begin(expander);
   for(int relay = 0; relay < RELAYS_PER_EXPANDER; relay++) {
     mcp.pinMode(relay, OUTPUT);
     mcp.digitalWrite(relay, loadState(expander * RELAYS_PER_EXPANDER + relay)?RELAY_ON:RELAY_OFF);
   }
   expanders[expander] = mcp;
 }
}
 
OneButton button_a_0(13, true);
OneButton button_b_0(9, true);
OneButton button_b_1(10, true);
 
void setup() {
  button_a_0.attachClick(toggleHallLight); // one click light on
  button_a_0.attachLongPressStart(turnOffAllLights); // longer press switch off the lights
 
  button_b_0.attachClick(toggleRoom0Light0); // first line one click lights on
  button_b_1.attachClick(toggleRoom0Light1); // second line one click lights on
  button_b_0.attachDoubleClick(turnOnRoom0Lights); // double click turn on all lights
  button_b_1.attachDoubleClick(turnOnRoom0Lights); // double click turn on all lights
  button_b_0.attachLongPressStart(turnOffRoom0Lights); // longer press turn off all lights
  button_b_1.attachLongPressStart(turnOffRoom0Lights); // longer press turn off all lights
}
 
void presentation()
{
 // Send the sketch version information to the gateway and Controller
 sendSketchInfo("Relay", "1.0");
 
 for (int relay=0; relay< NUMBER_OF_RELAYS; relay++) {
   present(relay, S_LIGHT);
 }
}
 
void loop() {
  button_a_0.tick();
  button_b_0.tick();
  button_b_1.tick();
}
 
void receive(const MyMessage &message) {
//   We only expect one type of message from controller. But we better check anyway.
 if (message.type==V_LIGHT) {
    changeRelayState(message.sensor, message.getBool());
 
    // Write some debug info
    Serial.print("Incoming change for sensor:");
    Serial.print(message.sensor);
    Serial.print(", New status: ");
    Serial.println(message.getBool());
    
  }
}
 
void toggleHallLight() {
  toggleRelayState(2);
}
 
void toggleRoom0Light0() {
  toggleRelayState(0);
}
 
void toggleRoom0Light1() {
  toggleRelayState(1);
}
 
void turnOnRoom0Lights() {
  changeRelayState(0, RELAY_ON);
  changeRelayState(1, RELAY_ON);
}
 
void turnOffRoom0Lights() {
  changeRelayState(0, RELAY_OFF);
  changeRelayState(1, RELAY_OFF);
}
 
void turnOffAllLights() {
  for (int relay=0; relay< NUMBER_OF_RELAYS; relay++) {
    changeRelayState(relay, RELAY_OFF);
  }
}
