// Файл для функций движения робота
#pragma once

#define PARROT_IN_ONE_FULL_ROTATION_WHEEL 1320
#define DIAMETER_WHEEL 8 // cm

#include "BTS7960_PRO.h"
// #include <Servo.h>

// Массив пинов сервоприводов
// #define SERVO_QUANTITY 2
// const uint8_t servoPins[SERVO_QUANTITY] = {10, 11};
// const uint8_t servoCount = sizeof(servoPins) / sizeof(servoPins[0]);
// Servo servos[SERVO_QUANTITY];
// int currentPositions[SERVO_QUANTITY] = {90, 90};

// #define GY25_STRAFE_DT 5000 //1500
// #define GY25_SERIAL Serial2
// #include "gy-25.h"
// GY25 gy25; // (12, 8); (указываем RX и TX пины)
// unsigned long int gy25_lastPrintTime = 0;

// Подключаем файл с энкодерами
// #include "encoders.h"
#include <arduino_encoder.h> 
Encoder enc1;
Encoder enc2;
Encoder enc3;
Encoder enc4;

// Объявление объекта управления моторами
BTS7960_PRO Motors;


// ========================= VOLTAGE ==============================

const int voltage_counter = 2;
const int voltage_pins[voltage_counter] = {A4, A5};

float getVoltage(int number) {
	if (number>voltage_counter || number<1) return 0.0;
	return analogRead(voltage_pins[number])*0.029325513;
	// int a = 0;
	// const int iteritions = 3;
	// for (int i = 0; i<iteritions; i++) {
	// 	a += analogRead(voltage_pins[number]);
	// 	delay(100);
	// }
	// return float(a)/iteritions*0.029325513;
	
}

// ========================= SERIAL ==============================

#include "serialParser.h"
void sendDataToSerial() {
  Serial.println();
  Serial.println("GY25: 0 0 0 0");
  Serial.print("ENC: ");
  Serial.print(enc1.get());
  Serial.print(" ");
  Serial.print(enc2.get());
  Serial.print(" ");
  Serial.print(enc3.get());
  Serial.print(" ");
  Serial.println(enc4.get());
  Serial.print("VOLTAGE: ");
  Serial.print(getVoltage(1));
  Serial.print(" ");
  Serial.println(getVoltage(2));
}

long int timer_for_ros_data_send = 0;
void serialROSEnc() { 
  // Выводим данные раз в 100 мс, не мешая вводу
  if (millis() - timer_for_ros_data_send >= 100) {
    timer_for_ros_data_send = millis();
    Serial.print("ENC: ");
		Serial.print(enc1.get());
		Serial.print(" ");
		Serial.print(enc2.get());
		Serial.print(" ");
		Serial.print(enc3.get());
		Serial.print(" ");
		Serial.println(enc4.get());
  }
}

// ========================= CODE ==============================

void setupRobot() {
	Serial.begin(9600);
	Motors.setup();
	// gy25.setup();
	// gy25.calibration();
	// Serial2.begin(115200); // ПОТОМУ ЧТО ТУПАЯ АРДУИНА 
	// for (uint8_t i = 0; i < servoCount; i++) {
	// 	servos[i].attach(servoPins[i]);
	// 	servos[i].write(90);
	// 	gy25.update();
	// }
	enc1.setup(A0,A1);
	enc2.setup(A2,A3);
	enc3.setup(13,12);
	enc4.setup(10,11);
	// setupEncoders();
	delay(1000);
	// gy25.update();
	Serial.println("System ready!");
}

// void smoothMoveServo(int servoNum, int targetAngle, int speed=35) {
// 	if (servoNum < 0 || servoNum >= SERVO_QUANTITY) {
// 		return;
// 	}
// 	targetAngle = constrain(targetAngle, 0, 180);
// 	int startAngle = currentPositions[servoNum];
// 	if (targetAngle > startAngle) {
// 		for (int angle = startAngle; angle <= targetAngle; angle++) {
// 			servos[servoNum].write(angle);
// 			currentPositions[servoNum] = angle;
// 			delay(speed);
// 			gy25.update();
// 		}
// 	} else {
// 		for (int angle = startAngle; angle >= targetAngle; angle--) {
// 			Serial.println(speed);
// 			servos[servoNum].write(angle);
// 			currentPositions[servoNum] = angle;
// 			delay(speed);
// 			gy25.update();
// 		}
// 	}
// }

// void brushesOn() {
//   gy25.delayUpdate(1000);
//   Motors.run(4, 100);
//   gy25.delayUpdate(1000);
// }
// void brushesOff() {
//   gy25.delayUpdate(1000);
//   Motors.run(4, 0);
//   gy25.delayUpdate(1000);
// }

int translateSpeedFronCmSecToParrotSec(long int speed) {
	speed *= PARROT_IN_ONE_FULL_ROTATION_WHEEL;
	return speed/(3.14*DIAMETER_WHEEL); 
}

int motorTargetRealSpeedEnc[5] = {0, 0, 0, 0, 0};
int motorTargetSpeedEnc[5] = {0, 0, 0, 0, 0};
double motorDifferencial[5] = {0, 0, 0, 0, 0};
// int motorTargetSpeedEnc[3] = {0, 0, 0};

