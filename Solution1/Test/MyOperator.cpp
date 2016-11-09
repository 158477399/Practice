#include "MyOperator.h"


CMyOperator::CMyOperator()
{
	m_iDeviceNum = 0;
	::memset(m_DeviceState, 0, sizeof(m_DeviceState));

	m_iCommMode = HY_I2C_INTERFACE;
	m_hDevice = INVALID_HANDLE_VALUE;
	m_iOneRowLength = MAX_ONCE_READ_LENGTH;

	CString str;
	str.Format(_T("Write_Event_%d"), m_iDeviceNum + 1);
	m_hUSBReadEvent = CreateEvent(NULL, true, true, str);
	
	m_hUSBSendEvent = CreateEvent(NULL, true, false,NULL);
	ResetEvent(m_hUSBSendEvent);

	m_hTouchEvent = CreateEvent(NULL, true, false, NULL);
	ResetEvent(m_hUSBSendEvent);

	m_hGetDataEndEvent = CreateEvent(NULL, true, false, NULL);
	ResetEvent(m_hGetDataEndEvent);

	m_REPORT_ID_OUT=0x01;		//the out report id
	m_REPORT_ID_IN =0x02;		//the in report id

	m_bStopThread = false;
	m_StopGetDataThread = false;
	m_bTouchDataThread = false;
	m_iReceivedBytes = 0;
	m_iSize = 0;

	m_ChannelXNum = '0';
	m_ChannelYNum = '0';
	InitializeCriticalSection(&m_CriticalSection);

	m_hGetRawDataThread = NULL;
	m_hWnd = NULL;
	m_hGetTouchDataThread = NULL;

	m_iMaxPoints = 5;
}

CMyOperator::~CMyOperator(void)
{
	if(INVALID_HANDLE_VALUE != m_hDevice)
	{
		CloseHandle(m_hDevice);
	}

	DeleteCriticalSection(&m_CriticalSection);

	if(NULL != m_hGetRawDataThread)
	{
		CloseHandle(m_hGetRawDataThread);
	}

	if(NULL != m_hThread)
	{
		CloseHandle(m_hThread);
	}
}

HIDD_ATTRIBUTES CMyOperator::GetDevicesInfo(void)
{
	HIDD_ATTRIBUTES attri;

	InitDevicesInfo(attri);

	HANDLE handle = INVALID_HANDLE_VALUE;
	BOOL bRet = FALSE;
	GUID hidGuid;
	HDEVINFO hardwareDeviceInfo;
	SP_DEVINFO_DATA hwInfoData;
	SP_INTERFACE_DEVICE_DATA deviceInfoData;
	PSP_INTERFACE_DEVICE_DETAIL_DATA functionClassDeviceData = NULL;
	ULONG predictedLength = 0;
	ULONG requiredLength = 0;
	ULONG hwpredictedLength = 254;
	ULONG hwrequiredLength = 254;
	char cbuf[512]={0};
	unsigned char ucHidVersionNum = 0;

	deviceInfoData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);
	HidD_GetHidGuid(&hidGuid); //Get HID Device's GUID
	hardwareDeviceInfo = SetupDiGetClassDevs(&hidGuid, NULL, NULL, (DIGCF_PRESENT|DIGCF_DEVICEINTERFACE));
	
	for (int i=0; i<128; i++)
	{
		if (!SetupDiEnumDeviceInterfaces(hardwareDeviceInfo, 0, &hidGuid, i, &deviceInfoData)) continue;
		SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo, &deviceInfoData, NULL, 0, &requiredLength, NULL);//Get length
		predictedLength = requiredLength;
		functionClassDeviceData = (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(predictedLength);
		if (!functionClassDeviceData) continue;
		functionClassDeviceData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
		if (!SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo, &deviceInfoData, functionClassDeviceData, predictedLength, &requiredLength, NULL)) break;

		handle = CreateFile(functionClassDeviceData->DevicePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED/*0*/, NULL);
		if (handle != INVALID_HANDLE_VALUE)
		{
			HidD_GetAttributes(handle, &attri);
			ucHidVersionNum = attri.VersionNumber;
			//if (/*(attri.VendorID == m_usVID) && */(attri.ProductID == m_usPID))
			if( 0x2808 == attri.VendorID || 0x10C4 == attri.VendorID )
			{
				//////////////////////////////只取设备版本号从1到MAX_DEVICE_NUM
				if(ucHidVersionNum >= 0 && ucHidVersionNum <= MAX_DEVICE_NUM)
				{
					if(ucHidVersionNum == 0)//旧版SIU板FW版本号为0，为了兼容，将其与版本号为1的同等处理
					{
						ucHidVersionNum = 1;
					}
					if(m_DeviceState[ucHidVersionNum - 1] != true)//相同版本号只取一个，先来后到
					{
						m_DeviceState[ucHidVersionNum - 1] = true;
						//memset(m_cDevicePath[ucHidVersionNum - 1], 0, sizeof(m_cDevicePath[ucHidVersionNum - 1]));
						//strcpy(m_cDevicePath[ucHidVersionNum - 1], functionClassDeviceData->DevicePath);
						
						m_iDeviceNum++;
						m_hDevice = handle;
						//CloseHandle(handle);
						handle = INVALID_HANDLE_VALUE;
						free(functionClassDeviceData);
						functionClassDeviceData = NULL;	
						
					}
				}
			}			
		}
		//CloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
		free(functionClassDeviceData);
		functionClassDeviceData = NULL;
	}
	SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
	//if(m_iDeviceNum>1) m_iDeviceNum=1;

	m_hThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, NULL);
	return attri;
}

