#include <XBee.h>

#define LM  0 // Left Motor
#define LMS 6 // Left Motor Signal Pin
#define LMD 5 // Left Motor Direction Pin
#define RM  1 // Right Motor
#define RMD 4 // Right Motor Signal Pin
#define RMS 3 // Right Motor Direction Pin

#define MAX       (1024)
#define MID       (MAX/2)
#define RANGE     (MAX/2)
#define FACTOR    (RANGE/256)
#define SCALE(x)  (x/FACTOR)
#define EXPC(x)   (ltb[x])

#define DEAD      (10)
#define FULL      (150)

unsigned char ltb[] = {0,0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,4,4,4,4,5,5,5,5,5,6,6,6,6,7,7,7,7,8,8,8,8,9,9,9,9,10,10,10,11,11,11,11,12,12,12,13,13,13,14,14,14,15,15,15,16,16,16,17,17,17,18,18,18,19,19,20,20,20,21,21,22,22,23,23,23,24,24,25,25,26,26,27,27,28,28,29,29,30,30,31,31,32,32,33,33,34,34,35,36,36,37,37,38,39,39,40,40,41,42,42,43,44,44,45,46,46,47,48,49,49,50,51,52,52,53,54,55,56,56,57,58,59,60,61,62,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,82,83,84,85,86,87,89,90,91,92,93,95,96,97,99,100,101,103,104,105,107,108,110,111,113,114,116,117,119,120,122,123,125,127,128,130,132,134,135,137,139,141,142,144,146,148,150,152,154,156,158,160,162,164,166,168,170,173,175,177,179,182,184,186,189,191,193,196,198,201,203,206,209,211,214,216,219,222,225,228,230,233,236,239,242,245,248,251,255};

XBee xbee = XBee();

Rx16IoSampleResponse io16 = Rx16IoSampleResponse();
Rx64IoSampleResponse io64 = Rx64IoSampleResponse();

void setup() {
  pinMode(LMS, OUTPUT);
  pinMode(LMD, OUTPUT);
  pinMode(RMS, OUTPUT);
  pinMode(RMD, OUTPUT);
  
  Serial.begin(9600);
  Serial1.begin(9600);
  
  xbee.setSerial(Serial1);
}

void loop() {
  XBeeResponse response;
  
  xbee.readPacket();
  
  response = xbee.getResponse();
  if (response.isAvailable()) {
    if (response.getApiId() == RX_16_IO_RESPONSE) {
      response.getRx16IoSampleResponse(io16);
      setMotors(MAX - io16.getAnalog(0,0), io16.getAnalog(2,0));
    } else if (response.getApiId() == RX_64_IO_RESPONSE) {
      response.getRx64IoSampleResponse(io64);
      setMotors(MAX - io64.getAnalog(0,0), io64.getAnalog(2,0));
    } else {
      Serial.print("API Packet: ");
      Serial.println(response.getApiId(), HEX);
    }
  }
}

void setMotors(int a, int b) {
  int r = a - MID;
  int l = b - MID;
  
  mspeed(RM, r);
  mspeed(LM, l);
}

void mspeed(byte motor, int speed) {
  byte s_pin = (motor == LM) ? LMS : (motor == RM) ? RMS : 0;
  byte d_pin = (motor == LM) ? LMD : (motor == RM) ? RMD : 0;
  
  if (motor)
    Serial.print("Right motor ");
  else
    Serial.print("Left motor ");
  
  if (!s_pin || !d_pin) return; // motor was not valid
  
  byte dir = speed < 0;
  
  if (dir)
    Serial.print("backwards ");
  else
    Serial.print("forwards ");
  
  if (speed < 0)
    speed *= -1;
  
  speed = SCALE(speed);
  
  if (speed > 255)
    speed = 255;
  
  speed = EXPC(speed);
  
  Serial.println(speed, DEC);
  
  digitalWrite(dir, d_pin);
  analogWrite(speed, s_pin);
}
