#pragma once

void serialGyroParserToMotor() { 
  // Выводим данные раз в 100 мс, не мешая вводу
  if (millis() - gy25_lastPrintTime >= 100) {
    gy25_lastPrintTime = millis();
    Serial.print("e ");
    Serial.print(gy25.horizontal_angle);
    Serial.println();
    //
    Serial3.write("e ");
    int a = -gy25.horizontal_angle;
    char buffer[10]; // Буфер достаточного размера
    // Преобразуем число в строку
    itoa(a, buffer, 10); // 10 - десятичная система счисления
    // Выводим каждый символ через Serial.write()
    for(int i = 0; buffer[i] != '\0'; i++) {
      Serial3.write(buffer[i]);
    }
    Serial3.write('\n');
  }
}

void serialMotor() {
  if (Serial3.available()) {
    Serial.write(Serial3.read());
  }
  if (Serial.available()) {
    Serial3.write(Serial.read());
  }
}



void waitForCommandWithTimeout(unsigned long timeoutMs = 10000) {
  String receivedString = "";
  bool commandReceived = false;
  unsigned long startTime = millis();
  
  // Serial.println("Ожидание команды: END COMMAND: forward - success");
  // Serial.print("Таймаут: ");
  // Serial.print(timeoutMs / 1000);
  // Serial.println(" секунд");
  
  while (!commandReceived && (millis() - startTime < timeoutMs)) {
    if (Serial3.available() > 0) {
      receivedString = Serial3.readStringUntil('\n');
      receivedString.trim();
      // Serial.print("Получено: ");
      // Serial.println(receivedString);
      if (receivedString == "END COMMAND: forward - success") {
        commandReceived = true;
        // Serial.println("✓ Команда получена!");
      }
    }
    // delay(10);
    gy25.update();
  }
  // if (!commandReceived) {
  //   Serial.println("✗ Таймаут! Команда не получена.");
  // }
}
