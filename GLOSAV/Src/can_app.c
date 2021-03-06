#include "can_app.h"
#include "mb_app.h"


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define CAN1_MESSAGE_QUEUE_MAX_LENGTH  32

#define FMS_PGN_CCVS1		0x00FEF1
#define FMS_PGN_EEC2		0x00F003
#define FMS_PGN_LFC			0x00FEE9
#define FMS_PGN_DD			0x00FEFC
#define FMS_PGN_EEC1		0x00F004
#define FMS_PGN_VW			0x00FEEA
#define FMS_PGN_HOURS		0x00FEE5
#define FMS_PGN_SERV		0x00FEC0
#define FMS_PGN_LFE1		0x00FEF2

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

static CanRxMsgTypeDef can1RxMessage;

QueueHandle_t xCAN1_MessageQueue;
//SemaphoreHandle_t	xCAN1_DataMutex;


static void CAN1_Listening_Task(void *pvParameters);
static void CAN2_Sending_Task(void *pvParameters);
void CAN1_Handling_Message(CanRxMsgTypeDef *can1msg);



#define CAN1_LISTENING_STACK_SIZE		128
#define CAN_LISTENING_DELAY					100

#define PGN_MASK 0x07FFF800

void CAN_App_Init(void)
{
		CAN_FilterConfTypeDef hcan1filter;
		hcan1filter.FilterIdHigh =((FMS_PGN_CCVS1<<11)>>16)&0xFFFF;
		hcan1filter.FilterIdLow = (FMS_PGN_CCVS1<<11)&0xFFFF;
		hcan1filter.FilterMaskIdHigh =(PGN_MASK>>16)&0xFFFF;
		hcan1filter.FilterMaskIdLow = PGN_MASK&0xFFFF;
		hcan1filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
		hcan1filter.FilterNumber = 0;
		hcan1filter.FilterMode = CAN_FILTERMODE_IDMASK;
		hcan1filter.FilterScale = CAN_FILTERSCALE_32BIT;
		hcan1filter.FilterActivation = ENABLE;
		hcan1filter.BankNumber = 14;
		HAL_CAN_ConfigFilter(&hcan1, &hcan1filter);
	
			hcan1filter.FilterIdHigh =((FMS_PGN_EEC2<<11)>>16)&0xFFFF;
		hcan1filter.FilterIdLow = (FMS_PGN_EEC2<<11)&0xFFFF;
		hcan1filter.FilterMaskIdHigh =(PGN_MASK>>16)&0xFFFF;
		hcan1filter.FilterMaskIdLow = PGN_MASK&0xFFFF;
		hcan1filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
		hcan1filter.FilterNumber = 1;
		hcan1filter.FilterMode = CAN_FILTERMODE_IDMASK;
		hcan1filter.FilterScale = CAN_FILTERSCALE_32BIT;
		hcan1filter.FilterActivation = ENABLE;
		hcan1filter.BankNumber = 15;
		HAL_CAN_ConfigFilter(&hcan1, &hcan1filter);
	
//--

		hcan1filter.FilterIdHigh =((FMS_PGN_LFC<<11)>>16)&0xFFFF;
		hcan1filter.FilterIdLow = (FMS_PGN_LFC<<11)&0xFFFF;
		hcan1filter.FilterMaskIdHigh =(PGN_MASK>>16)&0xFFFF;
		hcan1filter.FilterMaskIdLow = PGN_MASK&0xFFFF;
		hcan1filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
		hcan1filter.FilterNumber = 2;
		hcan1filter.FilterMode = CAN_FILTERMODE_IDMASK;
		hcan1filter.FilterScale = CAN_FILTERSCALE_32BIT;
		hcan1filter.FilterActivation = ENABLE;
		hcan1filter.BankNumber = 16;
		HAL_CAN_ConfigFilter(&hcan1, &hcan1filter);
		
		hcan1filter.FilterIdHigh =((FMS_PGN_DD<<11)>>16)&0xFFFF;
		hcan1filter.FilterIdLow = (FMS_PGN_DD<<11)&0xFFFF;
		hcan1filter.FilterMaskIdHigh =(PGN_MASK>>16)&0xFFFF;
		hcan1filter.FilterMaskIdLow = PGN_MASK&0xFFFF;
		hcan1filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
		hcan1filter.FilterNumber = 3;
		hcan1filter.FilterMode = CAN_FILTERMODE_IDMASK;
		hcan1filter.FilterScale = CAN_FILTERSCALE_32BIT;
		hcan1filter.FilterActivation = ENABLE;
		hcan1filter.BankNumber = 17;
		HAL_CAN_ConfigFilter(&hcan1, &hcan1filter);
		
		hcan1filter.FilterIdHigh =((FMS_PGN_EEC1<<11)>>16)&0xFFFF;
		hcan1filter.FilterIdLow = (FMS_PGN_EEC1<<11)&0xFFFF;
		hcan1filter.FilterMaskIdHigh =(PGN_MASK>>16)&0xFFFF;
		hcan1filter.FilterMaskIdLow = PGN_MASK&0xFFFF;
		hcan1filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
		hcan1filter.FilterNumber = 4;
		hcan1filter.FilterMode = CAN_FILTERMODE_IDMASK;
		hcan1filter.FilterScale = CAN_FILTERSCALE_32BIT;
		hcan1filter.FilterActivation = ENABLE;
		hcan1filter.BankNumber = 18;
		HAL_CAN_ConfigFilter(&hcan1, &hcan1filter);
		
		hcan1filter.FilterIdHigh =((FMS_PGN_VW<<11)>>16)&0xFFFF;
		hcan1filter.FilterIdLow = (FMS_PGN_VW<<11)&0xFFFF;
		hcan1filter.FilterMaskIdHigh =(PGN_MASK>>16)&0xFFFF;
		hcan1filter.FilterMaskIdLow = PGN_MASK&0xFFFF;
		hcan1filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
		hcan1filter.FilterNumber = 5;
		hcan1filter.FilterMode = CAN_FILTERMODE_IDMASK;
		hcan1filter.FilterScale = CAN_FILTERSCALE_32BIT;
		hcan1filter.FilterActivation = ENABLE;
		hcan1filter.BankNumber = 19;
		HAL_CAN_ConfigFilter(&hcan1, &hcan1filter);
		
		hcan1filter.FilterIdHigh =((FMS_PGN_HOURS<<11)>>16)&0xFFFF;
		hcan1filter.FilterIdLow = (FMS_PGN_HOURS<<11)&0xFFFF;
		hcan1filter.FilterMaskIdHigh =(PGN_MASK>>16)&0xFFFF;
		hcan1filter.FilterMaskIdLow = PGN_MASK&0xFFFF;
		hcan1filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
		hcan1filter.FilterNumber = 6;
		hcan1filter.FilterMode = CAN_FILTERMODE_IDMASK;
		hcan1filter.FilterScale = CAN_FILTERSCALE_32BIT;
		hcan1filter.FilterActivation = ENABLE;
		hcan1filter.BankNumber = 20;
		HAL_CAN_ConfigFilter(&hcan1, &hcan1filter);
		
		hcan1filter.FilterIdHigh =((FMS_PGN_SERV<<11)>>16)&0xFFFF;
		hcan1filter.FilterIdLow = (FMS_PGN_SERV<<11)&0xFFFF;
		hcan1filter.FilterMaskIdHigh =(PGN_MASK>>16)&0xFFFF;
		hcan1filter.FilterMaskIdLow = PGN_MASK&0xFFFF;
		hcan1filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
		hcan1filter.FilterNumber = 8;
		hcan1filter.FilterMode = CAN_FILTERMODE_IDMASK;
		hcan1filter.FilterScale = CAN_FILTERSCALE_32BIT;
		hcan1filter.FilterActivation = ENABLE;
		hcan1filter.BankNumber = 22;
		HAL_CAN_ConfigFilter(&hcan1, &hcan1filter);
		
		hcan1filter.FilterIdHigh =((FMS_PGN_LFE1<<11)>>16)&0xFFFF;
		hcan1filter.FilterIdLow = (FMS_PGN_LFE1<<11)&0xFFFF;
		hcan1filter.FilterMaskIdHigh =(PGN_MASK>>16)&0xFFFF;
		hcan1filter.FilterMaskIdLow = PGN_MASK&0xFFFF;
		hcan1filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
		hcan1filter.FilterNumber = 7;
		hcan1filter.FilterMode = CAN_FILTERMODE_IDMASK;
		hcan1filter.FilterScale = CAN_FILTERSCALE_32BIT;
		hcan1filter.FilterActivation = ENABLE;
		hcan1filter.BankNumber = 21;
		HAL_CAN_ConfigFilter(&hcan1, &hcan1filter);
	
	
	
		hcan1.pRxMsg = &can1RxMessage;

		xCAN1_MessageQueue = xQueueCreate( CAN1_MESSAGE_QUEUE_MAX_LENGTH, sizeof( CanRxMsgTypeDef ) );
		
		//xCAN1_DataMutex = xSemaphoreCreateMutex();
	
		xTaskCreate(CAN1_Listening_Task,(signed char*)"CAN1 Listening",CAN1_LISTENING_STACK_SIZE,NULL, tskIDLE_PRIORITY + 1, NULL);
		xTaskCreate(CAN2_Sending_Task,(signed char*)"CAN2 Sending",128,NULL, tskIDLE_PRIORITY + 5, NULL);		
}