void CMyOperator::InitDevicesInfo(HIDD_ATTRIBUTES &DevicesInfo)
{
	DevicesInfo.ProductID = 0;
	DevicesInfo.Size = 0;
	DevicesInfo.VendorID = 0;
	DevicesInfo.VersionNumber = 0;
}

int CMyOperator::GetDevicesNum() const
{
	return m_iDeviceNum;
}

unsigned char CMyOperator::ReadReg(unsigned char RegAddr, unsigned char *pRegData)
{
	if(m_hDevice == NULL)
	{
		return ERROR_CODE_NO_DEVICE;
	}
		
	unsigned char ReCode = ERROR_CODE_OK;
	unsigned char wBuffer[4];
	unsigned char rBuffer[2];

// 	BYTE i2cdelay = 0;
// 	Comm_Base_GetI2CByteDelay(&i2cdelay);
// 	Comm_Base_SetI2CByteDelay(2);
	wBuffer[0] = RegAddr;

	if(HY_I2C_INTERFACE==m_iCommMode)
	{
		if( ReCode == ERROR_CODE_OK )
		{
			ReCode = HY_IIC_IO(m_hDevice, wBuffer, 1, NULL, 0);
		}
		
		SysDelay(1);

		if( ReCode == ERROR_CODE_OK )
		{
			ReCode = HY_IIC_IO(m_hDevice, NULL, 0, rBuffer, 1);
		}	
	}

	if(ReCode == ERROR_CODE_OK)
	{
 		*pRegData = rBuffer[0];
	}

	return ReCode;
}

//***********************************************
//写寄存器
//***********************************************
unsigned char CMyOperator::WriteReg(unsigned char RegAddr, unsigned char RegData)
{
	if(m_hDevice == NULL)
	{
		return ERROR_CODE_NO_DEVICE;
	}	

	unsigned char ReCode = ERROR_CODE_OK;
	unsigned char wBuffer[4];

	wBuffer[0] = RegAddr;
	wBuffer[1] = RegData;

	if( ReCode == ERROR_CODE_OK )
	{
		ReCode = HY_IIC_IO(m_hDevice, wBuffer, 2, NULL, 0);
	}

	return ReCode;
}


/*
//===========================================================
//获取IIC发送字节时的时间延迟，单位us
//===========================================================
int CMyOperator::HY_Get_IIC_ByteDelay(BYTE *pByteDelay)
{
	unsigned char i=0;
	unsigned char ReCode = ERROR_CODE_OK;
	unsigned char CmdPacket[REPORT_SIZE+1];
	memset(CmdPacket, 0xff, REPORT_SIZE+1);

	CmdPacket[0] = G_REPORT_ID_OUT;
	CmdPacket[3] = 5;
	CmdPacket[4] = CMD_GET_IIC_BYTE_DELAY;
	CmdPacket[5] = GetChecksum(CmdPacket+1, CmdPacket[3]-1);

	for(i=0; i<COMMAND_REPEAT_TIMES; i++)
	{
		{
			ReCode = USB_SendCmd(CmdPacket, 1, 1);
			if(ReCode == ERROR_CODE_OK)
			{
				if(GetChecksum(m_InBuffer+1, m_InBuffer[3]-1) != m_InBuffer[m_InBuffer[3]])	//checksum
				{
					ReCode = ERROR_CODE_CHECKSUM_ERROR;
				}
				else
				{
					if(m_InBuffer[4] == CMD_GET_IIC_BYTE_DELAY)
					{
						*pByteDelay = m_InBuffer[5];
						ReCode = ERROR_CODE_OK;
						break;
					}
					else if(m_InBuffer[4] == CMD_NACK)
					{
						ReCode = m_InBuffer[5];
					}
					else
					{
						ReCode = ERROR_CODE_PACKET_RE_ERROR;
					}
				}
			}
		}
	}

	return ReCode;
}
*/

//=============================================
//获取校验
//=============================================
unsigned char CMyOperator::GetChecksum(unsigned char *pData, unsigned char BytesNum)
{
	unsigned int i=0;
	unsigned char Checksum=0;
	for(i=0; i<BytesNum; i++)
	{
		Checksum ^= pData[i];
	}
	Checksum++;
	return Checksum;
}

