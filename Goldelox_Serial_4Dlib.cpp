/*
 * Goldelox_Serial_4Dlib.cpp
 *
 *  Created on: Feb 16, 2023
 *      Author: mehme
 */

#include "stm32f4xx_hal.h"
#include "Goldelox_Serial_4Dlib.h"
#include "Goldelox_Const4DSerial.h"


Goldelox_Serial_4Dlib::Goldelox_Serial_4Dlib(UART_HandleTypeDef * port) {
	_huart = port;
	//Flush the buffer
	HAL_UART_AbortReceive(_huart);
}

void Goldelox_Serial_4Dlib::WriteInt(uint16_t data){
	//Separate the upper and lower bytes
	uint8_t thisData[] = {(uint8_t) (data >> 8), (uint8_t) (data & 0xFF)};
    //Write the data
	HAL_UART_Transmit(_huart, thisData, 2, 10);
}

//**********************************Intrinsic 4D Routines**************************************//
void Goldelox_Serial_4Dlib::WriteChars(char * charsout){
	//Count how many bytes to write
	int numBytes = 0;
	while(*charsout++) {
		numBytes++;
		//Protect against overrun somewhat
		if(numBytes > 1000)
			break;
		}
	HAL_UART_Transmit(_huart, (uint8_t *) charsout, numBytes, 10) ;
}

void Goldelox_Serial_4Dlib::WriteBytes(uint8_t * source, int size)
{
	HAL_UART_Transmit(_huart, source, size, 10);
}

void Goldelox_Serial_4Dlib::WriteWords(uint16_t * Source, int Size)
{
  uint16_t wk ;
  for (int i = 0; i < Size; i++)
  {
    wk = *Source++ ;
	//Separate the upper and lower bytes
	uint8_t thisData[] = {(uint8_t) (wk >> 8), (uint8_t) (wk & 0xFF)};
    HAL_UART_Transmit(_huart, (uint8_t *) thisData, 2, 10) ;
  }
}

void Goldelox_Serial_4Dlib::getbytes(uint8_t * data, int size)
{
	int response = HAL_UART_Receive(_huart, data, size, TimeLimit4D);

	if (response != HAL_OK)
	{
		Error4D = Err4D_Timeout;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv) ;
	}
}

void Goldelox_Serial_4Dlib::GetAck(void)
{
	uint8_t readx = 0;
	Error4D = Err4D_OK;

	int response = HAL_UART_Receive(_huart, &readx, 1, TimeLimit4D);

	if (response != HAL_OK)
	{
		//Flush the buffer
		HAL_UART_AbortReceive(_huart);

		Error4D = Err4D_Timeout;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}
	else if (readx != 6)
	{
		Error4D = Err4D_NAK;
		Error4D_Inv = readx;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}
}

uint16_t Goldelox_Serial_4Dlib::GetWord(void)
{
	uint8_t readx[2] = {0, 0};

	if (Error4D != Err4D_OK)
		return 0 ;

	//Receive 2 bytes
	int response = HAL_UART_Receive(_huart, readx, 2, TimeLimit4D);

	if (response != HAL_OK)
	{
		//Flush the buffer
		HAL_UART_AbortReceive(_huart);

		Error4D  = Err4D_Timeout ;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv) ;
		return 0 ;
	}
	else
		return readx[0] << 8 | readx[1] ;
}

void Goldelox_Serial_4Dlib::getString(char * outStr, int strLen)
{
	if (Error4D != Err4D_OK)
	{
		outStr[0] = 0 ;
		return ;
	}

	int response = HAL_UART_Receive(_huart, (uint8_t *) outStr, strLen, TimeLimit4D);

	if (response != HAL_OK)
	{
		//Flush the buffer
		HAL_UART_AbortReceive(_huart);

		Error4D  = Err4D_Timeout ;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv) ;
	}

	//Add a terminator
	//TODO: Not sure if this is required?
	outStr[strLen - 1] = 0 ;
}

int Goldelox_Serial_4Dlib::GetAckResp(void)
{
	uint8_t readx[3] = {0, 0, 0};
	Error4D = Err4D_OK;

	int response = HAL_UART_Receive(_huart, readx, 3, TimeLimit4D);

	if (response != HAL_OK)
	{
		//Flush the buffer
		HAL_UART_AbortReceive(_huart);

		Error4D = Err4D_Timeout;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}
	else if (readx[0] != 6)
	{
		Error4D = Err4D_NAK;
		Error4D_Inv = readx[0];
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}

	return ((readx[1] << 8) | (readx[2] & 0xFF));
}

