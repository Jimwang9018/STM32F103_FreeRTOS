#include "esp8266.h"
#include "wifiSendTask.h"
#include "powerTask.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "usart3.h"
#include <string.h>
#include <stdio.h>
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "delay.h"
#include "main.h"
#include "esp8266.h"
#include "flashFunction.h"
#include "key.h"
#include "led.h"


void wifiSetMode(void);
void wifiTransMode(void);
void wifiFree(void);

static void ErrManger(u8 errData)
{
		if(errData != 0)
		{
			if(BLE.IsLink() == true)												/*蓝牙连接上时通过蓝牙传输数据*/
					BLE.SendErrData(errData);
		}
}


void vWifiSendTask(void *pvParameters)
{				
		xSemaphoreTake( Usart3.xSemaphoreUsart3ReadFull, ( portTickType ) 0 );
	
    while(1)
    {
				
        if(WIFI.mode == WIFI_SET_MODE)
				{		
						WIFI.active = true;
						wifiSetMode();
				}
				
				else if(WIFI.mode == WIFI_TRANS_MODE)
				{		
						WIFI.active = true;
						wifiTransMode();
				}
				
				else																	//判断刷牙上传条件：1、刷牙时间超过1分钟。2、电量足够
				{				
						wifiFree();
						vTaskDelay(50/portTICK_RATE_MS );
				}
    }
}


void wifiSetMode()
{
		u8 temp = 0;
		static u8 cont = 0;
		static bool isLink = false;
		
		switch(WIFI.setStep)
		{
				case SET_STEP_LIST_AP :
															{		
																	if( xSemaphoreTake( WIFI.xSemaphoreInitWIFI, (portTickType)30000 ) == pdTRUE )
																	{
																			temp = WIFI.Init();
																			if(temp == 0)
																			{
																					BLE.SendSetWIFIData(OPEN_WIFI_SUCCESS);
																			}else{
																					BLE.SendErrData(temp);
																					WIFI.setStep = ABNORMAL_EXIT;
																					break;
																			}
																																		
																			temp = WIFI.ListAP();
																			if(temp == 0)
																			{
																					BLE.SendSetWIFIData(LIST_AP_SUCCESS);
																					WIFI.setStep = SET_STEP_LINK;
																			}else{
																					BLE.SendErrData(temp);
																					WIFI.setStep = ABNORMAL_EXIT;
																					break;
																			}							
																	}else{
																			WIFI.setStep = ABNORMAL_EXIT;
																			break;
																	}																																																	
															}	break;
							
				case SET_STEP_LINK :
													{
															if( xSemaphoreTake( WIFI.xSemaphoreAddAP, (portTickType)40000 ) == pdTRUE )
															{		
																	if(WIFI.APPOverFlag == 1)
																	{																																			
																			WIFI.APPOverFlag = 0;
																			WIFI.setStep = ABNORMAL_EXIT;
																			break;
																	}else{
																			temp = WIFI.AddAP();
																			if(temp == 0)
																			{
																					BLE.SendSetWIFIData(ADD_AP_SUCCESS);
																					WIFI.setStep = SET_REIGSTER_SERVER;
																			}else{
																					WIFI.setStep = ABNORMAL_EXIT;
																					BLE.SendErrData(temp);
																					break;
																			}																			
																	}																																
															}else{
																	WIFI.setStep = ABNORMAL_EXIT;
																	break;
															}
																																		
													} break;
							
				case SET_REIGSTER_SERVER :
															{																																	
																	if(xSemaphoreTake( WIFI.xSemaphoreRegisterServer, (portTickType)40000 ) == pdTRUE )
																	{		
																			if(isLink == false)
																			{
																					temp = WIFI.AddServer();
																					if(temp == 0)
																					{
																							BLE.SendSetWIFIData(LINK_SERVER_SUCCESS);
																					}else{
																							BLE.SendErrData(temp);
																							break;
																					}
																					
																					temp = WIFI.UnvarnishSend();
																					if(temp == 0)
																					{
																							BLE.SendSetWIFIData(SET_UNVARNISH_SUCCESS);
																					}else{																					
																							BLE.SendErrData(temp);
																							break;
																					}
																					isLink = true;
																			}																				
																																																																										
																			FlashFun.readUserID();
																			WIFI.RegisterToServer();
																			
																			if( xSemaphoreTake( Usart3.xSemaphoreUsart3ReadFull, ( portTickType ) 10000 ) == pdTRUE )
																			{																				
																					if(Usart3.RxBuff.data[0] == 0x88)
																					{
																							if(Usart3.RxBuff.data[6] == 0x01)
																							{																						
																									FlashFun.storgeLinkData(WIFI.linkData);
																									FlashFun.saveRegisterData();
																									BLE.SendSetWIFIData(REIGSTER_SERVER_SUCCESS);
																									WIFI.setStep = SET_OVER;
																									cont = 0;																								
																									break;
																							}
																					}
																					
																			} else
																			{
																					if(++cont < 3)
																					{
																							BLE.SendErrData(0x16);																																							
																							break;	
																					}else{
																							cont = 0;
																							BLE.SendErrData(0x16);
																							WIFI.setStep = SET_OVER;
																							break;	
																					}																																									
																			}
																	}else{
																			WIFI.setStep = SET_OVER;
																			break;
																	}																					
															}	break;
							
				case SET_OVER :
											{
													WIFI.OutUnvarnish();
													WIFI.Enable(DISABLE);
													WIFI.mode = WIFI_STANDBY_MODE;
													isLink = false;
													BLE.SendSetWIFIData(WIFI_SET_OVER);
											} break;
				case ABNORMAL_EXIT :
											{
													WIFI.Enable(DISABLE);
													WIFI.mode = WIFI_STANDBY_MODE;
													isLink = false;
													BLE.SendSetWIFIData(WIFI_SET_OVER);
											}
											
				default : break;
							
		}
}