//===========================================================
//所有命令数据包最后均通过此函数发送到USB端口，然后等待命令反馈
//数据包，为使命令顺序发送，函数中采用临界区机制
//CmdPacketsNum表示数据包数目，RePacketsNum表示FW应该返回的数据包数目
//基本上设置命令都是单数据包, SPI_IO, IIC_IO可能出现多数据包
//===========================================================
unsigned char CMyOperator::USB_SendCmd(unsigned char *pCmdPackets, unsigned char CmdPacketsNum, unsigned char RePacketsNum)
{
	EnterCriticalSection(&m_CriticalSection);

	unsigned char *pCmd = NULL;
	unsigned char i=0;
	DWORD dwBytesSucceed = 0;

	long WaitResult = 0;
	unsigned char ReCode = ERROR_CODE_OK;
	OVERLAPPED oWrite;

	memset(m_InBuffer, 0xff, sizeof(m_InBuffer));
	m_PacketNumToReturn = RePacketsNum;
	m_RePacketCounter = 0;

	memset((char*)&oWrite, 0, sizeof(OVERLAPPED));
 	CString str;
 	str.Format(_T("Write_Event_%d"), m_iDeviceNum);
 	oWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, str);
	//oWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	ResetEvent(m_hUSBReadEvent);

	CancelIo(m_hDevice);

#if 1
	for(i=0; i<CmdPacketsNum; i++)		//send command packets
	{
		pCmd = pCmdPackets+i*(REPORT_SIZE+1);
		ResetEvent(oWrite.hEvent);

		HidD_FlushQueue(m_hDevice);
		if(::WriteFile(m_hDevice, pCmd, MAX_ONCE_WRITE_LENGTH/*REPORT_SIZE+1*/, 0, &oWrite))
		{
			ReCode = ERROR_CODE_OK;
		}
		else
		{
			DWORD dError = GetLastError();
			if(dError == ERROR_IO_PENDING)
			{

				WaitResult = WaitForSingleObject(oWrite.hEvent, USB_RW_TIMEOUT);				
				if(WaitResult == WAIT_OBJECT_0)
				{
					ReCode = ERROR_CODE_OK;
				}
				else
				{
					CancelIo(m_hDevice);
					ReCode = ERROR_CODE_WRITE_USB_ERROR;
					break;
				}
			}
			else
			{
				CancelIo(m_hDevice);
				ReCode = ERROR_CODE_WRITE_USB_ERROR;
				break;
			}
		}

	}

#endif


	if(ReCode == ERROR_CODE_OK)		//wait and receiving return packets
	{
		WaitResult = WaitForSingleObject(m_hUSBSendEvent, USB_RW_TIMEOUT);		
		if(WaitResult != WAIT_OBJECT_0)
		{
			ReCode = ERROR_CODE_WAIT_RESPONSE_TIMEOUT;
		}
		else
		{
			ReCode == ERROR_CODE_OK;
		}
	}

	CloseHandle(oWrite.hEvent);
	LeaveCriticalSection(&m_CriticalSection);
	return ReCode;
}


