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

	// Begin(&Serial) means debug mode, will output messages using Serial
	// Use fingerBoard.Begin() to begin without debug message output

	FbStatus status = fingerBoard.Begin(&Serial); 

	if (status == FbStatus::ERROR_SENSOR_NOT_FOUND)
	{
		Serial.println("Did not find fingerprint sensor :(");
		while (true);
	}
}

void loop()
{
	// Serial CMD routine...
	fingerBoard.CmdCheck();

	int id = fingerBoard.GetFingerID();

	switch (id)
	{
	case 1: case 2: case 3:
		fingerBoard.InputPassword("kj7ooim");
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
		fingerBoard.TypeString("www.pengzhihui.com", true);

		// The keyboard will press ctrl+v and then release.
		fingerBoard.PressKey(KEY_LEFT_CTRL);
		fingerBoard.PressKey('v');
		fingerBoard.ReleaseAll();
		 
		*/
	}
}