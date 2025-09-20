# Dual-CAN-Driver-STM32
This driver can be integrated in any STM32 Microcontroller with two CAN ports: CAN1 and CAN2.

## Example Usage

1. For transmitting new messages, **call** this function in your application layer:

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

2. For decoding received messages, **define** this function in your application layer:

```
void decode_can_rx(CAN_HandleTypeDef *hcan, uint32_t id, uint32_t dlc, uint8_t *data, bool extd, bool rtr) {
 	if(hcan->Instance == CAN1) {
		// Decode data coming on CAN1
	}

	if(hcan->Instance == CAN2) {
		// Decode data coming on CAN2
	}
}
```
