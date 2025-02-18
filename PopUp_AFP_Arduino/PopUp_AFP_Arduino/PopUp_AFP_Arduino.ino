#include <math.h>
#define POSITION A0
#define OPEN 6
#define CLOSE 5
#define LIGHT 3
#define QUEUELENGTH         10             // number of commands that can be saved in the serial queue
#define MAXCOMMAND          42            // max length of a command
#define EOFSTR              '\n'
#define EOCOMMAND           '#'           // defines the end character of a command
#define SOCOMMAND           '>'           // defines the start character of a command


int sample_sum = 0;
int panel_position=0; // Current panel position as read by potentiometer
int target_panel_pos = 0;
int panel_light=0; // State for panel light
int panel_brightness=0; // Value for panel light 0-255
int panel_dir=0; // 0 = Closing. 1 = Opening TO BE DETERMINED
int panel_state=0; //0 = Closed. 1 = Open 2 = Unknown 3 = Error 4 = Moving
int panel_closed_pos=0; // define close position
int panel_open_pos=930; // define open position
int target_state = 3; // used to hold target state (close = 0 , open = 1, done = 3, halt  = 4). Initialize with 3
int panel_fast = 180; // value for fast panel movement
int panel_slow = 60; // value for slow panel movement
float accel_spd = 30; // increasing speed
int accel_int = 0; // int value
int opening = 0;
int closing = 0;
char* queue[QUEUELENGTH];
int queueHead = -1;
int queueCount = 0;
int idx = 0;                              // index into the command string
String line;       
int watchdog_time_span = 0;
int watchdog_pos_0 = 0;
int watchdog_pos_1 = 0;

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
  TCCR2B = TCCR2B & B11111000 | B00000001; // set pin 3 pwm frequency to 31khz
  pinMode(POSITION,INPUT);
  pinMode(OPEN,OUTPUT);
  pinMode(CLOSE,OUTPUT);
  pinMode(LIGHT,OUTPUT);
  analogWrite(LIGHT,0); // Set panel light to off 
  int position_sum=0;
  for (int j=0;j<32;j++)
  {
    position_sum +=analogRead(POSITION);
  }
  panel_position = round(position_sum/32); // parse panel position at startup
  if (panel_position!=panel_open_pos || panel_position!=panel_closed_pos) 
  {
    panel_state = 2;
  }
  if (panel_position-panel_closed_pos < 10)
  {
    panel_state = 0;
  }
  if (panel_open_pos - panel_position < 50)
  {
    panel_state = 1;
  }
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
    target_state = 1;
  }
  else if (cmd == "CLOSE")
  {
    target_state = 0;
  }
  else if (cmd == "HALT")
  {
    target_state = 4;
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
     //Serial.print(panel_light);
      //Serial.println("#");
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
  watchDog ();

  if ( queueCount >= 1 ) {               // check for serial command
    processSerialCommand();
  }
  sample_sum = 0;
  for (int i = 0; i < 32; i++)
  {
    sample_sum = sample_sum + analogRead(POSITION);
  }
  int avg = sample_sum/32;
  panel_position = round(sample_sum/32);

  
  if (target_state == 4) // HALT command. stops all movement and sets variables
  {
    analogWrite(OPEN,0);
    analogWrite(CLOSE,0);
    panel_state = 2;
    target_state = 3;
    Serial.print(panel_state);
    Serial.print("#");
  }




  
  if (target_state == 0) // if target action is to close the panel
  {
    panel_state = 4 ;
    analogWrite(OPEN,0); 
    if (accel_spd <= panel_fast)
    {
    accel_spd += 0.5;
    }
    accel_int = round(accel_spd);
    if (panel_position > panel_closed_pos + 200) // loop for fast movement
    {
      analogWrite(CLOSE,accel_int);
    }
    if (panel_position <= panel_closed_pos + 200 && panel_position > panel_closed_pos) // loop for slow movement
    {
      analogWrite(CLOSE,panel_slow);
    } 
    if (panel_position == panel_closed_pos || panel_position < panel_closed_pos)
    {
      if (panel_closed_pos == 0)
      {
        for (int i=0;i<420;i++)
        {
          analogWrite(CLOSE,panel_slow);
        }
        
      }
      accel_spd = 30;
      panel_state = 0; // panel is now closed
      target_state = 3; // target_state = 3 -> panel has reached desired position, skip the IFs
      analogWrite(CLOSE,0);
      Serial.print(panel_state);
      Serial.print("#");
    }
  }

  
  
  if (target_state == 1) // if target action is to open the panel
  {
    panel_state = 4;
    analogWrite(CLOSE,0);
    if (accel_spd <= panel_fast)
    {
    accel_spd += 0.5;
    }
    accel_int = round(accel_spd);
    if (panel_position < panel_open_pos - 200) // loop for fast movement
    {
      analogWrite(OPEN,accel_int);
    }
    if (panel_position > panel_open_pos - 200 && panel_position < panel_open_pos ) // loop for slow movement
    {
      analogWrite(OPEN,panel_slow);
    } 
    if (panel_position == panel_open_pos || panel_position >panel_open_pos )
    {
      panel_state = 1; // panel is now open
      target_state = 3; // target_state = 3 -> panel has reached desired position;
      analogWrite(OPEN,0);
      accel_spd = 30;
      Serial.print(panel_state);
      Serial.print("#");
    }
  }
}
void watchDog () // checks to see if the motor is stalled every 250 "ticks"
{
  if (target_state == 0 || target_state == 1)
  { 
    if (watchdog_time_span == 0) 
    {
      watchdog_pos_0 = panel_position; // at the start of the 150 ticks, assigns position 0 as current panel position
    }
    watchdog_time_span += 1; // increase by 1
    if (watchdog_time_span == 250) // at tick 250 do
      { 
        watchdog_pos_1 = panel_position; //assign position 1 as current panel position 
        if (abs(watchdog_pos_1 - watchdog_pos_0) < 5) // if the absolute difference between the two positions is less than 10 do
        {
          analogWrite(OPEN,0); // cancels all movement
          analogWrite(CLOSE,0); //cancels all movement
          target_state = 3; // sets target state to 3
          panel_state = 3; // sets sets panel state to 3 (error in Driver)
        }
        watchdog_time_span = 0; // reset counter
      }
  }
  if (target_state == 3 || target_state == 4)
  {
    watchdog_time_span = 0; // reset counter to 0 if state 3 or 4 are reached
    watchdog_pos_0 = 0;
    watchdog_pos_1 = 0;
  }
}
