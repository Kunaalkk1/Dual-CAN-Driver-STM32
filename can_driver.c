/*
 * can_driver.c
 *
 *  Created on: Jan 1, 2025
 *      Author: Kunaal
 */

#include "can_driver.h"

CAN_RxHeaderTypeDef RxHeader;
HAL_StatusTypeDef can_status;

static CAN_Queue_t can_queue_fifo0 = {0};
static CAN_Queue_t can_queue_fifo1 = {0};
static CAN_Queue_t can_tx_queue1 = {0};
static CAN_Queue_t can_tx_queue2 = {0};

void can_init(void) {
    CAN_FilterTypeDef sFilterConfig1 = {
        .FilterBank = 0,
        .FilterMode = CAN_FILTERMODE_IDMASK,
        .FilterScale = CAN_FILTERSCALE_32BIT,
        .FilterIdHigh = 0x0000,
        .FilterIdLow = 0x0000,
        .FilterMaskIdHigh = 0x0000,
        .FilterMaskIdLow = 0x0000,
        .FilterFIFOAssignment = CAN_RX_FIFO0,
        .FilterActivation = ENABLE,
		.SlaveStartFilterBank = 14
    };

    HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig1);
    sFilterConfig1.FilterBank = 14;
    sFilterConfig1.FilterFIFOAssignment = CAN_RX_FIFO1;

    HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig1);
    HAL_CAN_Start(&hcan1);
    HAL_CAN_Start(&hcan2);

	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK) {
		Error_Handler();
	}
}

static bool queue_is_full(const CAN_Queue_t *q) {
    return ((q->head + 1) % CAN_QUEUE_SIZE) == q->tail;
}

static bool queue_is_empty(const CAN_Queue_t *q) {
    return q->head == q->tail;
}

static void queue_push(CAN_Queue_t *q, const CAN_Frame_t *f) {
    if (!queue_is_full(q)) {
        q->buffer[q->head] = *f;
        q->head = (q->head + 1) % CAN_QUEUE_SIZE;
    }
}

static bool queue_pop(CAN_Queue_t *q, CAN_Frame_t *f) {
    if (queue_is_empty(q)) return false;
    *f = q->buffer[q->tail];
    q->tail = (q->tail + 1) % CAN_QUEUE_SIZE;
    return true;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef RxHeader;
    CAN_Frame_t frame;
    uint8_t rxData[8];

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, rxData) == HAL_OK) {
        frame.dlc = RxHeader.DLC;
        memcpy(frame.data, rxData, RxHeader.DLC);

        if (RxHeader.IDE == CAN_ID_EXT) {
            frame.id = RxHeader.ExtId;
            frame.extd = true;
        } else {
            frame.id = RxHeader.StdId;
            frame.extd = false;
        }

        frame.rtr = (RxHeader.RTR == CAN_RTR_DATA);

        queue_push(&can_queue_fifo0, &frame);
    }
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef RxHeader;
    CAN_Frame_t frame;
    uint8_t rxData[8];

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &RxHeader, rxData) == HAL_OK) {
        frame.dlc = RxHeader.DLC;
        memcpy(frame.data, rxData, RxHeader.DLC);

        if (RxHeader.IDE == CAN_ID_EXT) {
            frame.id = RxHeader.ExtId;
            frame.extd = true;
        } else {
            frame.id = RxHeader.StdId;
            frame.extd = false;
        }

        frame.rtr = (RxHeader.RTR == CAN_RTR_DATA);

        queue_push(&can_queue_fifo1, &frame);
    }
}

void CAN_ProcessRxFifo0(void) {
    CAN_Frame_t frame;
    uint32_t start = HAL_GetTick();

    while (queue_pop(&can_queue_fifo0, &frame)) {
        if ((HAL_GetTick() - start) > 2) {
            break;
        }
    }
}

void CAN_ProcessRxFifo1(void) {
    CAN_Frame_t frame;
    uint32_t start = HAL_GetTick();

    while (queue_pop(&can_queue_fifo1, &frame)) {
        if ((HAL_GetTick() - start) > 2) {
            break;
        }
    }
}

void CAN_ProcessTxQueue0(void) {
    CAN_Frame_t frame;
    uint32_t txMailbox;

    while (!queue_is_empty(&can_tx_queue1)) {
        if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
            break;
        }

        if (queue_pop(&can_tx_queue1, &frame)) {
            CAN_TxHeaderTypeDef TxHeader;
            TxHeader.DLC = frame.dlc;
            TxHeader.RTR = frame.rtr ? CAN_RTR_REMOTE : CAN_RTR_DATA;

            if (frame.extd) {
                TxHeader.IDE = CAN_ID_EXT;
                TxHeader.ExtId = frame.id;
            } else {
                TxHeader.IDE = CAN_ID_STD;
                TxHeader.StdId = frame.id;
            }

            HAL_CAN_AddTxMessage(&hcan1, &TxHeader, frame.data, &txMailbox);
        }
    }
}

void CAN_ProcessTxQueue1(void) {
    CAN_Frame_t frame;
    uint32_t txMailbox;

    while (!queue_is_empty(&can_tx_queue2)) {
        if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan2) == 0) {
            break;
        }

        if (queue_pop(&can_tx_queue2, &frame)) {
            CAN_TxHeaderTypeDef TxHeader;
            TxHeader.DLC = frame.dlc;
            TxHeader.RTR = frame.rtr ? CAN_RTR_REMOTE : CAN_RTR_DATA;

            if (frame.extd) {
                TxHeader.IDE = CAN_ID_EXT;
                TxHeader.ExtId = frame.id;
            } else {
                TxHeader.IDE = CAN_ID_STD;
                TxHeader.StdId = frame.id;
            }

            HAL_CAN_AddTxMessage(&hcan2, &TxHeader, frame.data, &txMailbox);
        }
    }
}

// Define this in the application layer
__weak void decode_can_rx(CAN_HandleTypeDef *hcan, uint32_t id, uint32_t dlc, uint8_t *data, bool extd, bool rtr) {
	if(hcan->Instance == CAN1) {
		// Decode data coming on CAN1
	}

	if(hcan->Instance == CAN2) {
		// Decode data coming on CAN2
	}
	return;
}

void can_send_data(CAN_HandleTypeDef *hcan, uint32_t id, uint32_t dlc, uint8_t *data, bool extd, bool rtr) {
    CAN_Frame_t frame;
    frame.id = id;
    frame.dlc = dlc;
    frame.extd = extd;
    frame.rtr = rtr;
    memcpy(frame.data, data, dlc);

    if (hcan == &hcan1) {
        queue_push(&can_tx_queue1, &frame);
    } else if (hcan == &hcan2) {
        queue_push(&can_tx_queue2, &frame);
    }
}

void can_loop(void) {
    CAN_ProcessRxFifo0();
    CAN_ProcessRxFifo1();
    CAN_ProcessTxQueue0();
    CAN_ProcessTxQueue1(); 
}
