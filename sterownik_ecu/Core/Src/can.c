/*
 * can.c
 *
 *  Created on: Jul 31, 2026
 *      Author: admin
 */

#include <can.h>
#include <uart.h>
#include <stdio.h>

void can_update(){
	CAN_TxHeaderTypeDef TxHeader;
	uint32_t TxMailbox;
	uint8_t TxData[8];

	TxHeader.StdId = 0x100;
	TxHeader.ExtId = 0;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = 2;
	TxHeader.TransmitGlobalTime = DISABLE;

	uint16_t rpm = 3500;

	TxData[0] = rpm & 0xFF;
	TxData[1] = rpm >> 8;

	HAL_StatusTypeDef status;

	status = HAL_CAN_AddTxMessage(&hcan2, &TxHeader, TxData, &TxMailbox);

	int len = snprintf(uart_buffer,
	                   sizeof(uart_buffer),
	                   "Tx=%d St=%d Er=%08lX E=%08lX T=%08lX F=%lu\r\n",
	                   status,
	                   HAL_CAN_GetState(&hcan2),
	                   HAL_CAN_GetError(&hcan2),
	                   CAN2->ESR,
	                   CAN2->TSR,
	                   HAL_CAN_GetTxMailboxesFreeLevel(&hcan2));

		    HAL_UART_Transmit(&huart1,
		                      (uint8_t*)uart_buffer,
		                      len,
		                      HAL_MAX_DELAY);

}
