# Dual-CAN-Driver-STM32
This driver can be integrated in any STM32 Microcontroller with two CAN ports: CAN1 and CAN2. It has a `queue-based Receiving and Transmitting system`, which allows it to send and receive multiple messages at very high frequencies with minimal messages dropped due to STM32 HAL CAN mailbox full.

## Usage

Note: You can find usage example in `main_example.c` and the IOC-generated CAN Configuration code in `can_configuration.c`.

### 1. Initializing the driver
To use this driver in your code, copy the `can_driver.c` and `can_driver.h` files into your STM32 project source folder.
To initialize the driver in the main code, after setting up CAN at the desired baud rate and **enabling _all_ the `CAN Interrupts` available, add these in the `main.c` file:**
```
/* main.c */
//.... Skip to the below section...//
/* Private includes */
/* USER CODE BEGIN Includes */
#include "can_driver.h" // ----------> Add this line
/* USER CODE END Includes */
//.... Skip to the below section...//
int main(void) {
//.... Skip to the below section...//
  /* USER CODE BEGIN 2 */
     can_init(); // ----------> Add this line
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    can_loop(); // ----------> Add this line
    /* USER CODE END WHILE */

}
```

### 2. Sending CAN Messages
For transmitting new messages, **`call`** the function `can_send_data` in your application layer like this:

```
/*******************************************************************************************************************************
**   Function info: Transmit data over CAN
**   Parameters-
**     1. CAN_HandleTypeDef *hcan - Tells the function which CAN bus you want to send the message on
**     2. uint32_t id: CAN Identifier, 11-bit for Standard or 29-bit for extended ID
**     3. uint32_t dlc: Size of data. Since this is classic/native CAN, this parameter can be up to 8 (max bytes per message).
**     4. uint8_t* data: Data of up to 8 bytes as specified in the DLC
**     5. bool extd: If ID is extended, make this true, otherwise make it false
**     6. bool rtr: If sending a remote request, make this true, to send data, make it false
**  Return: none
*******************************************************************************************************************************/
can_send_data(CAN_HandleTypeDef *hcan, uint32_t id, uint32_t dlc, uint8_t *data, bool extd, bool rtr);
```

### 3. Receiving CAN messages
For decoding received messages, **`define`** the function `decode_can_rx` in your application layer:

```
// Function parameters are same as in 'can_send_data' function
void decode_can_rx(CAN_HandleTypeDef *hcan, uint32_t id, uint32_t dlc, uint8_t *data, bool extd, bool rtr) {
 	if(hcan->Instance == CAN1) {
		// Decode data coming on CAN1
	}

	if(hcan->Instance == CAN2) {
		// Decode data coming on CAN2
	}
}
```

_Note: You can also call `can_send_data` inside `decode_can_rx` to respond to incoming messages_

## Thank you
This library is publicly available for anyone to use. You can use it for TWO CAN ports, and modify it to use it for ONE CAN port as well.