//===========================================================
//IIC 读写数据,当写出的数据量较大时，分多个数据包发送
//数据包全部放在CmdPackets中
//===========================================================
int CMyOperator::HY_IIC_IO(HANDLE cyHandle, BYTE *pBufferWrite, unsigned short BytesToWrite, BYTE *pBufferRead, unsigned short BytesToRead)
{
	if(BytesToWrite > MAX_WRITE_DATA_LEN || BytesToRead > MAX_READ_DATA_LEN) return ERROR_CODE_INVALID_PARAM;
	if((BytesToWrite == 0) && (BytesToRead == 0)) return ERROR_CODE_INVALID_PARAM;
	if(pBufferRead == NULL)	BytesToRead = 0;

	unsigned char CmdPacketsNum=0, RePacketsNum=0;
	unsigned short j=0;
	unsigned int BytesCounter=0;
	unsigned char ReCode = ERROR_CODE_OK;
	unsigned char CmdPackets[MAX_PACKETS_NUM*(REPORT_SIZE+1)];
	unsigned char *pPacket=NULL;

	memset(CmdPackets, 0xff, MAX_PACKETS_NUM*(REPORT_SIZE+1));
	if(1/*COMM_MODE_NORMAL == G_CommMode*/)
	{
		//将要发送的所有数据封装成数据包，每个数据包均为REPORT_SIZE+1字节
		for(CmdPacketsNum=1; CmdPacketsNum<MAX_PACKETS_NUM; CmdPacketsNum++)
		{
			CmdPackets[j] = m_REPORT_ID_OUT;
			CmdPackets[j+1] = CmdPackets[j+2] = 0xff;
			CmdPackets[j+4] = CMD_IIC_IO;
			if(CmdPacketsNum == 1)	//first packet
			{
				CmdPackets[j+5] = 0x00;
				CmdPackets[j+6] = (BytesToWrite>>8);
				CmdPackets[j+7] = (unsigned char)BytesToWrite;
				CmdPackets[j+8] = (BytesToRead>>8);
				CmdPackets[j+9] = (unsigned char)BytesToRead;
				if(BytesCounter+(REPORT_SIZE-10) > BytesToWrite)
				{
					CmdPackets[j+3] = BytesToWrite-BytesCounter+10;
					memcpy(CmdPackets+j+10, pBufferWrite+BytesCounter, BytesToWrite-BytesCounter);
					BytesCounter = BytesToWrite; 
				}
				else
				{
					CmdPackets[j+3] = REPORT_SIZE;
					memcpy(CmdPackets+j+10, pBufferWrite+BytesCounter, (REPORT_SIZE-10));
					BytesCounter += (REPORT_SIZE-10);
				}
			}
			else
			{
				CmdPackets[j+5] = 0x01;
				if(BytesCounter+(REPORT_SIZE-6) > BytesToWrite)
				{
					CmdPackets[j+3] = BytesToWrite-BytesCounter+6;
					memcpy(CmdPackets+j+6, pBufferWrite+BytesCounter, BytesToWrite-BytesCounter);
					BytesCounter = BytesToWrite; 
				}
				else
				{
					CmdPackets[j+3] = REPORT_SIZE;
					memcpy(CmdPackets+j+6, pBufferWrite+BytesCounter, (REPORT_SIZE-6));
					BytesCounter += (REPORT_SIZE-6);
				}
			}
			CmdPackets[j+CmdPackets[j+3]] = GetChecksum(CmdPackets+j+1, CmdPackets[j+3]-1);

			j += (REPORT_SIZE+1);
			if(BytesCounter >= BytesToWrite) break;
		}

		if(BytesToRead == 0) 
			RePacketsNum = 1;
		else 
			((BytesToRead%(REPORT_SIZE-6)) == 0) ? RePacketsNum = BytesToRead/(REPORT_SIZE-6) : RePacketsNum = BytesToRead/(REPORT_SIZE-6)+1;

		for(int iTimes = 0; iTimes < COMMAND_REPEAT_TIMES; iTimes++)
		{
			ReCode = USB_SendCmd(CmdPackets, CmdPacketsNum, RePacketsNum);
			if(ReCode == ERROR_CODE_OK)
			{
				BytesCounter = 0;
				for(j=0; j<RePacketsNum; j++)
				{

					DWORD dw = WaitForSingleObject(m_hUSBSendEvent, USB_RW_TIMEOUT);
					//pPacket = m_InBuffer+j*(REPORT_SIZE+1);
					pPacket = m_InBuffer;
					ResetEvent(m_hUSBSendEvent);


					unsigned char ch = GetChecksum(pPacket+1, pPacket[3]-1);
					if(ch != pPacket[pPacket[3]])
					{
						ReCode = ERROR_CODE_CHECKSUM_ERROR;
						break;
					}
					else
					{
						if(pPacket[4] == CMD_IIC_IO)
						{
							if(BytesToRead > 0)
							{
								if((pPacket[5] == FIRST_PACKET) || (pPacket[5] == SERIAL_PACKET))
								{
									memcpy(pBufferRead+BytesCounter, pPacket+6, pPacket[3]-6);		//获取读到的数据
									BytesCounter += (pPacket[3]-6);
								}
								else
								{
									ReCode = ERROR_CODE_PACKET_RE_ERROR;
									break;
								}
							}
							else
							{
								ReCode = ERROR_CODE_OK;
								break;
							}
						}
						else if(pPacket[4] == CMD_NACK)
						{
							ReCode = pPacket[5];
							break;
						}
						else
						{
							ReCode = ERROR_CODE_PACKET_RE_ERROR;
							break;
						}
					}
				}
			}
			///////如果写失败，重复三次
			if(ReCode == ERROR_CODE_OK)break;
		}
	}

	return ReCode;
}

void CMyOperator::SysDelay(unsigned int DelayMS)	//实现系统精确延时，ms级
{
	LARGE_INTEGER Param;
	LONGLONG StartTime, EndTime;
	double SysFreq;

	QueryPerformanceFrequency(&Param);
	SysFreq = Param.QuadPart;
	QueryPerformanceCounter(&Param);
	StartTime = Param.QuadPart;
	do
	{
		QueryPerformanceCounter(&Param);
		EndTime = Param.QuadPart;
	}while((EndTime-StartTime)*1000/SysFreq<DelayMS);
}