uint16_t Goldelox_Serial_4Dlib::GetAckRes2Words(uint16_t * word1, uint16_t * word2)
{

	uint8_t readx[7] = {0, 0, 0, 0, 0, 0, 0};
	Error4D = Err4D_OK;

	int response = HAL_UART_Receive(_huart, readx, 7, TimeLimit4D);

	if (response != HAL_OK)
	{
		//Flush the buffer
		HAL_UART_AbortReceive(_huart);

		Error4D = Err4D_Timeout;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}
	else if (readx[0] != 6)
	{
		Error4D = Err4D_NAK;
		Error4D_Inv = readx[0];
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}

	//Update the provided buffers
	*word1 = (readx[3] << 8) | (readx[4] & 0xFF);
	*word2 = (readx[5] << 8) | (readx[6] & 0xFF);

	//Return result code
	return (readx[1] << 8) | (readx[2] & 0xFF);
}

void Goldelox_Serial_4Dlib::GetAck2Words(uint16_t * word1, uint16_t * word2)
{
	uint8_t readx[5] = {0, 0, 0, 0, 0};
	Error4D = Err4D_OK;

	int response = HAL_UART_Receive(_huart, readx, 5, TimeLimit4D);

	if (response != HAL_OK)
	{
		//Flush the buffer
		HAL_UART_AbortReceive(_huart);

		Error4D = Err4D_Timeout;
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}
	else if (readx[0] != 6)
	{
		Error4D = Err4D_NAK;
		Error4D_Inv = readx[0];
		if (Callback4D != NULL)
			Callback4D(Error4D, Error4D_Inv);
	}

	//Update the provided buffers
	*word1 = (readx[1] << 8) | (readx[2] & 0xFF);
	*word2 = (readx[3] << 8) | (readx[4] & 0xFF);
}

//TODO: Fix this
uint16_t Goldelox_Serial_4Dlib::GetAckResSector(uint8_t * Sector)
{
	int Result ;
	GetAck() ;
	Result = GetWord() ;
	getbytes(Sector, 512) ;
	return Result ;
}

//TODO: Fix this
uint16_t Goldelox_Serial_4Dlib::GetAckResStr(char * OutStr)
{
	int Result ;
	GetAck() ;
	Result = GetWord() ;
	getString(OutStr, Result) ;
	return Result ;
}

//TODO: Fix this
uint16_t Goldelox_Serial_4Dlib::GetAckResData(uint8_t * OutData, uint16_t size)
{
	int Result ;
	GetAck() ;
	Result = GetWord() ;
	getbytes(OutData, size) ;
	return Result ;
}


uint16_t Goldelox_Serial_4Dlib::gfx_BGcolour(uint16_t  colour)
{
	WriteInt(F_gfx_BGcolour);
	WriteInt(colour);

	return GetAckResp();
}

void Goldelox_Serial_4Dlib::gfx_ChangeColour(uint16_t  oldColour, uint16_t  newColour)
{
	WriteInt(F_gfx_ChangeColour);
	WriteInt(oldColour);
	WriteInt(newColour);
	GetAck();
}

void Goldelox_Serial_4Dlib::gfx_Circle(uint16_t  X, uint16_t  Y, uint16_t  radius, uint16_t  colour)
{
	WriteInt(F_gfx_Circle);
	WriteInt(X);
	WriteInt(Y);
	WriteInt(radius);
	WriteInt(colour);
	GetAck();
}

void Goldelox_Serial_4Dlib::gfx_Clipping(uint16_t  onOff)
{
	WriteInt(F_gfx_Clipping);
	WriteInt(onOff);
	GetAck();
}

void Goldelox_Serial_4Dlib::gfx_ClipWindow(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2)
{
	WriteInt(F_gfx_ClipWindow);
	WriteInt(X1);
	WriteInt(Y1);
	WriteInt(X2);
	WriteInt(Y2);
	GetAck();
}

void Goldelox_Serial_4Dlib::gfx_Cls()
{
	WriteInt(F_gfx_Cls);
	GetAck();
}

uint16_t Goldelox_Serial_4Dlib::gfx_Contrast(uint16_t Contrast)
{
	WriteInt(F_gfx_Contrast);
	WriteInt(Contrast);

	return GetAckResp();
}

