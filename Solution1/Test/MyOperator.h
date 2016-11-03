#ifndef __MYOPERATOR_H__
#define __MYOPERATOR_H__


#include <Windows.h>
#include <string>
#include <stdio.h>
#include <atlstr.h>
#include <atltypes.h>


extern "C" {   
#include "setupapi.h"  
#include "hidsdi.h"  
}

#define		MAX_DEVICE_NUM			4
#define		REPORT_SIZE				63		//the report size
#define		COMM_MODE_NORMAL		0
#define		COMM_MODE_FAST			1
#define		TOUCH_POINTS_AND_GESTID_ADDR		0x01  //GEST_ID And Points

#define MAX_NUM_POINT 10

#define		EVENT_DOWN						0x00
#define		EVENT_UP						0x01
#define		EVENT_CONTACT					0x02
#define		NO_EVENT						0x03

/*-----------------------------------------------------------
Error Code Define
-----------------------------------------------------------*/
#define		ERROR_CODE_OK						0x00
#define		ERROR_CODE_CHECKSUM_ERROR			0x01
#define		ERROR_CODE_INVALID_COMMAND			0x02
#define		ERROR_CODE_INVALID_PARAM			0x03
#define		ERROR_CODE_IIC_WRITE_ERROR			0x04
#define		ERROR_CODE_IIC_READ_ERROR			0x05
#define		ERROR_CODE_WRITE_USB_ERROR			0x06
#define		ERROR_CODE_WAIT_RESPONSE_TIMEOUT	0x07
#define		ERROR_CODE_PACKET_RE_ERROR			0x08
#define		ERROR_CODE_NO_DEVICE				0x09
#define		ERROR_CODE_WAIT_WRITE_TIMEOUT		0x0a
#define		ERROR_CODE_READ_USB_ERROR			0x0b
#define		ERROR_CODE_COMM_ERROR				0x0c
#define		ERROR_CODE_ALLOCATE_BUFFER_ERROR	0x0d
#define		ERROR_CODE_DEVICE_OPENED			0x0e
#define		ERROR_CODE_DEVICE_CLOSED			0x0f

//===============================================About Factory Test
#define		ADDR_FW_MAIN_VERSOIN		0xa6
#define		ADDR_FW_SUB_VERSOIN			0x3e
#define		ADDR_FW_SUB_VERSOIN_6X06	0xaf
#define		ADDR_FACTORY_ID				0xa8

#define		HY_I2C_INTERFACE			0x00
#define		HY_SPI_INTERFACE			0x01
#define		HY_USB_INTERFACE			0x02
#define		HY_UART_INTERFACE			0x03

/*-----------------------------------------------------------
命令定义
-----------------------------------------------------------*/
#define		CMD_GET_FW_VERSION			0x01
#define		CMD_SET_INTERFACE			0x02
#define		CMD_GET_INTERFACE			0x03
#define		CMD_SET_IIC_SLV_ADDR		0x04
#define		CMD_GET_IIC_SLV_ADDR		0x05
#define		CMD_SET_IIC_CLK				0x06
#define		CMD_GET_IIC_CLK				0x07
#define		CMD_SET_IIC_BYTE_DELAY		0x08
#define		CMD_GET_IIC_BYTE_DELAY		0x09
#define		CMD_SET_IIC_WR_DELAY		0x0a
#define		CMD_GET_IIC_WR_DELAY		0x0b

#define		CMD_SET_SPI_CLK_PHASE		0x10
#define		CMD_GET_SPI_CLK_PHASE		0x11
#define		CMD_SET_SPI_CS_MODE			0x12
#define		CMD_GET_SPI_CS_MODE			0x13
#define		CMD_SIU_IO_CTRL				0x14
#define		CMD_SIU_POWER_CTRL			0x15
#define		CMD_SIU_VPP_CTRL			0x16
#define		CMD_SIU_LED_CTRL			0x17


#define		CMD_SET_TP_INT_MODE			0x18
#define		CMD_GET_TP_INT_MODE			0x19
#define		CMD_SET_INT_POLLING_CYCLE	0x1a
#define		CMD_GET_INT_POLLING_CYCLE	0x1b

#define		CMD_TOUCH_FLAG				0x1c
#define		CMD_SET_PORT_STATUS			0x1d
#define		CMD_IO_LEVEL_SIGNAL			0x1e