DWORD WINAPI CMyOperator::ThreadProc(LPVOID lp)
{
	CMyOperator *pOperator = (CMyOperator*)lp;

	OVERLAPPED oRead;
	long WaitResult = 0;
	//unsigned char InBuffer[(REPORT_SIZE+1)*20];
	unsigned char InBuffer[0x8000];
	DWORD dwBytesSucceed = 0;

	DWORD iStartTime=0;//Calc Report rates
	DWORD iEndTime=0;	
	DWORD iStartCount=0;
	DWORD iEndCount=0;	
	DWORD iSiuCount=0;
	DWORD iIntNum=0;

	while(!pOperator->m_bStopThread)
	{
		if(pOperator->m_hDevice != INVALID_HANDLE_VALUE)
		{			
			memset((char*)&oRead, 0, sizeof(OVERLAPPED));
			oRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			//ResetEvent(pOperator->G_hCloseDeviceEvent);
			ResetEvent(oRead.hEvent);
			bool bDataComing = false;


			if(::ReadFile(pOperator->m_hDevice, InBuffer, pOperator->m_iOneRowLength, 0, &oRead))
			{
				bDataComing = true;						
				CloseHandle(oRead.hEvent);
			}
			else
			{
				DWORD dError = GetLastError(); 
				if(dError == ERROR_IO_PENDING)
				{
					WaitResult = WaitForSingleObject(oRead.hEvent, USB_RW_TIMEOUT/*COM_RW_TIMEOUT*/);							
					CloseHandle(oRead.hEvent);						

					if(WaitResult == WAIT_OBJECT_0)
					{
						bDataComing=true;					    
					}
					else
					{
						CancelIo(pOperator->m_hDevice);
					}
				}
				else
				{
					::Sleep(50);
					CancelIo(pOperator->m_hDevice);
				}
			}


			if(bDataComing)
			{
 				memcpy(pOperator->m_InBuffer, InBuffer, oRead.InternalHigh);	//命令反馈数据包
				SetEvent(pOperator->m_hUSBSendEvent);

				
				pOperator->m_iReceivedBytes+=oRead.InternalHigh;
				if(pOperator->m_REPORT_ID_IN != pOperator->m_InBuffer[0]|| 0xff!=pOperator->m_InBuffer[1]||0xff!=pOperator->m_InBuffer[2])//Find the packet head
				{
					for(int n=0; n<pOperator->m_iReceivedBytes; n++)
					{
						if(pOperator->m_REPORT_ID_IN == pOperator->m_InBuffer[n]&&0xff==pOperator->m_InBuffer[n+1]&&0xff==pOperator->m_InBuffer[n+2]) 
						{
							memcpy(pOperator->m_InBuffer, pOperator->m_InBuffer+n, pOperator->m_iReceivedBytes-n);
							pOperator->m_iReceivedBytes-=n;
							break;
						}
					}
				}
								
// 				if(pOperator->m_iReceivedBytes>=MAX_ONCE_READ_LENGTH)
// 				{
// 					for(int n=0; n+MAX_ONCE_READ_LENGTH<=pOperator->m_iReceivedBytes; n=n+MAX_ONCE_READ_LENGTH)
// 					{
// 						if(0x32!=pOperator->m_InBuffer[n+4]&&CMD_TOUCH_FLAG!=pOperator->m_InBuffer[n+4]&&CMD_IO_LEVEL_SIGNAL!=pOperator->m_InBuffer[n+4]&&CMD_SAMSUNG_IIC_IO!=pOperator->m_InBuffer[n+4]) 
// 						{
// 							memcpy(InBuffer, pOperator->m_InBuffer+n, MAX_ONCE_READ_LENGTH);
// 							memcpy(InBuffer+MAX_ONCE_READ_LENGTH, pOperator->m_InBuffer, n);
// 							memcpy(InBuffer+MAX_ONCE_READ_LENGTH+n, pOperator->m_InBuffer+MAX_ONCE_READ_LENGTH+n, pOperator->m_iReceivedBytes-MAX_ONCE_READ_LENGTH-n);
// 							//memcpy(m_InBuffer, InBuffer, 64);
// 							memcpy(pOperator->m_InBuffer, InBuffer+MAX_ONCE_READ_LENGTH, pOperator->m_iReceivedBytes-MAX_ONCE_READ_LENGTH);
// 							pOperator->m_iReceivedBytes-=MAX_ONCE_READ_LENGTH;
// 
// 							memcpy(pOperator->m_InBuffer+(MAX_ONCE_READ_LENGTH)*pOperator->m_RePacketCounter, InBuffer, MAX_ONCE_READ_LENGTH);	//命令反馈数据包
// 							if(++pOperator->m_RePacketCounter == pOperator->m_PacketNumToReturn)	
// 								SetEvent(pOperator->m_hUSBSendEvent);
// 
// 							//Sleep(1);
// 							break;
// 						}
// 					}
// 				}

				if(pOperator->m_iReceivedBytes >= pOperator->m_iOneRowLength)
				{
					pOperator->m_iReceivedBytes=pOperator->m_iReceivedBytes - pOperator->m_iOneRowLength;
					if(pOperator->m_iReceivedBytes<0 || pOperator->m_iReceivedBytes > pOperator->m_iOneRowLength) pOperator->m_iReceivedBytes=0;
					if(pOperator->m_InBuffer[4] == CMD_TOUCH_FLAG)
					{	
						//if(pOperator->G_hWnd != NULL)
						{                            
							if(pOperator->m_InBuffer[5] == 0x01) 
							{	
								SetEvent(pOperator->m_hTouchEvent);
							}
							else if(pOperator->m_InBuffer[5] == 0x02)
							{	
								if((pOperator->m_InBuffer[6] == 0xaa)&&(pOperator->m_InBuffer[7] == 0xaa))
								{
                                    
									SetEvent(pOperator->m_hTouchEvent);
									
								}
								else if((pOperator->m_InBuffer[6] == 0xbb)&&(pOperator->m_InBuffer[7] == 0xbb))
								{
									SetEvent(pOperator->m_hTouchEvent);
								}

							}
						}
					}
									
				}//if	*/						
			} //if		
		}
	}

	return 0;
}