uint16_t Goldelox_Serial_4Dlib::gfx_FrameDelay(uint16_t  Msec)
{
	WriteInt(F_gfx_FrameDelay);
	WriteInt(Msec);

	return GetAckResp();
}


void Goldelox_Serial_4Dlib::gfx_Line(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  colour)
{
	WriteInt(F_gfx_Line);
	WriteInt(X1);
	WriteInt(Y1);
	WriteInt(X2);
	WriteInt(Y2);
	WriteInt(colour);

	GetAck();
}

uint16_t Goldelox_Serial_4Dlib::gfx_LinePattern(uint16_t  Pattern)
{
	WriteInt(F_gfx_LinePattern);
	WriteInt(Pattern);

	return GetAckResp();
}

void Goldelox_Serial_4Dlib::gfx_LineTo(uint16_t  X, uint16_t  Y)
{
	WriteInt(F_gfx_LineTo);
	WriteInt(X);
	WriteInt(Y);

	GetAck();
}

void Goldelox_Serial_4Dlib::gfx_MoveTo(uint16_t  X, uint16_t  Y)
{
	WriteInt(F_gfx_MoveTo);
	WriteInt(X);
	WriteInt(Y);

	GetAck();
}

uint16_t Goldelox_Serial_4Dlib::gfx_OutlineColour(uint16_t  colour)
{
	WriteInt(F_gfx_OutlineColour);
	WriteInt(colour);

	return GetAckResp();
}


void Goldelox_Serial_4Dlib::gfx_Polygon(uint16_t  n, t4DWordArray  Xvalues, t4DWordArray  Yvalues, uint16_t  colour)
{
	WriteInt(F_gfx_Polygon);
	WriteInt(n);
	WriteWords(Xvalues, n);
	WriteWords(Yvalues, n);
	WriteInt(colour);

	GetAck();
}



void Goldelox_Serial_4Dlib::gfx_Polyline(uint16_t  n, t4DWordArray  Xvalues, t4DWordArray  Yvalues, uint16_t  colour)
{
	WriteInt(F_gfx_Polyline);
	WriteInt(n);
	WriteWords(Xvalues, n);
	WriteWords(Yvalues, n);
	WriteInt(colour);

	GetAck();
}

void Goldelox_Serial_4Dlib::gfx_PutPixel(uint16_t  X, uint16_t  Y, uint16_t  colour)
{
	WriteInt(F_gfx_PutPixel);
	WriteInt(X);
	WriteInt(Y);
	WriteInt(colour);

	GetAck();
}

void Goldelox_Serial_4Dlib::gfx_Rectangle(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  colour)
{
	WriteInt(F_gfx_Rectangle);
	WriteInt(X1);
	WriteInt(Y1);
	WriteInt(X2);
	WriteInt(Y2);
	WriteInt(colour);

	GetAck();
}

void Goldelox_Serial_4Dlib::gfx_RectangleFilled(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  colour)
{
	WriteInt(F_gfx_RectangleFilled);
	WriteInt(X1);
	WriteInt(Y1);
	WriteInt(X2);
	WriteInt(Y2);
	WriteInt(colour);

	GetAck();
}

uint16_t Goldelox_Serial_4Dlib::gfx_ScreenMode(uint16_t  screenMode)
{
	WriteInt(F_gfx_ScreenMode);
	WriteInt(screenMode);

	return GetAckResp();
}

void Goldelox_Serial_4Dlib::gfx_Set(uint16_t  Func, uint16_t  Value)
{
	WriteInt(F_gfx_Set);
	WriteInt(Func);
	WriteInt(Value);

	GetAck();
}


uint16_t Goldelox_Serial_4Dlib::gfx_Transparency(uint16_t  OnOff)
{
	WriteInt(F_gfx_Transparency);
	WriteInt(OnOff);

	return GetAckResp();
}

uint16_t Goldelox_Serial_4Dlib::gfx_TransparentColour(uint16_t  colour)
{
	WriteInt(F_gfx_TransparentColour);
	WriteInt(colour);

	return GetAckResp();
}

void Goldelox_Serial_4Dlib::gfx_Triangle(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  X3, uint16_t  Y3, uint16_t  colour)
{
	WriteInt(F_gfx_Triangle);
	WriteInt(X1);
	WriteInt(Y1);
	WriteInt(X2);
	WriteInt(Y2);
	WriteInt(X3);
	WriteInt(Y3);
	WriteInt(colour);

	GetAck();
}

