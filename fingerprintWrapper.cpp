#include "fingerprintWrapper.h"

FingerBoard::FingerBoard(HardwareSerial* hs)
{
	sensor = Fingerprint(&Serial1);
	sensor.begin(57600);

	sensorSerial = hs;

	Keyboard.begin();
}

FbStatus FingerBoard::Begin()
{
	FbStatus ret = FbStatus::SUCCESS;

	if (sensor.verifyPassword())
		ret = FbStatus::SUCCESS;
	else
		ret = FbStatus::ERROR_SENSOR_NOT_FOUND;

	return ret;
}

FbStatus FingerBoard::Begin(Serial_* hs)
{
	debug = hs;

	return Begin();
}

int FingerBoard::GetFingerID()
{
	uint8_t p = sensor.getImage();

	switch (p)
	{
	case FINGERPRINT_OK:
		if (debug)
			debug->println("Image taken");
		break;
	case FINGERPRINT_NOFINGER:
		isTouching = false;
		return -1;
	case FINGERPRINT_PACKETRECIEVEERR:
		Serial.println("Communication error");
		return -1;
	case FINGERPRINT_IMAGEFAIL:
		if (debug)
			debug->println("Imaging error");
		return -1;
	default:
		if (debug)
			debug->println("Unknown error");
		return -1;
	}

	// OK success!

	p = sensor.image2Tz();
	switch (p)
	{
	case FINGERPRINT_OK:
		if (debug)
			debug->println("Image converted");
		break;
	case FINGERPRINT_IMAGEMESS:
		if (debug)
			debug->println("Image too messy");
		return -1;
	case FINGERPRINT_PACKETRECIEVEERR:
		if (debug)
			debug->println("Communication error");
		return -1;
	case FINGERPRINT_FEATUREFAIL:
		if (debug)
			debug->println("Could not find fingerprint features");
		return -1;
	case FINGERPRINT_INVALIDIMAGE:
		if (debug)
			debug->println("Could not find fingerprint features");
		return -1;
	default:
		if (debug)
			debug->println("Unknown error");
		return -1;
	}

	// OK converted!
	p = sensor.fingerFastSearch();
	if (p == FINGERPRINT_OK)
	{
		if (debug)
			debug->println("Found a print match!");
	}
	else if (p == FINGERPRINT_PACKETRECIEVEERR)
	{
		if (debug)
			debug->println("Communication error");
		return -1;
	}
	else if (p == FINGERPRINT_NOTFOUND)
	{
		if (debug)
			debug->println("Did not find a match");
		return -1;
	}
	else
	{
		if (debug)
			debug->println("Unknown error");
		return -1;
	}

	// found a match!
	if (debug)
	{
		debug->print("Found ID #");
		debug->print(sensor.fingerID);

		debug->print(" with confidence of ");
		debug->println(sensor.confidence);
	}

	return sensor.fingerID;
}

bool FingerBoard::AddFinger(unsigned int id)
{
	int FAIL_COUNT = 10;
	while (FAIL_COUNT--)
	{
		if (FPMXX_Add_Fingerprint(id))
			return true;

		delay(1000);
	}

	return false;
}

void FingerBoard::DeleteAllFingers()
{
	FPMXX_Delete_All_Fingerprint();
}

void FingerBoard::InputPassword(String pswd)
{
	if (!isTouching)
	{
		Keyboard.println(pswd);
		Keyboard.write(176); //ENTER

		isTouching = true;
	}
}

void FingerBoard::InputString(String s, bool enter)
{
	Keyboard.println(s);
	if (enter)
		Keyboard.write(176);
}

void FingerBoard::Press(uint8_t k)
{
	Keyboard.press(k);
}

void FingerBoard::Release()
{
	Keyboard.releaseAll();
}





void FingerBoard::FPMXX_Cmd_Save_Finger(unsigned int storeID)
{
	unsigned long temp = 0;
	unsigned char i;

	FPMXX_Save_Finger[5] = (storeID & 0xFF00) >> 8;
	FPMXX_Save_Finger[6] = (storeID & 0x00FF);

	for (i = 0; i < 7; i++) //计算校验和
		temp = temp + FPMXX_Save_Finger[i];

	FPMXX_Save_Finger[7] = (temp & 0x00FF00) >> 8; //存放校验数据
	FPMXX_Save_Finger[8] = temp & 0x0000FF;

	FPMXX_Send_Cmd(9, FPMXX_Save_Finger, 12);
}

void FingerBoard::FPMXX_Send_Cmd(unsigned char length, unsigned char* address, unsigned char returnLength)
{
	unsigned char i = 0;

	sensorSerial->flush();

	for (i = 0; i < 6; i++) //包头
	{
		sensorSerial->write(FPMXX_Pack_Head[i]);
	}

	for (i = 0; i < length; i++)
	{
		sensorSerial->write(*address);
		address++;
	}

	FPMXX_Cmd_Receive_Data(returnLength);
}

void FingerBoard::FPMXX_Cmd_Receive_Data(unsigned int r_size)
{
	int i = 0;

	while (1)
	{
		if (sensorSerial->available() == r_size)
		{
			for (i = 0; i < r_size; i++)
			{
				FPMXX_RECEIVE_BUFFER[i] = sensorSerial->read();
			}
			break;
		}
	}
}

void FingerBoard::FPMXX_Cmd_StoreTemplate(unsigned int ID)
{
	unsigned int temp = 0;

	FPMXX_Save_Finger[5] = (ID & 0xFF00) >> 8;
	FPMXX_Save_Finger[6] = (ID & 0x00FF);

	for (int i = 0; i < 7; i++) //计算校验和
		temp = temp + FPMXX_Save_Finger[i];

	FPMXX_Save_Finger[7] = (temp & 0x00FF00) >> 8; //存放校验数据
	FPMXX_Save_Finger[8] = temp & 0x0000FF;

	sensorSerial->write((char*)FPMXX_Pack_Head, 6);
	sensorSerial->write((char*)FPMXX_Save_Finger, 9);

	FPMXX_Cmd_Receive_Data(12);
}

bool FingerBoard::FPMXX_Add_Fingerprint(unsigned int  writeID)
{
	FPMXX_Send_Cmd(6, FPMXX_Get_Img, 12);

	//判断接收到的确认码,等于0指纹获取成功    
	if (FPMXX_RECEIVE_BUFFER[9] == 0)
	{
		delay(100);
		FPMXX_Send_Cmd(7, FPMXX_Img_To_Buffer1, 12); //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1

		while (1)
		{
			FPMXX_Send_Cmd(6, FPMXX_Get_Img, 12);

			//判断接收到的确认码,等于0指纹获取成功   
			if (FPMXX_RECEIVE_BUFFER[9] == 0)
			{
				delay(500);
				FPMXX_Send_Cmd(7, FPMXX_Img_To_Buffer2, 12); //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
				FPMXX_Cmd_StoreTemplate(writeID);

				return true;
			}

			delay(100);
		}
	}

	return false;
}

void FingerBoard::FPMXX_Delete_All_Fingerprint()
{
	//添加清空指纹指令
	FPMXX_Send_Cmd(6, FPMXX_Delete_All_Model, 12); //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1

	delay(2000);
}
