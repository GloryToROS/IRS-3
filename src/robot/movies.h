// Файл для функций движения робота
#pragma once

// #define GY25_STRAFE_ANDLE 1
// #define GY25_STRAFE_DT 700
#include "robot.h"

// #define ENC_POROG 50
// #define ENC_TIME 500
// #define ENC_GYRO_TURN_POROG 3
// #define ENC_FORWARD_KP 0.5
// #define ENC_FORWARD_KD 10
// #define ENC_FORWARD_ALIGNMENT_KP 20 // выравнивание колес друг относительно друга 
// #define ENC_TURN_KP 5.0
// #define ENC_TURN_KD 4

// #define ENC_GYRO_FORWARD_KP 2 // 50
// #define ENC_GYRO_FORWARD_KD 60 // 50
// #define ENC_GYRO_FORWARD_UPDATE_DT 20 // 50

// #define GYRO_TURN_KP 5 // 5
// #define GYRO_TURN_KD 80 // 40
#define ENC_ANGLE_TO_PARROT 18.1 // 17
#define ENC_CM_TO_PARROT 122.6 //120
#define ENC_MOTOR_MAX_SPEED 70 // 70
#define ENC_MOTOR_MAX_SPEED_TURN 70
#define ENC_MOTOR_R_BOOST 1 //1.07 // ОН ОТВЕЧАЕТ ЗА ЛЕВЫЙ МОТОР!!!

// void runGyro(long int forward=0) {
// 	const int ENC_GYRO_BASIC_SPEED = 4000;
// 	long int enc_target = (enc1_count+enc2_count)/2 + forward * ENC_CM_TO_PARROT;
// 	long int e_old = 0;
// 	gy25.update();
// 	long int gyro_target = gy25.horizontal_angle_strafe; 
// 	motorCreateTargetSpeedEncRun2(ENC_GYRO_BASIC_SPEED,ENC_GYRO_BASIC_SPEED);
// 	int serial_e_for_gyro = 0;
// 	// gy25.setupStrafe();
//   // Serial.println(abs((enc1_count+enc2_count)/2 - enc_target));
// 	long int gyro_p_general = 0;
// 	long int gyro_update_timer = millis() + ENC_GYRO_FORWARD_UPDATE_DT;
// 	long int gyro_d_old = 0;
//   while (abs((enc1_count+enc2_count)/2 - enc_target) > 1450) {
// 		// serial
// 		SerialData data = readSerialData();
//   	if (data.mode != '\0') {
// 			if      (data.mode=='S' && data.data_counter==0) break;
// 			else if (data.mode=='e' && data.data_counter==1) serial_e_for_gyro = data.data_1;
// 			else if (data.mode=='g' && data.data_counter==0) sendDataToSerial();
// 			else Serial.println("ERROR: unknow command");
// 		}
//     // проверить на буксирование (дописать потом)
//     // gyro
// 		gy25.update();
// 		if (gyro_update_timer<millis()) {
// 			long int e_gyro = gy25.horizontal_angle_strafe - gyro_target + serial_e_for_gyro;
// 			long int p_gyro = e_gyro * ENC_GYRO_FORWARD_KP;
// 			long int d_gyro = e_gyro-gyro_d_old;
// 			d_gyro *= ENC_GYRO_FORWARD_KD;
// 			gyro_d_old = e_gyro;
// 			gyro_p_general += p_gyro + d_gyro;
// 			// Serial.println(gyro_p_general);
// 			gyro_update_timer = ENC_GYRO_FORWARD_UPDATE_DT + millis();
// 			gyro_p_general = constrain(gyro_p_general,-ENC_GYRO_BASIC_SPEED,ENC_GYRO_BASIC_SPEED);
// 		}
// 	  motorCreateTargetSpeedEncRun2(ENC_GYRO_BASIC_SPEED+gyro_p_general,ENC_GYRO_BASIC_SPEED-gyro_p_general);
// 		// move
//     motorSpeedEncUpdate();
// 	}
//   motorCreateTargetSpeedEncRun2(0,0);
// 	for (long int t = millis()+1500; t>millis();) {
// 		motorSpeedEncUpdate();
// 		gy25.update();
// 	}
// 	Serial.println("END COMMAND: forward - success");
// }