// P-регулятор скорости по энкодерам.
// number: 1 или 2 — номер мотора.
// speed: желаемая скорость в единицах "импульсов энкодера в секунду".
// Функция должна вызываться часто; она самостоятельно поддерживает текущее
// управляющее значение для мотора. Для простоты используется пропорциональный
// коэффициент MOT_KP и ограничение по допустимой мощности мотора.
void motorTargetSpeedEncRun(int number, int speed) {
	if (number != 1 && number != 2 && number != 3 && number != 4) return;

	const double MOT_KP = 0.005; // коэффициент П-регулятора (можно подстроить)
	const double MOT_KD = 0.05;
	const double MOT_KI = 0;

	const int MOTOR_MAX_OUT = 100; // ограничение выхода

	static long lastEnc[5] = {0, 0, 0, 0, 0};
	static unsigned long lastTime[5] = {0, 0, 0, 0, 0};
	static int motorOut[5] = {0, 0, 0, 0, 0};
	// motorTargetSpeedEnc[number] = speed;
	// motorTargetRealSpeedEnc[number] = motorTargetSpeedEnc[number];

	unsigned long now = millis();

	long curEnc = 0;
	if (number == 1) curEnc = enc1.get();
	if (number == 2) curEnc = enc2.get();
	if (number == 3) curEnc = enc3.get();
	if (number == 4) curEnc = enc4.get();
	
	// Инициализация при первом вызове для данного мотора
	if (lastTime[number] == 0) {
			lastTime[number] = now;
			lastEnc[number] = curEnc;
			// если требуется стартовая установка мощности, используем текущую motorOut (0)
	}

	unsigned long dt = now - lastTime[number];
	if (dt < 20) { // слишком мало времени — ждём накопления импульсов
			return;
	}

	long dEnc = curEnc - lastEnc[number];
	// измеренная скорость в импульсах в секунду
	float measured = (float)dEnc * 1000.0f / (float)dt;
	// Serial.print(measured);
	// Serial.print(" ");

	// ошибка в тех же единицах, что и параметр speed
	double e = (float)speed - measured;
	double d = e - motorDifferencial[number];
	motorDifferencial[number] = e;
	int deltaOut = MOT_KP*e + MOT_KD*d;

	// обновляем выход и ограничиваем
	int out = motorOut[number] + deltaOut;
	out = constrain(out,-MOTOR_MAX_OUT,MOTOR_MAX_OUT);
	if (motorTargetRealSpeedEnc[number]==0 && measured==0) out = 0;

	// Если целевая скорость 0, делать мягкую остановку — уменьшать к нулю
	// if (speed == 0 && abs(out) < 3) out = 0;

	// Применяем к мотору и сохраняем состояние
	// Serial.println(out);
	Motors.run(number, out);
	motorOut[number] = out;

	// сохранить для следующего шага
	lastEnc[number] = curEnc;
	lastTime[number] = now;
}

void motorCreateTargetSpeedEncRun(int number, int speed=0) {
	if (number != 1 && number != 2 && number != 3 && number != 4) return;
	motorTargetSpeedEnc[number] = translateSpeedFronCmSecToParrotSec(speed);
}

void motorCreateTargetSpeedEncRun2(int m1, int m2, int m3, int m4) {
	motorTargetSpeedEnc[1] = translateSpeedFronCmSecToParrotSec(m1);
	motorTargetSpeedEnc[2] = translateSpeedFronCmSecToParrotSec(m2);
	motorTargetSpeedEnc[3] = translateSpeedFronCmSecToParrotSec(m3);
	motorTargetSpeedEnc[4] = translateSpeedFronCmSecToParrotSec(m4);
}

const int MOTOR_ENC_A = 10; // задержка по времени между ускорениями на единичку по скорости 
const int MOTOR_ENC_DENC = 60; // задержка по времени между ускорениями на единичку по скорости 

void motorSpeedEncUpdateNumber(int number) {
	if (number != 1 && number != 2 && number != 3 && number != 4) return;
	static unsigned long lastTime2[5] = {0, 0, 0, 0, 0};
	unsigned long now = millis();
	// Инициализация при первом вызове для данного мотора
	if (lastTime2[number] == 0) lastTime2[number] = now;
	if (now-lastTime2[number]>MOTOR_ENC_A) {
		if (abs(motorTargetRealSpeedEnc[number]-motorTargetSpeedEnc[number])>MOTOR_ENC_DENC) {
			if (motorTargetRealSpeedEnc[number]>motorTargetSpeedEnc[number]) motorTargetRealSpeedEnc[number] -= MOTOR_ENC_DENC;
			if (motorTargetRealSpeedEnc[number]<motorTargetSpeedEnc[number]) motorTargetRealSpeedEnc[number] += MOTOR_ENC_DENC;
		}
		else if (abs(motorTargetRealSpeedEnc[number])<MOTOR_ENC_DENC+5) motorTargetRealSpeedEnc[number] = 0;
		lastTime2[number] = now;
	}
	// run motor
	motorTargetSpeedEncRun(number,motorTargetRealSpeedEnc[number]);
}


void motorSpeedEncUpdate() {
	motorSpeedEncUpdateNumber(1);
	motorSpeedEncUpdateNumber(2);
	motorSpeedEncUpdateNumber(3);
	motorSpeedEncUpdateNumber(4);
	// for (int i = 0; i<5; i++) Serial.print(String(motorTargetSpeedEnc[i]) + " ");
	// Serial.println();
}

// void testErrorEncDistanseRun(int a) {
//   motorCreateTargetSpeedEncRun2(a,a);
//   for (long int t = millis() + 4000; t>millis();) {
//     motorSpeedEncUpdate();
//   }
//   enc1.clear();
//   enc2.clear();
//   enc3.clear();
//   enc4.clear();
//   motorCreateTargetSpeedEncRun2(0,0);
//   for (long int t = millis() + 2000; t>millis();) {
//     motorSpeedEncUpdate();
//   }
//   Serial.print(enc1_count);
//   Serial.print(" ");
//   Serial.println(enc2_count);
// }

