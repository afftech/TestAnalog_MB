#define Enginr_on_1 D12  //D10
#define Enginr_on_2 D13  //D12
#define Enginr_on_3 D14  //D13

uint32_t A_R[] = { 0, 0, 0 };  //A_R[] = { A_3, 0, 0 };
int find = 3;
int Engine1S;
int Engine2S;
int Engine3S;

void analogWriteR(uint32_t name, int data) {
  bool Rev_state = 0;
  for (int i = 0; i < find; i++) {
    if (A_R[i] == name) {
      Rev_state = 1;
    }
  }

  if (Rev_state) {
    analogWrite(name, map(data, 0, 255, 255, 0));
  } else {
    analogWrite(name, data);
  }
  switch (name) {
    case A_1:
      Engine1S = data;
      break;
    case A_2:
      Engine2S = data;
      break;
    case A_3:
      Engine3S = data;
      break;
  }
}
void StateAnalog() {
  if (Engine1S > 0) {
    digitalWrite(Enginr_on_1, 1);
  } else {
    digitalWrite(Enginr_on_1, 0);
  }
  if (Engine2S > 0) {
    digitalWrite(Enginr_on_2, 1);
  } else {
    digitalWrite(Enginr_on_2, 0);
  }
  if (Engine3S > 0) {
    digitalWrite(Enginr_on_3, 1);
  } else {
    digitalWrite(Enginr_on_3, 0);
  }
}
