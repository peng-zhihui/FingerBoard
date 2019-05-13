#pragma once
#include "fingerprint.h"
#include "Keyboard.h"

enum FbStatus
{
	ERROR_SENSOR_NOT_FOUND = 1,
	SUCCESS = 0,
	ERROR_NO_FINGER = -1
};

class FingerBoard
{
public:
	FingerBoard(HardwareSerial* hs);

	FbStatus Begin();
	FbStatus Begin(Serial_* hs);

	int GetFingerID();
	bool AddFinger(unsigned int id);
	void DeleteAllFingers();


	void InputPassword(String pswd);
	void InputString(String s, bool enter);
	void Press(uint8_t k);
	void Release();

private:
	Serial_* debug = NULL;
	HardwareSerial* sensorSerial = NULL;
	Fingerprint sensor = NULL;

	bool isTouching = false;

	volatile unsigned char FPMXX_RECEIVE_BUFFER[64];

	unsigned char FPMXX_Pack_Head[6] = { 0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF }; //协议包头
	unsigned char FPMXX_Get_Img[6] = { 0x01, 0x00, 0x03, 0x01, 0x0, 0x05 }; //获得指纹图像
	unsigned char FPMXX_Save_Finger[9] = { 0x01, 0x00, 0x06, 0x06, 0x01, 0x00, 0x0B, 0x00, 0x19 }; //将BUFFER1中的特征码存放到指定的位置
	unsigned char FPMXX_Search[11] = { 0x01, 0x0, 0x08, 0x04, 0x01, 0x0, 0x0, 0x03, 0xE7, 0x0, 0xF8 }; //搜索指纹搜索范围0 - 999,使用BUFFER1中的特征码搜索
	unsigned char FPMXX_Get_Device[6] = { 0x01, 0x00, 0x03, 0x16, 0x00, 0x1A };
	unsigned char FPMXX_Get_Templete_Count[6] = { 0x01, 0x00, 0x03, 0x1D, 0x00, 0x21 }; //获得模版总数
	unsigned char FPMXX_Img_To_Buffer1[7] = { 0x01, 0x0, 0x04, 0x02, 0x01, 0x0, 0x08 }; //将图像放入到BUFFER1
	unsigned char FPMXX_Img_To_Buffer2[7] = { 0x01, 0x0, 0x04, 0x02, 0x02, 0x0, 0x09 }; //将图像放入到BUFFER2
	unsigned char FPMXX_Reg_Model[6] = { 0x01, 0x0, 0x03, 0x05, 0x0, 0x09 }; //将BUFFER1跟BUFFER2合成特征模版
	unsigned char FPMXX_Delete_All_Model[6] = { 0x01, 0x0, 0x03, 0x0d, 0x00, 0x11 }; //删除指纹模块里所有的模版


	void FPMXX_Cmd_Save_Finger(unsigned int storeID);
	void FPMXX_Send_Cmd(unsigned char length, unsigned char* address, unsigned char returnLength);
	void FPMXX_Cmd_Receive_Data(unsigned int r_size);
	void FPMXX_Cmd_StoreTemplate(unsigned int ID);
	bool FPMXX_Add_Fingerprint(unsigned int  writeID);
	void FPMXX_Delete_All_Fingerprint();
};