unsigned char CMyOperator::GetRawData(void)
{
	int LineNum=0, i=0, ReCode = ERROR_CODE_OK;
	CString str, strTemp;
	//m_strRawData = "";
	//m_strDifferData = "";

	int rawData[TX_NUM_MAX * RX_NUM_MAX] = {0};
	if(1/*ReCode == ERROR_CODE_OK*/)	
	{
		//m_strCurrentTestMsg = "Start Scan...";
		//SysDelay(50);
		ReCode = StartScan(1);
		if(ReCode == ERROR_CODE_OK)
		{
			memset(m_RawData, 0, sizeof(m_RawData));
			//--------------------------------------------Read RawData
			ReCode = ReadRawData( 0, 0xAD,  m_ChannelXNum * m_ChannelYNum * 2, rawData);
		
			if( ERROR_CODE_OK != ReCode ) 
			{
				return ReCode;
			}

			for ( int iRow = 0; iRow < 18; ++iRow )
			{
				for ( int iCol = 0; iCol < 32; ++iCol )
				{
					m_RawData[iRow][iCol] = rawData[iRow * 32 + iCol];

				}
			}
		}

		return ReCode;
	}
	else
	{
		return ReCode;
	}
}

unsigned char CMyOperator::StartScan(unsigned char Freq)
{
	unsigned char RegVal= 0;
	unsigned char times= 0;
	const unsigned char MaxTimes = 250;	//最长等待160ms
	unsigned char ReCode = ERROR_CODE_COMM_ERROR;

	if(m_hDevice == NULL)
	{
		return ERROR_CODE_NO_DEVICE;
	}

	if(Freq == 0)
	{
		return ERROR_CODE_INVALID_PARAM;
	}

	ReCode = ReadReg(DEVIDE_MODE_ADDR, &RegVal);
	if(ReCode == ERROR_CODE_OK)
	{
		RegVal |= 0x80;		//最高位置1，启动扫描
		ReCode = WriteReg(DEVIDE_MODE_ADDR, RegVal);
		if(ReCode == ERROR_CODE_OK)
		{
			while(times++ < MaxTimes)		//等待扫描完成
			{
				SysDelay(16);	//8ms
				ReCode = ReadReg(DEVIDE_MODE_ADDR, &RegVal);
				if(ReCode == ERROR_CODE_OK)
				{
					if((RegVal>>7) == 0)
						break;
				}
				else
				{
					break;
				}
			}

			if(times < MaxTimes)
			{
				ReCode = ERROR_CODE_OK;
			}
			else
			{
				ReCode = ERROR_CODE_COMM_ERROR;
			}
		}
	}
	return ReCode;
}

unsigned char CMyOperator::ReadRawData(unsigned char Freq, unsigned char LineNum, int ByteNum, int *pRevBuffer)
{
	unsigned char ReCode=ERROR_CODE_COMM_ERROR;
	unsigned char I2C_wBuffer[3];
	unsigned char *pReadData = new unsigned char[ByteNum];
	unsigned char *pReadDataTmp = new unsigned char[ByteNum*2];

	unsigned short BytesNumInTestMode3=0, BytesNumInTestMode2=0,BytesNumInTestMode1=0;
    unsigned short BytesNumInTestMode6=0, BytesNumInTestMode5=0,BytesNumInTestMode4=0;

	if(m_hDevice == NULL)
	{
		return ERROR_CODE_NO_DEVICE;
	}		
	if(pReadData == NULL)
	{
		return ERROR_CODE_ALLOCATE_BUFFER_ERROR;
	}		

	int iReadNum=ByteNum/342;

	if(0 != (ByteNum%342))
	{
		iReadNum++;
	}

	if(ByteNum <= 342)
	{
		BytesNumInTestMode1 = ByteNum;		
	}
	else
	{
		BytesNumInTestMode1 = 342;
	}
	

    ReCode = WriteReg(REG_LINE_NUM, LineNum);//Set row addr;

	if(HY_I2C_INTERFACE == m_iCommMode)
	{	
		I2C_wBuffer[0] = REG_RawBuf0;	//set begin address
		if(ReCode == ERROR_CODE_OK)
		{
			ReCode = HY_IIC_IO(m_hDevice, I2C_wBuffer, 1, pReadData, BytesNumInTestMode1);
			//ReCode = HY_IIC_IO(m_hDevice, I2C_wBuffer, 1, pReadData, 0);
		}

		for(int i=1; i<iReadNum; i++)
		{
			if(ReCode != ERROR_CODE_OK) break;

			if(i==iReadNum-1)//last packet
			{
				ReCode = HY_IIC_IO(m_hDevice, NULL, 0, pReadData+342*i, ByteNum-342*i);
			}
			else
			{
				ReCode = HY_IIC_IO(m_hDevice, NULL, 0, pReadData+342*i, 342);	
			}	
		}

	}
	
	if(ReCode == ERROR_CODE_OK)
	{
		for(int i=0; i<(ByteNum>>1); i++)
		{
			pRevBuffer[i] = (short)(pReadData[i<<1]<<8)+pReadData[(i<<1)+1];
			//if(pRevBuffer[i] & 0x8000)//有符号位
			//{
			//	pRevBuffer[i] -= 0xffff + 1;
			//}
		}
	}

	delete []pReadData;
	delete []pReadDataTmp;
	return ReCode;
}