#define		CMD_SPI_IO					0x20
#define		CMD_IIC_IO					0x21
#define		CMD_SPI_UPDATE_TP_FW		0x22
#define		CMD_SAMSUNG_IIC_IO			0x23
#define		CMD_OTP_IO					0x24
#define		CMD_ENTER_DEBUG_MODE		0x25

#define		CMD_ENTER_UPDATE_MODE		0x40
#define		CMD_UPDATE_SIU_FW			0x41
#define		CMD_EXIT_UPDATE_MODE		0x42

#define		CMD_ACK						0xf0
#define		CMD_NACK					0xff

/*-----------------------------------------------------
与SIU板相关的宏定义	-----------	Start
-----------------------------------------------------*/
#define		REPORT_SIZE				63		//the report size
#define		MAX_WRITE_DATA_LEN		512
#define		MAX_READ_DATA_LEN		360		//read buffer size
#define		MAX_PACKETS_NUM			20
#define		SERIAL_PACKET			0x01
#define		COMMAND_REPEAT_TIMES	3		//容错机制，每个命令最多发送三次


#define		MAX_ONCE_WRITE_LENGTH	64

//USB相关
#define		USB_RW_TIMEOUT				200	//ms
#define		REPORT_SIZE					63		//the report size
#define		EP_PACKET_SIZE				64
#define		REPORT_ID_OUT  				0x01		//the out report id
#define		REPORT_ID_IN				0x02		//the in report id

//For bootloader
#define		CMD_ENTER_UPGRADE_MODE_BL	0xB0
#define		CMD_READY_FOR_UPGRADE_BL	0xB1
#define		CMD_SEND_DATA_BL			0xB2
#define		CMD_UPGRADE_CHECKSUM_BL		0xB3
#define		CMD_EXIT_UPRADE_MODE_BL		0xB4
#define		CMD_READ_REGISTER			0x50
#define		CMD_WRITE_REGISTER			0x51
#define		CMD_ACK						0xf0
#define		CMD_NACK					0xff
#define     RAWDATA_BEGIN_ADDR_5926		0x08 //rawdata addr

#define		LINEBYTES(i) 				((i+31)/32*4)
#define		KEY_BITS					17
#define		INT_BITS					32
#define		MAX_FILE_SIZE				(20*1024)
#define		DEFAULT_KEY					0x00018D76
#define		BOOT_PACKET_DATA_NUM		128
#define		DATA_BUFFER_SIZE 			(1024*64)
#define		CONF_BUFFER_SIZE 			(1024)
#define		MAX_UPDATE_FW_SIZE			(27*1024)
#define		MAX_UPDATE_PACKET_SIZE		128//200
#define		MAX_W_FLASH_SIZE			256//480
#define		MAX_R_FLASH_SIZE			256//320
#define		MAX_USB_PACKET_SIZE			56	//200
#define		MAX_SIU_FW_SIZE				(16*1024)
#define		MAX_OTP_FILE_SIZE			(20*1024)
#define		FIRST_PACKET				0
#define		MID_PACKET					1
#define		END_PACKET					2

//#define		CODE_DEBUG

//#define		COMMAND_REPEAT_TIMES	5		//容错机制，每个命令最多发送5次
//#define		USB_RW_TIMEOUT			200		//500ms
#define		COM_RW_TIMEOUT			1000		//500ms

#define		MAX_ONCE_READ_LENGTH	64
#define		MAX_ONCE_WRITE_LENGTH	64
#define     CMD_GET_GPIO_STATUS			0x34

//===============================================电路校准相关寄存器
#define		FT_ADDR_STORE_CLB_INFO	0
#define		FT_ADDR_RUN_STATE_CTRL	1
#define		FT_ADDR_START_SCAN		8
#define		TX_NUM_MAX			    80
#define		RX_NUM_MAX			    80

//===============================================
#define		SAMSUNG		1

#define		I2C_WRITE				0x00
#define		I2C_READ				0x01

#define		I2C_SLV_ADDR_70			0x70
#define		I2C_SLV_ADDR_72			0x72

#define		NO_TOUCH				0xffff
#define		MAX_TOUCH_POINTS		10