void Goldelox_Serial_4Dlib::media_Image(uint16_t  X, uint16_t  Y)
{
	WriteInt(F_media_Image);
	WriteInt(X);
	WriteInt(Y);

	GetAck();
}

void Goldelox_Serial_4Dlib::media_SetAdd(uint16_t  HiWord, uint16_t  LoWord)
{
	WriteInt(F_media_SetAdd);
	WriteInt(HiWord);
	WriteInt(LoWord);

	GetAck();
}

void Goldelox_Serial_4Dlib::media_SetSector(uint16_t  HiWord, uint16_t  LoWord)
{
	WriteInt(F_media_SetSector);
	WriteInt(HiWord);
	WriteInt(LoWord);

	GetAck();
}

void Goldelox_Serial_4Dlib::media_Video(uint16_t  X, uint16_t  Y)
{
	WriteInt(F_media_Video);
	WriteInt(X);
	WriteInt(Y);

	GetAck();
}

void Goldelox_Serial_4Dlib::media_VideoFrame(uint16_t  X, uint16_t  Y, uint16_t  Framenumber)
{
	WriteInt(F_media_VideoFrame);
	WriteInt(X);
	WriteInt(Y);
	WriteInt(Framenumber);

	GetAck();
}

void Goldelox_Serial_4Dlib::putCH(uint16_t  WordChar)
{
	WriteInt(F_putCH);
	WriteInt(WordChar);

	GetAck() ;
}

void Goldelox_Serial_4Dlib::pokeB(uint16_t  Address, uint16_t  WordValue)
{
	WriteInt(F_pokeB);
	WriteInt(Address);
	WriteInt(WordValue);

	GetAck() ;
}

void Goldelox_Serial_4Dlib::pokeW(uint16_t  Address, uint16_t  WordValue)
{
	WriteInt(F_pokeB);
	WriteInt(Address);
	WriteInt(WordValue);

	GetAck() ;
}

void Goldelox_Serial_4Dlib::print(const char str[])
{
  int len = strlen(str);
  for(int x = 0; x<len; x++)
  {
	putCH(str[x]);
  }
}

void Goldelox_Serial_4Dlib::println(const char c[])
{
  print(c);
  putCH('\n');
  putCH('\r');
}

//-Private:

void Goldelox_Serial_4Dlib::printNumber(unsigned long n, uint8_t base) {
  char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
  int count = 0;
  do {
    unsigned long m = n;
    n /= base;
    char c = m - base * n;
    buf[count] = c < 10 ? c + '0' : c + 'A' - 10;
    count++;
  } while(n);

  for(int x = count - 1; x>=0; x--)
  {
	putCH(buf[x]);
  }
}

uint16_t Goldelox_Serial_4Dlib::txt_Attributes(uint16_t  Attribs)
{
	WriteInt(F_txt_Attributes);
	WriteInt(Attribs);

	return GetAckResp();
}

uint16_t Goldelox_Serial_4Dlib::txt_BGcolour(uint16_t  colour)
{
	WriteInt(F_txt_BGcolour);
	WriteInt(colour);

	return GetAckResp();
}

uint16_t Goldelox_Serial_4Dlib::txt_Bold(uint16_t  Bold)
{
	WriteInt(F_txt_Bold);
	WriteInt(Bold);

	return GetAckResp();
}

uint16_t Goldelox_Serial_4Dlib::txt_FGcolour(uint16_t  colour)
{
	WriteInt(F_txt_FGcolour);
	WriteInt(colour);

	return GetAckResp();
}

uint16_t Goldelox_Serial_4Dlib::txt_FontID(uint16_t  FontNumber)
{
	WriteInt(F_txt_FontID);
	WriteInt(FontNumber);

	return GetAckResp();
}

uint16_t Goldelox_Serial_4Dlib::txt_Height(uint16_t  Multiplier)
{
	WriteInt(F_txt_Height);
	WriteInt(Multiplier);

	return GetAckResp();
}

uint16_t Goldelox_Serial_4Dlib::txt_Inverse(uint16_t  Inverse)
{
	WriteInt(F_txt_Inverse);
	WriteInt(Inverse);

	return GetAckResp();
}