void CMyOperator::SetChannelNum(unsigned char ChannelXNum, unsigned char ChannelYNum)
{
	m_ChannelXNum = ChannelXNum;
	m_ChannelYNum = ChannelYNum;
}

DWORD WINAPI CMyOperator::GetDataThreadProc(LPVOID lp)
{
	CMyOperator *pMyOperator = (CMyOperator *)lp;

	////////////设置FIR，0：关闭，1：打开
	unsigned char ucRawRegData = 0;
	unsigned char ReCode;

	pMyOperator->ReadReg(0xFB, &ucRawRegData);
	pMyOperator->SysDelay(50);
	pMyOperator->WriteReg(0xFB, 1);
	pMyOperator->SysDelay(50);

	while(!pMyOperator->m_StopGetDataThread)
	{
		

// 		for(int i = 0 ; i < 3; i++)//取第三帧
// 		{
			ReCode = pMyOperator->GetRawData();//读取RawData
//		}

		if(ReCode != ERROR_CODE_OK) 
		{
			continue;
		}

		::SendMessage(pMyOperator->GetDlgHWnd(), ON_MYEVENT_SNED, (WPARAM)(&(pMyOperator->m_RawData)), 0);
		//::PostMessage(pMyOperator->GetDlgHWnd(), ON_MYEVENT_SNED, (WPARAM)(&(pMyOperator->m_RawData)), 0);
	}

	pMyOperator->WriteReg(0xFB, ucRawRegData);//恢复原值

	//激活获取RawData完成的信号
	//SetEvent(pMyOperator->m_hGetDataEndEvent);

	return 0;
}

DWORD WINAPI CMyOperator::TouchDataThreadProc(LPVOID lp)
{
	CMyOperator *pMyOperator = (CMyOperator *)lp;
	unsigned char buffer[TOUCH_BUFFER_LEN + sizeof(TouchDataStruct)] = {0};
	while (!pMyOperator->m_bTouchDataThread)
	{
		ResetEvent(pMyOperator->m_hTouchEvent);
		DWORD  dwRes = ::WaitForSingleObject(pMyOperator->m_hTouchEvent, 200);

		if(WAIT_OBJECT_0 == dwRes)
		{
			//读到TouchData
			TouchDataStruct* pTouchStu = (TouchDataStruct*)(buffer+TOUCH_BUFFER_LEN);

			pMyOperator->ReadTouchData( true, pMyOperator->m_iMaxPoints, buffer, *pTouchStu);

			//必须使用SendMessage
			::SendMessage(pMyOperator->GetDlgHWnd(), WM_TOUCH_MESSAGE, (WPARAM)pTouchStu, 0);
		}		
	}

	return 0;
}

void CMyOperator::TestItem_RawDataTest()
{
	if(NULL == m_hGetRawDataThread)
	{
		m_StopGetDataThread = false;
		m_hGetRawDataThread = CreateThread(NULL, 0, GetDataThreadProc, (LPVOID)this, 0, NULL);
	}
}

void CMyOperator::TestItem_StopRawDataTest()
{
	if(!m_StopGetDataThread)
	{
		m_StopGetDataThread = true;

		if(NULL != m_hGetRawDataThread)
		{
			Sleep(50);
			WaitForSingleObject(m_hGetRawDataThread, INFINITE);
			CloseHandle(m_hGetRawDataThread);
			m_hGetRawDataThread = NULL;
		}
	}
}

void CMyOperator::StartTouchData()
{
	if(NULL == m_hGetTouchDataThread)
	{
		m_bTouchDataThread = false; 
		m_hGetTouchDataThread = CreateThread(NULL, 0, TouchDataThreadProc, (LPVOID)this, 0, NULL);
	}
}

void CMyOperator::EndTouchData()
{
	if(!m_bTouchDataThread)
	{
		m_bTouchDataThread = true;

		if(NULL != m_hGetTouchDataThread)
		{
			//WaitForSingleObject(m_hGetTouchDataThread, INFINITE);
			CloseHandle(m_hGetTouchDataThread);
			m_hGetTouchDataThread = NULL;
		}
	}

}

bool CMyOperator::SetDlgHWnd(HWND hWnd)
{
	if(NULL == hWnd)
	{
		return false;
	}

	m_hWnd = hWnd;

	return true;
}

HWND CMyOperator::GetDlgHWnd() const
{
	return m_hWnd;
}


