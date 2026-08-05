/*
 * can.c
 *
 *  Created on: Jul 31, 2026
 *      Author: admin
 */

#include <can.h>
#include <engine_info.h>
#include <uart.h>
#include <stdio.h>
#include <trigger.h>

static void CAN_Send4xU16(uint16_t id,
                          uint16_t a,
                          uint16_t b,
                          uint16_t c,
                          uint16_t d)
{

    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;
    uint8_t TxData[8];

    TxHeader.StdId = id;
    TxHeader.ExtId = 0;
    TxHeader.IDE = CAN_ID_STD;
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.DLC = 8;
    TxHeader.TransmitGlobalTime = DISABLE;

    TxData[0] = a & 0xFF;
    TxData[1] = a >> 8;

    TxData[2] = b & 0xFF;
    TxData[3] = b >> 8;

    TxData[4] = c & 0xFF;
    TxData[5] = c >> 8;

    TxData[6] = d & 0xFF;
    TxData[7] = d >> 8;

    HAL_CAN_AddTxMessage(&hcan2, &TxHeader, TxData, &TxMailbox);
}

void can_update(void)
{
    /* Ramka 0x100 */
    CAN_Send4xU16(0x100,
                  rpm,
                  ignition_time,
                  map,
                  maf);

    /* Ramka 0x101 */
    CAN_Send4xU16(0x101,
                  lambda,
                  voltage,
                  accelerator,
                  egt);

    /* Ramka 0x102 */
    CAN_Send4xU16(0x102,
                  iat,
                  coolant_temp_engine,
                  coolant_temp_cooler,
                  rpm_regulator);

    /* Ramka 0x103 */
    CAN_Send4xU16(0x103,
                  target_lambda,
                  target_rpm,
                  target_rpm_regulator,
                  0);

#ifdef CAN_DEBUG

    int len = snprintf(uart_buffer,
                       sizeof(uart_buffer),
                       "CAN St=%d Err=%08lX ESR=%08lX TSR=%08lX Free=%lu\r\n",
                       HAL_CAN_GetState(&hcan2),
                       HAL_CAN_GetError(&hcan2),
                       CAN2->ESR,
                       CAN2->TSR,
                       HAL_CAN_GetTxMailboxesFreeLevel(&hcan2));

    HAL_UART_Transmit(&huart1,
                      (uint8_t *)uart_buffer,
                      len,
                      HAL_MAX_DELAY);

#endif
}
