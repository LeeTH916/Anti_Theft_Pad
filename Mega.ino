#include <SoftwareSerial.h>
int garo[23] = {31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53};
int sero[10] = {A9, A8, A7, A6, A5, A4, A3, A2, A1, A0};
int mode_flag = 0;

int prev[2][23][10];

int sound = 3;
int tones = 277;
int weight[23][10];

int flag = 0;
volatile char cnt[23][10];
int sound_delay = 0;

void setup() {
  for (int i = 0; i < 10; i++) {
    pinMode(sero[i], INPUT_PULLUP);
  }
  initial();
  Serial3.begin(115200);
  Serial.begin(230400);
}

void loop() {
  if (mode_flag == 0) {
    Wait_WifiConnect();
  }
  else if (mode_flag == 1) {
    Wait_Start();
  }
  else if (mode_flag == 2) {
    Test_mode();
  }
  else if (mode_flag == 3) { // 사용자가 도난기능을 ON한 상태
    
    AntiTheftMode();
    delay(80);
  }
  else if (mode_flag == 4) {
    Buzzer();
  }
}

void initial() {
  delay(500);
  for (int k = 0; k < 3; k++) {
    for (int i = 0; i < 23; i++) {
      pinMode(garo[i], OUTPUT);
      digitalWrite(garo[i], LOW);
      for (int j = 0; j < 10; j++) {
        if (k == 2) {
          int cur = analogRead(sero[j]);
          weight[i][j] = middle_value(prev[0][i][j], prev[1][i][j], cur);
          cnt[i][j] = 0;
        }
        else {
          prev[k][i][j] = analogRead(sero[j]);
        }
      }
      pinMode(garo[i], INPUT);
    }
  }
  sound_delay = 0;
  flag = 0;
}

int middle_value(int a, int b, int c) {
  if (a >= b) {
    if (b >= c) {
      return b;
    }
    else if (a <= c) {
      return a;
    }
    else {
      return c;
    }
  }
  else if (a >= c) {
    return a;
  }
  else if (b >= c) {
    return c;
  }
  else {
    return b;
  }
}

void Wait_WifiConnect() {
  if (Serial3.available()) {
    String mode_tmp = Serial3.readString();
    if (mode_tmp == "WifiConnected") {
      mode_flag = 1;
    }
  }
}

void Wait_Start() {
  for (int i = 0; i < 23; i++) {
    pinMode(garo[i], OUTPUT);
    digitalWrite(garo[i], LOW);
    for (int j = 0; j < 10; j++) {
      weight[i][j] = analogRead(sero[j]);
    }
    pinMode(garo[i], INPUT);
  }
  if (Serial3.available()) {
    String mode_tmp = Serial3.readString();
    if (mode_tmp == "Detection_start") {
      mode_flag = 3;
    }

    if (mode_tmp == "Test_Mode") {
      mode_flag = 2;
    }
  }

}

void Test_mode() {
  for (int i = 0; i < 23; i++) {
    pinMode(garo[i], OUTPUT);
    digitalWrite(garo[i], LOW);
    for (int j = 0; j < 10; j++) {
      weight[i][j] = analogRead(sero[j]);
    }
    pinMode(garo[i], INPUT);
  }
  if (Serial3.available()) {
    String mode_tmp = Serial3.readString();
    if (mode_tmp == "Detection_start") {
        delay(500);
  for (int k = 0; k < 3; k++) {
    for (int i = 0; i < 23; i++) {
      pinMode(garo[i], OUTPUT);
      digitalWrite(garo[i], LOW);
      for (int j = 0; j < 10; j++) {
        if (k == 2) {
          int cur = analogRead(sero[j]);
          weight[i][j] = middle_value(prev[0][i][j], prev[1][i][j], cur);
          cnt[i][j] = 0;
        }
        else {
          prev[k][i][j] = analogRead(sero[j]);
        }
      }
      pinMode(garo[i], INPUT);
    }
  }
      mode_flag = 3;
      return;
    }
  }
  print_weight();
}



void AntiTheftMode() {
//
//
  for (int i = 0; i < 23; i++) {
    pinMode(garo[i], OUTPUT);

    for (int j = 0; j < 10; j++) {
      bool compare_check = check(i, j);
      if (compare_check == true) {
        mode_flag = 4;
        Serial3.print("t");
        print_weight();
        pinMode(garo[i], INPUT);
        return;
      }
    }
    pinMode(garo[i], INPUT);
  }


  if (Serial3.available()) {
    String mode_tmp = Serial3.readString();
    if (mode_tmp == "Reset") {
      mode_flag = 1;
      initial();
      return;
    }
  }

  print_weight();
}

bool check_again(int garo_num, int sero_num, int read_v )
{
  int read_voltage = analogRead(sero[sero_num]);
  int Compare_voltage = weight[garo_num][sero_num];
  int temp_cnt = 0;

  for (int i = 0; i < 1000; i++)
  {
    read_voltage = analogRead(sero[sero_num]);
    if (  !(read_voltage > Compare_voltage + 200  || read_voltage < Compare_voltage - 200) )
    {
      temp_cnt++;
      if (temp_cnt == 10)
      {
        //weight[garo_num][sero_num] = read_voltage;
        cnt[garo_num][sero_num] = 0;
        return false;
      }
    }
    else {
      temp_cnt = 0;
    }
  }
  return true;
}

bool check_update(int volt , int garo_num, int sero_num)
{
  int read_voltage;
  int cnt_volt = 0;
  for (int i = 0; i < 22; i++)
  {
    read_voltage = analogRead(sero[sero_num]);
    if ( (read_voltage < volt + 50 && read_voltage > volt - 50) )
    {
      cnt_volt++;
    }
  }
  if (cnt_volt < 20)
  {
    return false;
  }
  return true;
}

bool check(int garo_num, int sero_num) {
  int read_voltage = analogRead(sero[sero_num]);
  int Compare_voltage = weight[garo_num][sero_num];

  if ( read_voltage > Compare_voltage + 300  || read_voltage < Compare_voltage - 300 ) {
    cnt[garo_num][sero_num] += 1;

    if (cnt[garo_num][sero_num] == 1 ) {

      if (!check_again(garo_num , sero_num , read_voltage))
      {
        return false;
      }
      return true;
    }
    return false;
  }
  else {
    if (check_update(read_voltage, garo_num, sero_num))
    {
      weight[garo_num][sero_num] = middle_value(prev[0][garo_num][sero_num], prev[1][garo_num][sero_num], read_voltage);
      prev[0][garo_num][sero_num] = prev[1][garo_num][sero_num];
      prev[1][garo_num][sero_num] = read_voltage;
    }
    cnt[garo_num][sero_num] = 0;
    return false;
  }
}

void print_weight()
{
  for (int i = 0 ; i < 10 ; i++) {
    for (int j = 0; j < 23 ; j++) {
      //Serial.print((int)cnt[j][i]);
      Serial.print(weight[j][i]);
      if (i == 9 && j == 22) continue;
      Serial.print(",");
    }
    //Serial.println();
  }
  Serial.println();
}

void Buzzer() {

  if (sound_delay <= 20 )
  {
    pinMode(sound, OUTPUT);
    tone(sound, tones);
    sound_delay++;
    delay(100);
  }
  else
  {
    pinMode(3, INPUT);
  }
  if (Serial3.available()) {
    String mode_tmp = Serial3.readString();
    if (mode_tmp == "Reset") {
      pinMode(sound, INPUT);
      mode_flag = 1;
      initial();
    }
  }
}
