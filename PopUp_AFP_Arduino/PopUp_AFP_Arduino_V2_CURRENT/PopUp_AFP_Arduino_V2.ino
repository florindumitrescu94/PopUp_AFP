#include <math.h>
#include <Servo.h>
#include <EEPROM.h>
#define MOTOR 5
#define LIGHT 3
#define QUEUELENGTH         10             // number of commands that can be saved in the serial queue
#define MAXCOMMAND          42            // max length of a command
#define EOFSTR              '\n'
#define EOCOMMAND           '#'           // defines the end character of a command
#define SOCOMMAND           '>'           // defines the start character of a command
#define ADDR 1 // defines eeprom address

Servo PanelServo;
int panel_light=0; // State for panel light
int eeprom_position = 0; // state for EEPROM position
int panel_brightness=0; // Value for panel light 0-255
int panel_position;
int panel_state=0; //0 = Closed. 1 = Open 2 = Unknown 3 = Error 4 = Moving
int panel_closed_pos=0; // define close position
int panel_open_pos=270; // define open position
int target_state = 3; // used to hold target state (close = 0 , open = 1, done = 3, halt  = 4). Initialize with 3
char* queue[QUEUELENGTH];
int queueHead = -1;
int queueCount = 0;
int idx = 0;                              // index into the command string
String line;       

char* pop() {
  --queueCount;
  return queue[queueHead--];
}


void push(char command[MAXCOMMAND]) {
  queueCount++;
  queueHead++;
  strncpy(queue[queueHead], command, MAXCOMMAND);
}

void setup() {
  TCCR2B = TCCR2B & B11111000 | B00000001;
  pinMode(LIGHT,OUTPUT);
  pinMode(MOTOR,OUTPUT);
  analogWrite(LIGHT,0); // Set panel light to off
  eeprom_position = EEPROM.read(ADDR);
  if (eeprom_position == 1) PanelServo.write(panel_open_pos);
  if (eeprom_position == 0) PanelServo.write(panel_closed_pos);
  PanelServo.attach(MOTOR); // Attach servo
  panel_state = 2; // Panel position will be unkown at beginning;
  for ( int i=0; i < QUEUELENGTH; i++)
  queue[i] = (char*)malloc(MAXCOMMAND);
  line.reserve(MAXCOMMAND);
  Serial.begin(9600); // begin seral comm over com port
  Serial.flush();
}

void clearSerialPort() {
  while ( Serial.available() )
    Serial.read();
}

// SerialEvent occurs whenever new data comes in the serial RX.
// you should really consider a start of command character.
void serialEvent() {

  // '#' ends the command, do not store these in the command buffer
  // read the command until the terminating # character
  char buf[MAXCOMMAND];
  while ( Serial.available() )
  {
    char inChar = Serial.read();
    switch ( inChar )
    {
      case '>':     // soc, reinit line
        // memset(line, 0, MAXCOMMAND);
        line = "";
        idx = 0;
        break;
      case '#':     // eoc
        line.toCharArray(buf, MAXCOMMAND);
        idx = 0;
        push(buf);
        break;
      default:      // anything else
        if ( idx < MAXCOMMAND - 1) {
          line += inChar;
        }
        break;
    }
  }
}

void processSerialCommand(){
   if ( queueCount == 0 )
    return;

  String cmd = String(pop());
  if (cmd == "OPEN")
  {
   PanelServo.write(panel_open_pos);
   panel_state = 1;
   EEPROM.write(ADDR,panel_state);
  }
  else if (cmd == "CLOSE")
  {
    PanelServo.write(panel_closed_pos);
    panel_state = 0;
    EEPROM.write(ADDR,panel_state);
  }
  else if (cmd.substring(0,7) == "LIGHTON") 
  {
      panel_brightness = (cmd.substring((cmd.indexOf('_')+1),(cmd.indexOf('_')+4)).toInt());
      analogWrite(LIGHT,panel_brightness); // turn on light with set brightness
      panel_light = 1;
      //Serial.print(panel_light);
      //Serial.println("#");
  }
  else if (cmd == "LIGHTOFF") 
  {
      analogWrite(LIGHT,0); // turn on light with set brightness
      panel_light = 0;
  }
  
  else if (cmd == "GETLIGHTSTATE")
  {
    Serial.print(panel_light);
    Serial.print("#");
  }
  else if (cmd == "GETSTATE")
  {
    Serial.print(panel_state);
    Serial.print("#");
  }
  else if (cmd == "GETLIGHT")
  {
    Serial.print(panel_brightness);
    Serial.print("#");
  }
  else if (cmd == "GETPOSITION")
  {
    Serial.print(panel_position);
    Serial.println("#");
  }
  else if (cmd.substring(0,7) == "SETOPEN") 
  {
    panel_open_pos = (cmd.substring((cmd.indexOf('_')+1),(cmd.indexOf('_')+5)).toInt());
  }
  else if (cmd.substring(0,9) == "SETCLOSED") 
  {
    panel_closed_pos = (cmd.substring((cmd.indexOf('_')+1),(cmd.indexOf('_')+5)).toInt());
  }
}

void loop() {

  if ( queueCount >= 1 ) {               // check for serial command
    processSerialCommand();
  }
}