// ***********************************************
// 读取触摸数据
// ***********************************************
unsigned char CMyOperator::ReadTouchData(bool bIsPointID, unsigned char iMaxPoints, unsigned char *pTouchData, struct TouchDataStruct& TouchDataProcessed/*返回触摸数据的结构体*/)
{	
	unsigned char Checksum=0, ReCode=ERROR_CODE_COMM_ERROR;
	unsigned char I2C_wBuffer[100];
	unsigned char I2C_rBuffer[100/*TOUCH_DATA_LEN*/];
	unsigned char *pReadData = NULL;
	unsigned int BytesNumToRead = 0;

	//CString str;

	if(m_hDevice == NULL)
	{
		return ERROR_CODE_COMM_ERROR;
	}		

	if(1/*iCommMode == HY_I2C_INTERFACE*/)
	{
		pReadData = I2C_rBuffer;

		BytesNumToRead = 2+6*iMaxPoints;			//手势、点数和坐标一次读取
		I2C_wBuffer[0] = TOUCH_POINTS_AND_GESTID_ADDR;

		ReCode = HY_IIC_IO(m_hDevice, I2C_wBuffer, 1, I2C_rBuffer, BytesNumToRead);

		if(ReCode == ERROR_CODE_OK)
		{
			pTouchData[0] = BytesNumToRead;		//存放数据长度	
			memcpy(pTouchData+1, pReadData, BytesNumToRead);

			TouchDataProcessed.clear();
			TouchDataProcessed.bpoints = pReadData[1]&0x0f;	//point num
			TouchDataProcessed.bSign = I2C_rBuffer[0];  //gesture

			if(TouchDataProcessed.bpoints > MAX_TOUCH_POINTS)
			{
				TouchDataProcessed.clear();
				ReCode = ERROR_CODE_COMM_ERROR;
			}
			else
			{
				if(bIsPointID)  //包含点序号
				{
					unsigned char PointID = 0;
					for(int i=0; i<iMaxPoints/*TouchDataProcessed.bpoints*/; i++)
					{
						PointID = ((pReadData[4+i*6])>>4); //y轴高字节第 1-5bit
						if(PointID < MAX_TOUCH_POINTS)
						{
							TouchDataProcessed.bEvent[PointID] = ((pReadData[2+i*6])>>6);  //x轴高字节2bit, event
							if((TouchDataProcessed.bEvent[PointID] == EVENT_DOWN) || (TouchDataProcessed.bEvent[PointID] == EVENT_CONTACT))
							{
								TouchDataProcessed.Points[PointID].x = ((pReadData[2+i*6]&0x0f)<<8)+pReadData[3+i*6];
								TouchDataProcessed.Points[PointID].y = ((pReadData[4+i*6]&0x0f)<<8)+pReadData[5+i*6];
								TouchDataProcessed.bTouchArea[PointID] = pReadData[7+i*6];
								TouchDataProcessed.bStrength[PointID] = pReadData[6+i*6];
							}
						}	
					}
				}
				else
				{
					for(int i=0;i<TouchDataProcessed.bpoints;i++)
					{
						TouchDataProcessed.Points[i].x = ((pReadData[2+i*6]&0x0f)<<8)+pReadData[3+i*6];
						TouchDataProcessed.Points[i].y = ((pReadData[4+i*6]&0x0f)<<8)+pReadData[5+i*6];	
						TouchDataProcessed.bTouchArea[i] = pReadData[7+i*6];
						TouchDataProcessed.bStrength[i] = pReadData[6+i*6];
					}
				}
				ReCode = ERROR_CODE_OK;
			}
		}
	}

	return ReCode;
}

unsigned char CMyOperator::EnterFactory()
{
	unsigned char RunState = 0;
	unsigned char ReCode = ERROR_CODE_COMM_ERROR;

	ReCode = ReadReg(DEVIDE_MODE_ADDR, &RunState);
	if(ReCode == ERROR_CODE_OK)
	{
		if(((RunState>>4)&0x07) == 0x04)	//factory
		{
			ReCode = ERROR_CODE_OK;
		}
		else
		{
			ReCode = WriteReg(DEVIDE_MODE_ADDR, 0x40);
			if(ReCode == ERROR_CODE_OK)
			{
				ReCode = ReadReg(DEVIDE_MODE_ADDR, &RunState);
				if(ReCode == ERROR_CODE_OK)
				{	
					if(((RunState>>4)&0x07) == 0x04)
					{
						ReCode = ERROR_CODE_OK;
					}
					else
					{
						ReCode = ERROR_CODE_COMM_ERROR;
					}
				}
			}
		}
	}

	return ReCode;
}

unsigned char CMyOperator::EnterWork()
{
	unsigned char RunState = 0;
	unsigned char ReCode = ERROR_CODE_COMM_ERROR;

	ReCode = ReadReg(DEVIDE_MODE_ADDR, &RunState);
	if(ReCode == ERROR_CODE_OK)
	{
		if(((RunState>>4)&0x07) == 0x00)	//work
		{
			ReCode = ERROR_CODE_OK;
		}
		else
		{
			ReCode = WriteReg(DEVIDE_MODE_ADDR, 0);
			if(ReCode == ERROR_CODE_OK)
			{
				ReCode = ReadReg(DEVIDE_MODE_ADDR, &RunState);
				if(ReCode == ERROR_CODE_OK)
				{	
					if(((RunState>>4)&0x07) == 0x00)	ReCode = ERROR_CODE_OK;
					else	ReCode = ERROR_CODE_COMM_ERROR;
				}
			}
		}
	}

	return ReCode;
}