#define		FLAG_DATA				0xf5	//Raw data and base data flag byte
#define		FLAG_REG				0xfc	//register flag
#define		FLAG_TOUCH_DATA			0xf9	//Touch data flag
#define		FLAG_CAL_REG			0xf3	//register flag in factory mode
#define		FLAG_CAL_REG_W			0xf7	//register flag in factory mode
#define		FLAG_FTS_REG_R			0xc3	//register flag in factory mode of FTS Protocol
#define		FLAG_FTS_REG_W			0xe3	//register flag in factory mode of FTS Protocol

#define		FLAG_REG_READ			0x40
#define		FLAG_REG_WRITE			0x00

#define		TOUCH_DATA_LEN			46	//26Bytes
#define		TOUCH_DATA_LEN_66		66	//66Bytes
#define		TOUCH_DATA_LEN_36		36	//36Bytes

#define		TOUCH_BUFFER_LEN		300 //最长buffer

//=================================================IIC Standard
#define		DEVIDE_MODE_ADDR				0x00
#define		REG_LINE_NUM	0x01	//1 Byte	读写(RW)	RawData Line Addr		1.HOST读取的互电容RAWDATA数据行数                                                                           
//2.连读rawdata方式：AA：Mc rawdata， AB：Sc Nomal，AC：Sc Water; 
#define		REG_TX_NUM	0x02	//1 Byte	读写(RO)	TxNum		当前使用Tx数量
#define		REG_RX_NUM	0x03	//1 Byte	读写(RO)	RxNum		当前使用Rx数量
#define		REG_CLB		0x04	//1 Byte	读写(RW)	CLB cmd&status		写 - 触发校准和保存参数
								//0x04: 开始自动校准   
								//0x05：保存校准后的参数到flash   
								//读 - 返回校准状态         
								//0x00：未校准初始状态        
								//0x01：校准进行中           
								//0x02：校准成功        
								//0xFF：校准失败

/////////////////////////////////////////////////Reg 8006

#define		REG_CbAddrH  		0x18	//1 Byte	读写(RW)	通道 CB地址偏移量高8位 
#define		REG_CbAddrL			0x19	//1 Byte	读写(RW)	通道 CB地址偏移量低8位 
#define		REG_OrderAddrH		0x1A	//1 Byte	读写(RW)	通道Order地址偏移量高8位
#define		REG_OrderAddrL		0x1B	//1 Byte	读写(RW)	通道Order地址偏移量低8位

#define		REG_RawBuf0			0x6A	//1 Byte	只读(RO)	RawBuf0		Rawdata数据寄存器0
#define		REG_RawBuf1			0x6B	//1 Byte	只读(RO)			Rawdata数据寄存器1
#define		REG_OrderBuf0		0x6C	//1 Byte	读写(RW)	OrderBuf0数据寄存器0 
#define		REG_CbBuf0			0x6E	//1 Byte	读写(RW)	CbBuf0数据寄存器0 

#define		REG_K1Delay			0x31	//1 Byte	读写(RW)	K1Delay		自电容K1周期 (1-255)
#define		REG_K2Delay			0x32	//1 Byte	读写(RW)	K2Delay		自电容K2周期 (10-255)
#define		REG_SCChannelCf		0x34	//1 Byte	读写(RW)	SCChannelCf		自电容通道CF值 （0~7）


//系统自定义消息
#define  ON_MYEVENT_SNED (WM_USER + 1)
#define	 WM_TOUCH_MESSAGE (WM_USER + 2)


struct/* FT_OS_API */TouchDataStruct
{
	BYTE	bpoints;//点数
	BYTE	bSign;//手势
	BYTE	bTouchArea[MAX_TOUCH_POINTS];
	BYTE	bStrength[MAX_TOUCH_POINTS];//lenovo特别用的强度,默认为0xff
	CPoint	Points[MAX_TOUCH_POINTS];
	BYTE	bEvent[MAX_TOUCH_POINTS];  //点事件 0（00）Down_Event手指第一次接触,1(01)Up_Event手指释放,2(10)contact_event手指长时间触摸,3(11)No_Event没有触摸
	BYTE	bPressure[MAX_TOUCH_POINTS];

	CPoint	PenPoint;
	USHORT  usPenPressure;
	BYTE	bPenEvent;
	BYTE	bPenButton;
	BYTE	bPenEraser;
	SHORT   sPenTiltX;
	SHORT   sPenTiltY;

