#pragma once

struct SerialData {
  char mode;
  int data_1;
  int data_2;
  int data_3;
  int data_4;
  int data_counter;
};

SerialData readSerialData() {
  SerialData result;
  result.mode = '\0';
  result.data_1 = 0;
  result.data_2 = 0;
  result.data_3 = 0;
  result.data_4 = 0;
  result.data_counter = 0;

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      // Первый символ — режим
      result.mode = input.charAt(0);

      // Удаляем режим из строки
      input = input.substring(1);
      input.trim();

      int values[4] = {0, 0, 0, 0};
      int index = 0;

      while (input.length() > 0 && index < 4) {
        int spacePos = input.indexOf(' ');
        String token;

        if (spacePos != -1) {
          token = input.substring(0, spacePos);
          input = input.substring(spacePos + 1);
        } else {
          token = input;
          input = "";
        }

        token.trim();
        if (token.length() > 0) {
          values[index] = token.toInt();
          result.data_counter++;
          index++;
        }
      }

      result.data_1 = values[0];
      result.data_2 = values[1];
      result.data_3 = values[2];
      result.data_4 = values[3];
    }
  }

  return result;
}


void testSerialParser() {
  SerialData data = readSerialData();

  if (data.mode != '\0') {
    Serial.print("Режим: ");
    Serial.println(data.mode);

    Serial.print("Данные 1: ");
    Serial.println(data.data_1);

    Serial.print("Данные 2: ");
    Serial.println(data.data_2);

    Serial.print("Данные 3: ");
    Serial.println(data.data_3);

    Serial.print("Данные 4: ");
    Serial.println(data.data_4);

    Serial.print("Количество чисел: ");
    Serial.println(data.data_counter);

    Serial.println("---");
  }
}

