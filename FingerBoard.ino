/***************************************************
  This is a sample code for my Fingerprint-Keyboard
  Designed specifically to work with the FPM3X fingerprint sensor
  ----> https://item.taobao.com/item.htm?spm=a230r.1.14.16.1d0f7d23CO42c0&id=570338891909

  The Arduino board used here must be Arduino Micro (or other Mega32U4 chip board)
  Arduino     FPM3X
  Tx1 <-----> Rx
  Rx1 <-----> Tx

  more detail please visit this page:
  ---->  https://zhuanlan.zhihu.com/p/64809151

  @Pengzhihui 2019-5-13
 ****************************************************/

#include "fingerprintWrapper.h"

FingerBoard fingerBoard(&Serial1);

void setup()
{
  Serial.begin(115200);

  FbStatus status = fingerBoard.Begin(&Serial);

  if (status == FbStatus::ERROR_SENSOR_NOT_FOUND)
  {
    Serial.println("Did not find fingerprint sensor :(");
    while (true);
  }
}

void loop()
{
  CmdCheck();

  int id = fingerBoard.GetFingerID();

  switch (id)
  {
  case 1: case 2: case 3:
    fingerBoard.InputPassword("299792458Microsoft");
    break;

  case 4: case 5: case 6:
    fingerBoard.InputPassword("299792458@TT");
    break;

  case 7: case 8: case 9:
    fingerBoard.InputPassword("299792458");
    break;

    /* 

    you can also use like this:

    // The keyboard will type the string and perform an enter.
    fingerBoard.InputString("www.pengzhihui.com", true);

    // The keyboard will press ctrl+v and then release.
    fingerBoard.Press(KEY_LEFT_CTRL);
    fingerBoard.Press('v');
    fingerBoard.Release();

    */
  }

  delay(300);
}


void CmdCheck()
{
  if (Serial.available() > 0)
  {
    char c = Serial.read();

    switch (c)
    {
    case  'A':  //Add Finger
      while (true)
      {
        Serial.println("Please enter the ID you want to enroll:");

        while (Serial.available() <= 0);
        int p = Serial.parseInt();

        if (p >= 0 && p < 10)
        {
          Serial.print("Enrolling [");
          Serial.print(p);
          Serial.println("] finger...");

          if (fingerBoard.AddFinger(p))
          {
            Serial.println("-------------------");
            Serial.println("Add ok !");
            Serial.println("-------------------");
          }
          else
          {
            Serial.println("-------------------");
            Serial.println("Add failed !");
            Serial.println("-------------------");
          }

          break;
        }
        else
        {
          Serial.println("Please enter a valid ID !");
        }
      }

      break;
    case 'D': //Delete all fingers
      fingerBoard.DeleteAllFingers();
      Serial.println("All Finger deleted !");
      break;
    }
  }
}