void wifiTransMode()
{
		static u8 sendDataFlag = 0;	
		static u8 temp = 0;
		static u8 tranModeStep = 0;
	
		if(FlashFun.readRegisterData() == 1)
		{
				WIFI.active = true;
					
				if(tranModeStep == 0)
				{
						WIFI.tranModeTimeCont = 0; /*超时监测清零*/
						temp = WIFI.Init();
						if(temp != 0)
						{
								vTaskDelay( 500/portTICK_RATE_MS );
								ErrManger(temp);
								WIFI.OutUnvarnish();
								WIFI.Enable(DISABLE);
								WIFI.mode = WIFI_STANDBY_MODE;
						}else{
								tranModeStep = 1;
						}
				}
				
				if(tranModeStep == 1)
				{
						WIFI.tranModeTimeCont = 0;	 /*超时监测清零*/
						temp = WIFI.CreatLink();
						if(temp != 0)
						{
								vTaskDelay( 500/portTICK_RATE_MS );
								ErrManger(temp);
								WIFI.OutUnvarnish();
								WIFI.Enable(DISABLE);
								WIFI.mode = WIFI_STANDBY_MODE;
						}else{
								tranModeStep = 2;
						}
				}
				
				if(tranModeStep == 2)
				{
						WIFI.tranModeTimeCont = 0;	 /*超时监测清零*/
						temp = WIFI.UnvarnishSend();
						if(temp != 0)
						{
								vTaskDelay( 500/portTICK_RATE_MS );
								ErrManger(temp);
								WIFI.OutUnvarnish();
								WIFI.Enable(DISABLE);
								WIFI.mode = WIFI_STANDBY_MODE;
						}else{
								tranModeStep = 3;
						}
				}
				
				if(tranModeStep == 3)
				{
						WIFI.tranModeTimeCont = 0;	 /*超时监测清零*/
						FlashFun.brushDataStart			= FlashFun.getStartAddress();
						FlashFun.brushDataStartOld  = FlashFun.brushDataStart;
						FlashFun.brushDataEnd				= FlashFun.getEndAddress();
						FlashFun.brushDataEndOld		= FlashFun.brushDataEnd;			
						FlashFun.readUserID();
						tranModeStep = 4;
				}
				
				if(tranModeStep == 4)
				{
						WIFI.tranModeTimeCont = 0;	 /*超时监测清零*/
						WIFI.isSendData = true;
						sendDataFlag = FLASH_KEEP_READ;					
						while(sendDataFlag == FLASH_KEEP_READ )
						{		
								WIFI.tranModeTimeCont = 0;	 /*超时监测清零*/
								sendDataFlag = FlashFun.readData();
								WIFI.SendDataToServer();
								
						}
						WIFI.isSendData = false;
						FlashFun.eraseSendedData();	
				}
										
		}				
		
		vTaskDelay( 500/portTICK_RATE_MS );
		WIFI.OutUnvarnish();
		WIFI.Enable(DISABLE);
		WIFI.mode = WIFI_STANDBY_MODE;
}


void wifiFree()
{
		static u32 cont = 0;
		static u8 wifiTransDataFlag = 0;
	
		WIFI.active = false;
		if(KEY.status == ON)
		{
				if(++cont >= 1200)  							//计时一分钟 刷牙超过一分钟
				{				
						cont =0;
						wifiTransDataFlag = 1;						
				}							
		}else if(KEY.status == OFF && wifiTransDataFlag == 1)
		{
				wifiTransDataFlag = 0;
				vTaskDelay(2000/portTICK_RATE_MS );
				FlashFun.setStartAddress(FlashFun.brushDataStart);
				FlashFun.setEndAddress(FlashFun.brushDataEnd);
				WIFI.mode = WIFI_TRANS_MODE;
				
				
		}
}
