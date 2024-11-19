#include "hw_serial.h"
// #define USART_DEBUG USART1
#define BUFFER_MAX_LEN 15
user_package tx_buffer[BUFFER_MAX_LEN]; // 发送缓冲区
user_package *rx_data_use_ptr;			// 当前接收数据指针
user_package rx_buffer[BUFFER_MAX_LEN]; // 接收缓冲区
uint8_t rx_buffer_head = 0;				// 缓冲区头索引
uint8_t rx_buffer_tail = 0;				// 缓冲区尾索引
uint8_t rx_offset = 0;					// 地址偏移量
uint8_t rx_head_count = 0;				// 包头计数
/*
 *@brief:串口发送数据包
 *@param:无
 *@return:无
 */
void HW_Serial_Package_Send(void)
{
	// 包头为0xff(注意包头要避免和实际数据相同)
	User_Serial_SendByte(USART2, 0xff);
	User_Serial_SendByte(USART2, 0xff);
	// 数据长度
	uint8_t *tx_offset = (uint8_t *)&tx_buffer;
	for (uint8_t i = 0; i < sizeof(tx_buffer); i++)
	{
		// 对地址偏移1个字节，然后取该地址的值
		User_Serial_SendByte(USART2, *(tx_offset++));
	}
}
/*
 *@brief:串口获取接收数据标志位
 *@param:uint8_t
 *@return:1表示接收到数据，0表示没有数据
 */
uint8_t HW_Serial_GetRxFlag(void)
{
	// 如果接收缓冲区头不等于接收缓冲区尾，则表示接收到数据
	if (rx_buffer_head != rx_buffer_tail)
	{
		// 当前数据索引指向接收缓冲区头索引
		rx_data_use_ptr = &rx_buffer[rx_buffer_head];
		// 头索引加1
		rx_buffer_head = (rx_buffer_head + 1) % BUFFER_MAX_LEN;
		return 1;
	}
	else
	{
		return 0;
	}
}
/*
 *@brief:串口接收数据包
 *@param:无
 *@return:无
 *@note:函数需放在接收标志位的判断下
 */
void HW_Serial_Package_Receive(void)
{
	uint8_t rx_data = USART_ReceiveData(USART2);
	// 对缓冲区的尾取地址，然后对地址偏移1个字节，然后赋值
	*(((uint8_t *)&rx_buffer[rx_buffer_tail]) + rx_offset) = rx_data;
	// 地址偏移加1
	rx_offset++;
	if (rx_data == 0xff) // 注意包头要避免和实际数据相同
	{
		rx_head_count++;
		if (rx_head_count == 2)
		{
			rx_offset = 0;
		}
	}
	else
	{
		rx_head_count = 0;
	}
	// 如果地址偏移量等于数据包长度，则将接收缓冲区尾索引加1
	if (rx_offset == (rx_buffer[rx_buffer_tail].data1 == 2 ? (sizeof(user_package) - 4) : sizeof(user_package)))
	{
		rx_offset = 0;
	}
}