uint16_t Goldelox_Serial_4Dlib::txt_Italic(uint16_t  Italic)
{
	WriteInt(F_txt_Italic);
	WriteInt(Italic);

	return GetAckResp();
}

void Goldelox_Serial_4Dlib::txt_MoveCursor(uint16_t  Line, uint16_t  Column)
{
	WriteInt(F_txt_MoveCursor);
	WriteInt(Line);
	WriteInt(Column);

	GetAck();
}

uint16_t Goldelox_Serial_4Dlib::txt_Opacity(uint16_t  TransparentOpaque)
{
	WriteInt(F_txt_Opacity);
	WriteInt(TransparentOpaque);

	return GetAckResp();
}

void Goldelox_Serial_4Dlib::txt_Set(uint16_t  Func, uint16_t  Value)
{
	WriteInt(F_txt_Set);
	WriteInt(Func);
	WriteInt(Value);

	GetAck();
}

uint16_t Goldelox_Serial_4Dlib::txt_Underline(uint16_t  Underline)
{
	WriteInt(F_txt_Underline);
	WriteInt(Underline);

	return GetAckResp();
}

uint16_t Goldelox_Serial_4Dlib::txt_Width(uint16_t  Multiplier)
{
	WriteInt(F_txt_Width);
	WriteInt(Multiplier);

	return GetAckResp();
}


uint16_t Goldelox_Serial_4Dlib::txt_Xgap(uint16_t  Pixels)
{
	WriteInt(F_txt_Xgap);
	WriteInt(Pixels);

	return GetAckResp() ;
}

uint16_t Goldelox_Serial_4Dlib::txt_Ygap(uint16_t  Pixels)
{
	WriteInt(F_txt_Ygap);
	WriteInt(Pixels);

	return GetAckResp() ;
}

uint16_t Goldelox_Serial_4Dlib::charheight(char  testChar)
{
	WriteInt(F_charheight);
	WriteBytes((uint8_t *) testChar, 1);

	return GetAckResp();
}

uint16_t Goldelox_Serial_4Dlib::charwidth(char testChar)
{
	WriteInt(F_charwidth);
	WriteBytes((uint8_t *) testChar, 1);

	return GetAckResp();
}

uint16_t Goldelox_Serial_4Dlib::gfx_GetPixel(uint16_t  X, uint16_t  Y)
{
	WriteInt(F_gfx_GetPixel);
	WriteInt(X);
	WriteInt(Y);

	return GetAckResp();
}

int Goldelox_Serial_4Dlib::media_Flush()
{
	WriteInt(F_media_Flush);

	return GetAckResp();
}

int Goldelox_Serial_4Dlib::media_Init()
{
	WriteInt(F_media_Init);

	return GetAckResp();
}

int Goldelox_Serial_4Dlib::media_ReadByte()
{
	WriteInt(F_media_ReadByte);

	return GetAckResp();
}

int Goldelox_Serial_4Dlib::media_ReadWord()
{
	WriteInt(F_media_ReadWord);

	return GetAckResp();
}

int Goldelox_Serial_4Dlib::media_WriteByte(uint16_t  Byte)
{
	WriteInt(F_media_WriteByte);
	WriteInt(Byte);

	return GetAckResp();
}

int Goldelox_Serial_4Dlib::media_WriteWord(uint16_t  Word)
{
	WriteInt(F_media_WriteWord);
	WriteInt(Word);

	return GetAckResp();
}

int Goldelox_Serial_4Dlib::peekB(uint16_t  Address)
{
	WriteInt(F_peekB);
	WriteInt(Address);

	return GetAckResp();
}

int Goldelox_Serial_4Dlib::peekW(uint16_t  Address)
{
	WriteInt(F_peekW);
	WriteInt(Address);

	return GetAckResp();
}

uint16_t Goldelox_Serial_4Dlib::putstr(char *  InString)
{
	WriteInt(F_putstr);
	WriteChars(InString);

	return GetAckResp();
}

uint16_t Goldelox_Serial_4Dlib::sys_GetModel(char *  ModelStr)
{
	WriteInt(F_sys_GetModel);

	return GetAckResStr(ModelStr);
}

uint16_t Goldelox_Serial_4Dlib::sys_GetVersion()
{
	WriteInt(F_sys_GetVersion);

	return GetAckResp();
}

uint16_t Goldelox_Serial_4Dlib::sys_GetPmmC()
{
	WriteInt(F_sys_GetPmmC);

	return GetAckResp();
}
