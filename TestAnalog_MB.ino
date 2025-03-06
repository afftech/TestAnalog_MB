//                      RX    TX
HardwareSerial Serial3(PA3, PA2);
//HardwareSerial SerialPort(1);  //2  // use UART2
//01 04 03 E7 00 0C      40 7C

//1 4 3 E7 0C 40 7C
//143E70C407C

//1418 10D012E712B3122E132A13E13E81216DAFD36D1BB7ED9
int size_data;
int value[12];
byte command[8] = { 0x01, 0x04, 0x03, 0xE7, 0x00, 0x0C, 0x00, 0x00 };
byte response[30] = {};

int REG = 12;
int16_t VALUE[12];
int cursor;

uint32_t Time_Next_Command, Time_Wait_Data;

enum States {
  SEND_DATA,
  WAIT_DATA,
  PARSING_DATA
};
int state = SEND_DATA;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial3.begin(115200);
  //SerialPort.begin(115200);
  Serial.println("Start");
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (state) {
    case SEND_DATA:
      if (millis() - Time_Next_Command >= 10) {//10
        Time_Next_Command = millis();
        sendDataToUART();
        state = WAIT_DATA;
      }
      break;
    case WAIT_DATA:
      if (millis() - Time_Wait_Data >= 8) {
        Time_Wait_Data = millis();
        size_data = available();
        state = PARSING_DATA;
      }
      break;
    case PARSING_DATA:
      PARSING(size_data);
      state = SEND_DATA;
      break;
  }
}

int available() {
  int size = 0;
  while (Serial3.available() > 0) {
    uint8_t test;
    test = Serial3.read();
    //Serial.print(test, HEX);
    response[size++] = test;
  }
  if (size != 0) {
    return size;
  }
  return 0;
}

void PARSING(int size) {
  word crc = modbus_crc16(response, size - 2);
  if ((lowByte(crc) == response[size - 2]) && (highByte(crc) == response[size - 1])) {
    byte value_pos = 0;
    for (byte i = 3; i < size - 2; i++) {
      if (!(i % 2 == 0)) {
        value[value_pos] = response[i] << 8;
      } else {
        value[value_pos] = value[value_pos] | response[i];
        /*Serial.print(value[value_pos]);
      Serial.print(" ");*/
        value_pos++;
      }
      // Serial.print(" ");
      //Serial.print(response[i], HEX);
    }
    Serial.println();
    for (byte i = 0; i < 12; i++) {
      Serial.print(" ");
      Serial.print(value[i]);
    }
    Serial.println();
  }
}

void sendDataToUART() {
  byte len;
  word crc = modbus_crc16(command, 6);
  command[6] = lowByte(crc);
  command[7] = highByte(crc);
  Serial.print("-->");
  len = 8;
  for (byte i = 0; i < len; i++) {
    Serial.print(command[i], HEX);
    Serial.print(" ");

    Serial3.write(command[i]);
  }
  Serial.println();
}

word modbus_crc16(byte* buf, int len) {
  word crc = 0xFFFF;
  for (int pos = 0; pos < len; pos++) {
    crc ^= (word)buf[pos];  // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--) {  // Loop over each bit
      if ((crc & 0x0001) != 0) {    // If the LSB is set
        crc >>= 1;                  // Shift right and XOR 0xA001
        crc ^= 0xA001;
      } else        // Else LSB is not set
        crc >>= 1;  // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}