static void CAN1_Listening_Task(void *pvParameters)
{
	CanRxMsgTypeDef RxMessage;	
	static uint8_t temp;
	while(1)
	{  
		while(xQueueReceive(xCAN1_MessageQueue, &RxMessage, portMAX_DELAY)==pdTRUE)
		{
			CAN1_Handling_Message(&RxMessage);
		}
		//taskYIELD();
		vTaskDelay(CAN_LISTENING_DELAY);
	}
}

static void CAN2_Sending_Task(void *pvParameters)
{
		TickType_t xLastWakeTime;
		xLastWakeTime = xTaskGetTickCount();
	
			CanTxMsgTypeDef TxMess;
			hcan2.pTxMsg = &TxMess;
			TxMess.StdId = 0x321;
			TxMess.ExtId = /*(FMS_PGN_DD<<8)*/0x100;
			TxMess.RTR = CAN_RTR_DATA;
			TxMess.IDE = CAN_ID_EXT;
			TxMess.DLC = 1;
			TxMess.Data[0]=0x3A;
			TxMess.Data[1]=0xFF;
			TxMess.Data[2]=0xAA;
			TxMess.Data[3]=0xFF;
			TxMess.Data[4]=0xAA;
			TxMess.Data[5]=0xFF;
			TxMess.Data[6]=0xAA;
			TxMess.Data[7]=0xFF;
	
		while(1)
		{
				TxMess.ExtId = 0x18FA0010;
				HAL_CAN_Transmit(&hcan2,10);
				vTaskDelayUntil( &xLastWakeTime, 400 );
				TxMess.ExtId = 0x18FB0010;
				HAL_CAN_Transmit(&hcan2,10);
				vTaskDelayUntil( &xLastWakeTime, 400 );
		}
}