// void turnGyro(long int right=0) {
// 	long int time = millis() + ENC_TIME;
// 	long int e_old = 0;
// 	gy25.update();
// 	long int gyro_target = gy25.horizontal_angle_strafe - right; //  + 5
// 	//enc_strafe_timer = millis() + GYRO_STRAFE_DT;
// 	// gy25.setupStrafe();
// 	const int	GY_DATA_SIZE = 5; 
// 	long int gy_array_data[GY_DATA_SIZE];
// 	for (int i = 0; i < GY_DATA_SIZE; i++) gy_array_data[i] = gy25.horizontal_angle_strafe;
// 	int serial_e_for_gyro = 0;
// 	while (time > millis()) {
// 		// serial
// 		SerialData data = readSerialData();
//   	if (data.mode != '\0') {
// 			if      (data.mode=='S' && data.data_counter==0) break;
// 			else if (data.mode=='e' && data.data_counter==1) serial_e_for_gyro = data.data_1;
// 			else if (data.mode=='g' && data.data_counter==0) sendDataToSerial();
// 			else Serial.println("ERROR: unknow command");
// 		}
// 		// GY-25
// 		gy25.update();
// 		double gy_data = 0;
// 		for (int i = 0; i < GY_DATA_SIZE-1; i++) {
// 			gy_array_data[i] = gy_array_data[i+1];
// 			gy_data += gy_array_data[i];
// 		}
// 		gy_array_data[GY_DATA_SIZE-1] = gy25.horizontal_angle_strafe;
// 		gy_data += gy_array_data[GY_DATA_SIZE-1];
// 		gy_data /= double(GY_DATA_SIZE);
// 		// test end
// 		if ((abs(gy_data - gyro_target) > ENC_GYRO_TURN_POROG)) {
// 			time = millis() + ENC_TIME;
// 		}
// 		long int e = gy_data - gyro_target + serial_e_for_gyro;
// 		long int p = e;
// 		long int d = e - e_old;
// 		e_old = e;
// 		p *= GYRO_TURN_KP;
// 		d *= GYRO_TURN_KD;
// 		long int m1 = constrain(p + d, -ENC_MOTOR_MAX_SPEED_TURN, ENC_MOTOR_MAX_SPEED_TURN);
// 		long int m2 = -constrain(p + d, -ENC_MOTOR_MAX_SPEED_TURN, ENC_MOTOR_MAX_SPEED_TURN);
// 		Motors.run(1, m1);
// 		Motors.run(2, m2);
// 	}
// 	Motors.run(1, 0);
// 	Motors.run(2, 0);
// 	Serial.println("END COMMAND: turn - success");
// }

// void turnGyro2(long int right=0) {
// 	const int GYRO_TURNING_MAX_SPEED = 4000;
// 	const int GYRO_TURNING_KP = 100;
// 	// int left_speed = ENC_GYRO_BASIC_SPEED;
// 	// int right_speed = ENC_GYRO_BASIC_SPEED;
// 	// if (right>0) right_speed *= -1;
// 	// else left_speed *= -1;
// 	// long int time = millis() + ENC_TIME;
// 	// long int e_old = 0;
// 	gy25.update();
// 	long int gyro_target = gy25.horizontal_angle_strafe - right;
// 	// gy25.setupStrafe();
// 	// motorCreateTargetSpeedEncRun2(ENC_GYRO_BASIC_SPEED,ENC_GYRO_BASIC_SPEED);
// 	int serial_e_for_gyro = 0;
// 	//while (time > millis()) {
// 	while (abs(gy25.horizontal_angle_strafe - gyro_target) > ENC_GYRO_TURN_POROG) {
// 		// serial
// 		SerialData data = readSerialData();
//   	if (data.mode != '\0') {
// 			if      (data.mode=='S' && data.data_counter==0) break;
// 			else if (data.mode=='e' && data.data_counter==1) serial_e_for_gyro = data.data_1;
// 			else if (data.mode=='g' && data.data_counter==0) sendDataToSerial();
// 			else Serial.println("ERROR: unknow command");
// 		}
// 		// gy-25
// 		gy25.update();
		
// 		// if () {
// 		// 	time = millis() + ENC_TIME;
// 		// }
// 		long int e = gy25.horizontal_angle_strafe - gyro_target + serial_e_for_gyro;
// 		long int p = e;
// 		// long int d = e - e_old;
// 		// e_old = e;
// 		p *= GYRO_TURN_KP;
// 		// d *= GYRO_TURN_KD;
// 		// long int m1 = constrain(p + d, -ENC_MOTOR_MAX_SPEED_TURN, ENC_MOTOR_MAX_SPEED_TURN);
// 		// long int m2 = -constrain(p + d, -ENC_MOTOR_MAX_SPEED_TURN, ENC_MOTOR_MAX_SPEED_TURN);
// 		// Motors.run(1, m1);
// 		// Motors.run(2, m2);
		
// 		// move
//     motorCreateTargetSpeedEncRun2(constrain(p*GYRO_TURNING_KP,-GYRO_TURNING_MAX_SPEED,GYRO_TURNING_MAX_SPEED),
// 																	constrain(-p*GYRO_TURNING_KP,-GYRO_TURNING_MAX_SPEED,GYRO_TURNING_MAX_SPEED));
// 		motorSpeedEncUpdate();
// 	}
// 	motorCreateTargetSpeedEncRun2(0,0);
// 	// Motors.run(1, 0);
// 	// Motors.run(2, 0);
// 	Serial.println("END COMMAND: turn - success");
// }