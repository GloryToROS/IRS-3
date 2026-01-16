/*
   code write for project:
   https://github.com/Ni3nayka/IRS-1/

   author: Egor Bakay <egor_bakay@inbox.ru> Ni3nayka
   write:  October 2024
   modify: november 2025
*/
 
#include "movies.h"

void setup() {
  setupRobot();
	//test();
	Motors.runs(0,0,0,0);
  // test();
}

void loop() {
  // cyclics:
  // gy25.update();
  // motorSpeedEncUpdate();
  // serialROSEnc();
  serialDataParser();
  serialROSEnc();
}

void serialDataParser() {
  SerialData data = readSerialData(); // Запрашиваем данные с Serial
  if (data.mode != '\0') { // Если получены данные, обрабатываем их
    if      (data.mode=='m' && data.data_counter==2) Motors.run(data.data_1, data.data_2);
    else if (data.mode=='M' && data.data_counter==4) Motors.runs(data.data_1, data.data_2, data.data_3, data.data_4);
    else if (data.mode=='n' && data.data_counter==2) motorCreateTargetSpeedEncRun(data.data_1, data.data_2);
    else if (data.mode=='N' && data.data_counter==4) motorCreateTargetSpeedEncRun2(data.data_1, data.data_2, data.data_3, data.data_4);
    // else if (data.mode=='F' && data.data_counter==1) runGyro(data.data_1);
    // else if (data.mode=='R' && data.data_counter==1) turnGyro(data.data_1);
    // else if (data.mode=='L' && data.data_counter==1) turnGyro(-data.data_1);
    else if (data.mode=='g' && data.data_counter==0) sendDataToSerial();
    // else if (data.mode=='C' && data.data_counter==1) motorTargetSpeedEncRun(1,data.data_1);
    // else if (data.mode=='t' && data.data_counter==1) test(); // testErrorEncDistanseRun(data.data_1);
    else Serial.println("ERROR: unknow command");
  }
}

// void test() {
//   gy25.delayUpdate(5000);
//   for (int i = 0; i<4; i++) {
//     runGyro(320);
//     turnGyro(-90);
//   }
// }