void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan)
{
	static uint32_t i=0;
	i++;
}

/**
  * @brief  Transmission  complete callback in non blocking mode 
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
	static uint8_t temp=0;
	temp=hcan->pRxMsg->Data[0];
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;

	xQueueSendFromISR(xCAN1_MessageQueue, hcan->pRxMsg, &xHigherPriorityTaskWoken);
	
	if(xHigherPriorityTaskWoken == pdTRUE)
	{
		taskYIELD();
	}
	
	HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);
}

/**
  * @brief  Error CAN callback.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	static uint32_t i=0;
	i++;
}

void CAN1_Handling_Message(CanRxMsgTypeDef *can1msg)
{
		static uint32_t i=0;
		uint32_t pgn=(can1msg->ExtId>>8)&0xFFFF;
	
	uint16_t test=0;
	
	//xSemaphoreTake( xMBInputRegParamsMutex, portMAX_DELAY );
  {	
			switch(pgn)
			{
					case FMS_PGN_CCVS1:
					{
							MBHoldingRegParams.params.CANFMSParams.vehicleSpeed=*((uint16_t*)&can1msg->Data[1]);
					}
					break;
					
					case FMS_PGN_EEC2:
					{
							MBHoldingRegParams.params.CANFMSParams.calcLoad=can1msg->Data[2];
					}
					break;
					
					case FMS_PGN_LFC:
					{
							MBHoldingRegParams.params.CANFMSParams.totalFuelCons=*((uint32_t*)&can1msg->Data[4]);
					}
					break;
					
					case FMS_PGN_DD:
					{
							MBHoldingRegParams.params.CANFMSParams.fuelLevelRel=can1msg->Data[1];
					}
					break;
					
					case FMS_PGN_EEC1:
					{
							MBHoldingRegParams.params.CANFMSParams.engSpeed=*((uint16_t*)&can1msg->Data[3]);
					}
					break;
					
					case FMS_PGN_VW:
					{
							if((can1msg->Data[0]>0)&&(can1msg->Data[0]<=AXLE_NUM))
							{
									MBHoldingRegParams.params.CANFMSParams.axleWeight[can1msg->Data[0]]=*((uint16_t*)&can1msg->Data[1]);
							}
					}
					break;
					
					case FMS_PGN_HOURS:
					{
							MBHoldingRegParams.params.CANFMSParams.calcTotalEngTime=*((uint32_t*)&can1msg->Data[0]);
					}
					break;
					
					case FMS_PGN_SERV:
					{
							MBHoldingRegParams.params.CANFMSParams.serviceDist=*((uint16_t*)&can1msg->Data[1]);
					}
					break;
					
					case FMS_PGN_LFE1:
					{
							MBHoldingRegParams.params.CANFMSParams.fuelRate=*((uint16_t*)&can1msg->Data[0]);
					}
					break;
					
					default:
					{
						
					}
			}
	}
  //xSemaphoreGive( xMBInputRegParamsMutex );
}