	TouchDataStruct()
	{
		bpoints=0;
		bSign=0;
		for (int i=0;i<MAX_TOUCH_POINTS;i++)
		{
			Points[i].x=Points[i].y=NO_TOUCH;
			bEvent[i] = 0xff;
			bStrength[i] = 0xff;
			bPressure[i] = 0x00;
		}

		PenPoint.x=PenPoint.y=NO_TOUCH;
		usPenPressure=0;
		bPenEvent=0xff;
		bPenButton=0xff;
		bPenEraser=0xff;
		sPenTiltX=0;
		sPenTiltY=0;

	}
	void clear()
	{
		bpoints=0;
		bSign=0xff;
		for (int i=0;i<MAX_TOUCH_POINTS;i++)
		{
			Points[i].x=Points[i].y=NO_TOUCH;
			bEvent[i] = 0xff;
			bStrength[i] = 0x00;
			bTouchArea[i] = 0x00;
		}
	}
};


class CMyOperator
{
public:
	CMyOperator();
	~CMyOperator(void);

	//获得连接设备数
	HIDD_ATTRIBUTES GetDevicesInfo(void);
	int GetDevicesNum() const;
	unsigned char ReadReg(unsigned char RegAddr, unsigned char *pRegData);
	unsigned char WriteReg(unsigned char RegAddr, unsigned char RegData);
	void SetChannelNum(unsigned char ChannelXNum, unsigned char ChannelYNum);

	//进入工厂模式
	unsigned char EnterFactory();
	//进入工作模式
	unsigned char EnterWork();

	bool SetDlgHWnd(HWND hWnd);
	HWND GetDlgHWnd()const;

	void TestItem_RawDataTest();
	void TestItem_StopRawDataTest();
	void StartTouchData();
	void EndTouchData();
	

private:
	void InitDevicesInfo(HIDD_ATTRIBUTES &DevicesInfo);
	unsigned char GetChecksum(unsigned char *pData, unsigned char BytesNum);
	unsigned char USB_SendCmd(unsigned char *pCmdPackets, unsigned char CmdPacketsNum, unsigned char RePacketsNum);
	int HY_IIC_IO(HANDLE cyHandle, BYTE *pBufferWrite, unsigned short BytesToWrite, BYTE *pBufferRead, unsigned short BytesToRead);
	unsigned char ReadRawData(unsigned char Freq, unsigned char LineNum, int ByteNum, int *pRevBuffer);
	void SysDelay(unsigned int DelayMS);
	unsigned char GetRawData(void);
	unsigned char StartScan(unsigned char Freq);
	unsigned char ReadTouchData(bool bIsPointID, unsigned char iMaxPoints, unsigned char *pTouchData, struct TouchDataStruct& TouchDataProcessed/*返回触摸数据的结构体*/);
	
	static DWORD WINAPI ThreadProc(LPVOID lp);
	static DWORD WINAPI GetDataThreadProc(LPVOID lp);
	static DWORD WINAPI TouchDataThreadProc(LPVOID lp);


private:
	int m_RawData[TX_NUM_MAX][RX_NUM_MAX];
	HANDLE m_hGetRawDataThread;
	bool m_StopGetDataThread;

	HANDLE m_hGetTouchDataThread;

	int m_iDeviceNum;
	bool m_DeviceState[MAX_DEVICE_NUM];
	HANDLE m_hDevice; //HID设备（SIU板）句柄
	int m_iCommMode;
	int m_iOneRowLength;

	unsigned char m_InBuffer[0x8000];		//存储收到的报告数据包
	unsigned char m_RePacketCounter;		//记录收到的数据包数目
	unsigned char m_PacketNumToReturn;		//应该返回的数据包数目
	int m_iReceivedBytes;

	
	unsigned char G_CommMode;

	unsigned short m_REPORT_ID_OUT;
	unsigned short m_REPORT_ID_IN;

	HANDLE m_hThread;
	HANDLE m_hUSBReadEvent;
	HANDLE m_hUSBSendEvent;
	HANDLE m_hTouchEvent;
	HANDLE m_hGetDataEndEvent;
	bool m_bStopThread;
	bool m_bTouchDataThread;
	CRITICAL_SECTION m_CriticalSection; 

	int m_iSize;
	int m_iMaxPoints;

	unsigned char m_ChannelXNum;
	unsigned char m_ChannelYNum;

	HWND m_hWnd;	
	DWORD m_TouchDataID;	
};

#endif 
