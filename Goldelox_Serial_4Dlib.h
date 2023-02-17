/*
 * Goldelox_Serial_4Dlib.h
 *
 *  Created on: Feb 16, 2023
 *      Author: mehme
 */

#ifndef INC_GOLDELOX_SERIAL_4DLIB_H_
#define INC_GOLDELOX_SERIAL_4DLIB_H_

#include "stm32f4xx_hal.h"
#include <string.h>

#define Err4D_OK 		0
#define Err4D_Timeout	1
#define Err4D_NAK		2

#define t4DWordArray 			uint16_t *
#define t4DByteArray 			char *
#define t4DSector 				char *

typedef void (*Tcallback4D)(int, unsigned char);

class Goldelox_Serial_4Dlib{
	public:
			Goldelox_Serial_4Dlib(UART_HandleTypeDef * virtualPort);
			Tcallback4D Callback4D;

			void WriteInt(uint16_t data);

			uint16_t charheight(char  TestChar);
			uint16_t charwidth(char  TestChar);

			uint16_t gfx_BGcolour(uint16_t  Color);
			void gfx_ChangeColour(uint16_t  OldColor, uint16_t  NewColor);
			void gfx_Circle(uint16_t  X, uint16_t  Y, uint16_t  Radius, uint16_t  Color);
			void gfx_CircleFilled(uint16_t  X, uint16_t  Y, uint16_t  Radius, uint16_t  Color);
			void gfx_Clipping(uint16_t  OnOff);
			void gfx_ClipWindow(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2);
			void gfx_Cls();
			uint16_t gfx_Contrast(uint16_t  Contrast);
			uint16_t gfx_FrameDelay(uint16_t  Msec);

			void gfx_Line(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  Color);
			uint16_t gfx_LinePattern(uint16_t  Pattern);
			void gfx_LineTo(uint16_t  X, uint16_t  Y);
			void gfx_MoveTo(uint16_t  X, uint16_t  Y);
			uint16_t gfx_OutlineColour(uint16_t  Color);
			void gfx_Polygon(uint16_t  n, uint16_t *  Xvalues, uint16_t *  Yvalues, uint16_t  Color);
			void gfx_Polyline(uint16_t  n, uint16_t *  Xvalues, uint16_t *  Yvalues, uint16_t  Color);
			void gfx_PutPixel(uint16_t  X, uint16_t  Y, uint16_t  Color);
			void gfx_Rectangle(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  Color);
			void gfx_RectangleFilled(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  Color);
			uint16_t gfx_ScreenMode(uint16_t  ScreenMode);
			void gfx_Set(uint16_t  Func, uint16_t  Value);
			uint16_t gfx_Transparency(uint16_t  OnOff);
			uint16_t gfx_TransparentColour(uint16_t  Color);
			void gfx_Triangle(uint16_t  X1, uint16_t  Y1, uint16_t  X2, uint16_t  Y2, uint16_t  X3, uint16_t  Y3, uint16_t  Color);
			void media_Image(uint16_t  X, uint16_t  Y);
			void media_SetAdd(uint16_t  HiWord, uint16_t  LoWord);
			void media_SetSector(uint16_t  HiWord, uint16_t  LoWord);
			void media_Video(uint16_t  X, uint16_t  Y);
			void media_VideoFrame(uint16_t  X, uint16_t  Y, uint16_t  Framenumber);
			void pokeB(uint16_t  Address, uint16_t  WordValue) ;
			void pokeW(uint16_t  Address, uint16_t  WordValue) ;
	        void putCH(uint16_t  WordChar);
	    	void print(const char[]);
	    	void println(const char[]);
	    	uint16_t txt_Attributes(uint16_t  Attribs);
	    	uint16_t txt_BGcolour(uint16_t  Color);
	    	uint16_t txt_Bold(uint16_t  Bold);
	    	uint16_t txt_FGcolour(uint16_t  Color);
	    	uint16_t txt_FontID(uint16_t  FontNumber);
	    	uint16_t txt_Height(uint16_t  Multiplier);
	    	uint16_t txt_Inverse(uint16_t  Inverse);
	    	uint16_t txt_Italic(uint16_t  Italic);
	    	void txt_MoveCursor(uint16_t  Line, uint16_t  Column);
	    	uint16_t txt_Opacity(uint16_t  TransparentOpaque);
	    	void txt_Set(uint16_t  Func, uint16_t  Value);
	    	uint16_t txt_Underline(uint16_t  Underline);
	    	uint16_t txt_Width(uint16_t  Multiplier);
	    	uint16_t txt_Wrap(uint16_t  Position);
	    	uint16_t txt_Xgap(uint16_t  Pixels);
	    	uint16_t txt_Ygap(uint16_t  Pixels);


			uint16_t gfx_Orbit(uint16_t  Angle, uint16_t  Distance, uint16_t *  Xdest, uint16_t *  Ydest);
			void gfx_SetClipRegion();
			uint16_t gfx_GetPixel(uint16_t  X, uint16_t  Y);
			int media_Flush();
			int media_Init();
			int media_ReadByte();
			int media_ReadWord();
			int media_WriteByte(uint16_t  Byte);
			int media_WriteWord(uint16_t  Word);
			int peekB(uint16_t  Address);
			int peekW(uint16_t  Address);
			uint16_t putstr(char *  InString);
			uint16_t sys_GetModel(char *  ModelStr);
			uint16_t sys_GetVersion();
			uint16_t sys_GetPmmC();

			void GetAck(void);

			//4D Global Variables Used
			int Error4D;  				// Error indicator,  used and set by Intrinsic routines
			unsigned char Error4D_Inv;	// Error byte returned from com port, onl set if error = Err_Invalid
			unsigned long TimeLimit4D = 3000;


	private:
			UART_HandleTypeDef * _huart;
			//Intrinsic 4D Routines
			void WriteChars(char * charsout);
			void WriteBytes(uint8_t * Source, int Size);
			void WriteWords(uint16_t * Source, int Size);
			void getbytes(uint8_t * data, int size);
			uint16_t GetWord(void);
			void getString(char * outStr, int strLen);
			int GetAckResp(void);
			uint16_t GetAckRes2Words(uint16_t * word1, uint16_t * word2);
			void GetAck2Words(uint16_t * word1, uint16_t * word2);
			uint16_t GetAckResSector(uint8_t * Sector);
			uint16_t GetAckResStr(char * OutStr);
			uint16_t GetAckResData(uint8_t * OutData, uint16_t size);
			void printNumber(unsigned long, uint8_t);
			//void printFloat(double number, uint8_t digits);

};

#endif /* INC_GOLDELOX_SERIAL_4DLIB_H_ */
