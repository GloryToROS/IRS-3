#define GY25_STRAFE_DT 6500
#define GY25_STRAFE_ANDLE 1
#define GY25_SERIAL Serial2
long int gy25_lastPrintTime = 0;
#include "gy-25.h"
GY25 gy25; // (12, 8); (указываем RX и TX пины)
#include "musor.h"

long int getGyro() {
  return gy25.horizontal_angle_strafe; 
  // return gy25.horizontal_angle;
}

void sendCommand(char command, int a, bool timeout=true) {
  char buffer[10]; // Буфер достаточного размера
  itoa(a, buffer, 10);
  Serial3.write(command);
  Serial3.write(' ');
  for(int i = 0; buffer[i] != '\0'; i++) {
    Serial3.write(buffer[i]);
  }
  Serial3.write("\n");
  if (timeout) waitForCommandWithTimeout();
  Serial.print("end command: ");
  Serial.print(command);
  Serial.print(" ");
  Serial.println(a);
}

void forward(int a) {
  sendCommand('F', a);
}

void left(int angle) {
  if (angle>0) Serial3.write("l\n");
  else Serial3.write("r\n");
  long int target = getGyro() + angle;
  while (abs(target-getGyro())>4) {
    gy25.update();
    // Serial.print(target);
    // Serial.print(" ");
    // Serial.println(gy25.horizontal_angle);
  }
  Serial3.write("S\n");
}

void napravlenie(int angle) {
  gy25.update();
  //if (angle>gy25.horizontal_angle) 
  left(angle-getGyro());
}

void delayGyro(long int t) {
  for (t += millis(); t>millis();) gy25.update();
}

void run() {
  sendCommand('F', 128);
  delayGyro(1000);
  napravlenie(-88);
  delayGyro(1000);
  Serial3.write("W\n");
  delayGyro(1000);
  forward(213);
  delayGyro(1000);
  napravlenie(-180);
  delayGyro(1000);
  sendCommand('F', 100); // 95
  delayGyro(1000);
  napravlenie(-100); // -87
  delayGyro(1000);
  sendCommand('F', 57);
  delayGyro(1000);
  napravlenie(-1);
  delayGyro(1000);
  sendCommand('R', 10);
  delayGyro(1000);
  sendCommand('F', 141); // 127
  delayGyro(1000); // закончили чистку "первой"
  napravlenie(88);
  delayGyro(1000);
  sendCommand('F', 265); //65
  delayGyro(1000);
  
  
  Serial3.write("W\n");
  delayGyro(1000);
  napravlenie(0);
  delayGyro(1000);
  sendCommand('F', 50);
  delayGyro(1000);
  sendCommand('F', -30);
  delayGyro(1000);
  napravlenie(0);
  delayGyro(1000);
  sendCommand('L', 20);
  delayGyro(1000);
  sendCommand('F', 30);
  delayGyro(1000);
  sendCommand('F', -30);
  delayGyro(1000);
  napravlenie(0);
  delayGyro(1000);
  sendCommand('R', 20);
  delayGyro(1000);
  sendCommand('F', 30);
  delayGyro(1000);
  sendCommand('F', -30);
  delayGyro(1000);
  // napravlenie(0);
  // delayGyro(1000);
  // sendCommand('R', 12);
  // delayGyro(1000);



  // napravlenie(-25);
  // delayGyro(1000);
  // sendCommand('F', 90);
  // delayGyro(1000);
  // napravlenie(90);
  // delayGyro(1000);
  // sendCommand('R', 20);
  // delayGyro(1000);
  // sendCommand('F', 90);
  // delayGyro(1000);
  // sendCommand('F', -90);
  // delayGyro(1000);
  // napravlenie(140);
  // delayGyro(1000);
  // sendCommand('F', 97);
  // delayGyro(1000);
  // napravlenie(180); // 184
  // delayGyro(1000);
  // sendCommand('R', 40);
  // delayGyro(1000);
  // sendCommand('L', 20);
  // delayGyro(1000);
  // sendCommand('F', 191);
  // delayGyro(1000);

  delayGyro(1000);
  napravlenie(180);
  delayGyro(1000);
  sendCommand('F', 200);


  Serial3.write("w\n");
  delayGyro(1000);
  
}

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600); // arduino uno = motor
	gy25.setup(); // gy25.calibration();
	Serial2.begin(115200); // ПОТОМУ ЧТО ТУПАЯ АРДУИНА 
  delayGyro(1000);
  run();
  delayGyro(1000);
  Serial3.write("w\n");
}

void loop() { // run over and over
  serialMotor();
  // serialGyroParserToMotor();
  gy25.update();
}


