/*
 * can_driver.h
 *
 *  Created on: Jan 1, 2025
 *      Author: Kunaal
 */

#include "main.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "stdint.h"
#include "can.h"

#define CAN_QUEUE_SIZE 32

typedef struct {
    uint32_t id;
    uint8_t dlc;
    uint8_t data[8];
    bool extd;
    bool rtr;
} CAN_Frame_t;

typedef struct {
    volatile uint16_t head;
    volatile uint16_t tail;
    CAN_Frame_t buffer[CAN_QUEUE_SIZE];
} CAN_Queue_t;

void can_init(void);
void can_loop(void);
void decode_can_rx(CAN_HandleTypeDef *hcan, uint32_t id, uint32_t dlc, uint8_t *data, bool extd, bool rtr);
void can_send_data(CAN_HandleTypeDef *hcan, uint32_t id, uint32_t dlc, uint8_t *data, bool extd, bool rtr);
