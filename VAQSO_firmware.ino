#define fan0 10
#define fan1 9
#define fan2 6
#define fan3 5
#define fan4 3

int fans[5] = {fan0, fan1, fan2, fan3, fan4};
char inputCommand[5] = {0};
int fanStatus[5] = {0};
int sumOfHighFan = 0;
char input = 0;
int index = 0;

void setFans(int fanVal)
{
  for (int i = 0; i < 5; i++)
  {
    fanStatus[i] = 1;
  }
  return;
}

void onOnlyAFan(int fanNumber)
{
  //  Serial.println(fanNumber);
  for (int i = 0; i < 5; i++)
  {
    if (i == fanNumber)
      analogWrite(fans[i], (int)255 / 1); // 速度調節したければここを弄ろう
    else
      analogWrite(fans[i], 0);
  }
  return;
}

void resetFans()
{
  for (int i = 0; i < 5; i++)
  {
    fanStatus[i] = 0;
    analogWrite(fans[i], 0);
  }
  sumOfHighFan = 0;

  return;
}

void setup()
{
  Serial.begin(115200);
  pinMode(fan0, OUTPUT);
  pinMode(fan1, OUTPUT);
  pinMode(fan2, OUTPUT);
  pinMode(fan3, OUTPUT);
  pinMode(fan4, OUTPUT);
  analogWrite(fan0, 0); // 一応 PWM 出力するものとして考えている
  analogWrite(fan1, 0);
  analogWrite(fan2, 0);
  analogWrite(fan3, 0);
  analogWrite(fan4, 0);
  delay(1000);
}

void loop()
{
  if (Serial.available() > 0)
  {
    index = 0;
    memset(inputCommand, 0, 5); // 入力コマンドの初期化

    while (Serial.available() > 0)
    {
      input = 'A' + Serial.read() - 65; // char型として1byte読み込む
      inputCommand[index] = input;

      if (index >= 5)
        continue; // コマンドの文字数制限
      if (input == '\r' || input == '\n' || input == ' ')
        continue;
      index++;
    }
    Serial.println(inputCommand);

    if (index < 5)
    {
      Serial.println("too short length");
    } // out of indexを避ける
    else if (inputCommand[0] == 'f')
    { // fan10やfan11など
      int fanNum = int(inputCommand[3] - '0');
      int fanVal = int(inputCommand[4] - '0');
      int previousVal = fanStatus[fanNum];

      if (previousVal != fanVal)
      { // 以前に格納されていた値と違った場合のみsumやstatusを変更
        if (fanVal == 1)
          sumOfHighFan++;
        else if (fanVal == 0)
        {
          sumOfHighFan--;
          analogWrite(fans[fanNum], 0);
        }

        fanStatus[fanNum] = fanVal;
        Serial.write("fan");
        Serial.print(fanNum);
        Serial.println(" is changed");
      }
    }
    else if (inputCommand[0] == 'r')
    { // ファンの状態をすべて初期化する
      resetFans();
      Serial.println("status is reset");
    }
    else if (inputCommand[0] == 'a')
    {
      int fanNum = int(inputCommand[4] - '0');
      if (fanNum == 0)
      {
        resetFans();
      }
      else if (fanNum == 1)
      {
        setFans(HIGH);
        sumOfHighFan = 5;
        Serial.println("all turned on");
      }
    }

    for (int i = 0; i < 5; i++)
    {
      Serial.print(fanStatus[i]); 
    }
  }

  int onTimeMS = (int)100 / sumOfHighFan; // 割り込み処理として書き直したい
  for (int i = 0; i < 5; i++)
  { // HIGHの個数に合わせた長さでファンを動作
    if (fanStatus[i] == 1)
    {
      onOnlyAFan(i);
      delay(onTimeMS);
    }
  }
}
