/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include"string.h"
#include "stdio.h"
#include <stdlib.h>
#include "string.h"
#include "variables.h"
#include "usbd_midi.h"
extern USBD_HandleTypeDef hUsbDeviceFS;


// change HID to MIDI in usb_device.c  and usbd_conf.c     !!!! also modifiy files in USB_Device
// device configurator can overwrite usb_device  files
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "midi.h"
#include "notes.h"
#include "lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */





#include "flash.h"




int _write(int le, char *ptr, int len)
{
int DataIdx;
for(DataIdx = 0; DataIdx < len; DataIdx++)
{
ITM_SendChar(*ptr++);
}
return len;
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

void note_replace(uint8_t note_replace);
//void USB_CDC_RxHandler(uint8_t*, uint32_t);
void patch_screen(void);
void buttons_store(void);
void flash_write(void);
void flash_read(void);
void panic_delete(void);
//void stop_start(void);
void note_off(void);
void arrows(void);
void cdc_send(void);
void all_notes_off(void);
void play_muting(void);
void led_full_clear(void); // runs once clearing all leds

void lcd_start(void);
void lcd_print(uint8_t  pos , char print);  // position 0-39 , character
void lcd_menu_vars(void);
void nrpn_send(void);
void loop_screen(void);// loop screen change
void note_buttons(void); // all note functions from buttons
void loop_lfo(void);
void settings_storage(void);
void pattern_settings(void);
void USBD_MIDI_DataInHandler(uint8_t *usb_rx_buffer, uint8_t usb_rx_buffer_length);
void midi_send_control(void); // runs midi send when needed
void midi_extras(void);
void flash_page_write(uint8_t page_select,uint8_t* data);
uint8_t pattern_scale_process(uint8_t value, uint8_t selected_sound ); // midi in to scaled note

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USB_DEVICE_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  // USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS);

  HAL_SPI_Init(&hspi1); // write to register hspi2
  HAL_DMA_Init(&hdma_spi1_rx);
  	HAL_DMA_Init(&hdma_spi1_tx);
  //	HAL_I2C_Init(&hi2c1);
  //	HAL_TIM_Base_Start(&htim2);
  //	HAL_TIM_Base_Start(&htim2);
  //	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
  //	TIM2->CNT=32000;

  	int __io_putchar(int ch)
  	{
  	    ITM_SendChar(ch);
  	    return (ch);
  	}


    HAL_TIM_Base_Start_IT(&htim2);
   // HAL_UART_Receive_IT(&huart1, &serial1_temp, 1);		// midi irq
    HAL_UART_Receive_DMA(&huart1, serial1_hold2,1);    //
  //CDC_Transmit_FS("Hello\r\n",7);

	printf("hello");

	//  get flash data
	flash_read();
	//other_buttons=1; // nothing
	//uint16_t arr_cnt=(bpm_table[tempo]*1.333333)-1;
	//TIM2->ARR= arr_cnt;   // -1
  	//TIM2->CCR1=(arr_cnt/2) ;   // 90-4v   50-1.5v 20- 0.6v   -1

	//TIM2->CCMR1 = 0;
	lcd_start();

	loop_screen();
	// panic_delete();                 WATCH FOR WEIRD APC SENDS , IE UP ARROW PLUS BOTTOM ROW 3 SENDS CONSTANT CONTROLLER 50 INFO  ?
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


	 // HAL_GPIO_WritePin(PPQ_GPIO_Port, PPQ_Pin, (ppq_send|1));

	 // uint16_t step_temp=0;
	 // uint8_t seq_step_mod;
	  uint8_t selected_scene=scene_buttons[0];


	  if (seq_pos_mem!=seq_pos){     // runs  8 times /step  , control sequencer counting

		  green_position[0]=seq_step; green_position[1]=seq_step;
		  cdc_send(); // all midi compiled for send  8/per note , sends on seq_pos=1 atm

		  if (serial_len)   HAL_UART_Transmit(&huart1,serial_out,serial_len,100); // uart send disable if no info, sent seq_pos,


		  //maybe dma if needed for now ok (2ms total for send )

		  // USB_send();

		  if ((seq_pos&7)==5){
				uint8_t sound_select=4;  // select sound to gate

				nrpn_gating_switch[sound_select]=nrpn_gating_sequence[seq_step];
				nrpn_gating_enable=sound_select+1;


				//if(nrpn_gating_switch[sound_select]) nrpn_gating_switch[sound_select]=0; else nrpn_gating_switch[sound_select]=127;    // might enable some presets here

					 // cdc_send();
				  }







			 seq_pos_mem=seq_pos;
		 } // end of seq_pos ,fast step 8/step



	  if ((s_temp) != (seq_pos>>3)) {			// runs on note steps 0-15



		  midi_extras();
			  pattern_settings();
			  led_full_clear();







			  uint8_t current_scene=scene_buttons[0]; //  local
			  seq_current_step=loop_lfo_out[current_scene+32];
			  loop_current_speed=pattern_scale_list[current_scene];

			  if (!seq_pos) LFO_tracking(); // runs once per bar
			  if ((s_temp==14) && (overdub_enabled==2))  overdub_enabled=1; // resets bar wipe on record

			  note_enable_list_selected=0;
			  note_enable_list_selected=LFO_high_list[current_scene]+1; // for lcd , note period
			  note_enable_list_selected=((LFO_low_list[current_scene]+1)*10)+note_enable_list_selected;
			 // note_enable_list_selected=((LFO_delay_list[current_scene]+1)*100)+note_enable_list_selected;

			  if ((!seq_pos)&& (!pause)) seq_step_long=(seq_step_long+1)&31;    // this needs to be main clock

			  play_position=seq_step_long;
			  if ((seq_step_long&31)==0)   play_position=(play_position+1)&31;  // normal screen leave alone ,too fast
			  if ((seq_step_long&3)==0) {play_position=(play_position>>2)<<2;} // reset

			  if ((s_temp&15)==14) { loop_lfo();lfo_full_send_enable=1;} // run lfo at end of a bar , 2 notes long to send 16 settings

			  ////


			  if ((send) && shift) {  all_notes_off(); flash_read();  button_states[send_button]=0; send=0; }   // reload everything from flash


			  uint8_t crap[64];
			  memcpy (crap,cdc_buffer,12);

		//////////////PRINT///////////////////
			  for (i=0;i<16;i++){  // i=scene

				//  printf(" %d",crap[i] );

				  printf(" %d",pitch_list_for_drums[i]);
				//  printf(" %d",pitch_list_for_drums[pitch_selected_for_drums[i]+(i*8)]);

				 // printf(" %d",loop_screen_note_on[(selected_scene*32)+i] );
				 // printf(" ds=%d ",drum_store_one[i+(scene_buttons[0]*4)] );


			  }


			  printf("   %d",seq_step );
			  printf("   %d",led_clear);
			  printf("   %d",pattern_select );

			  printf("   %d",midi_cue_time[midi_cue_count] );
			  printf("   %d",midi_cue_count );
			  printf(" \n" );
		//	  printf(" incoming=%d ",square_buttons_list[test_byte[11]]);  printf(" step=%d ",seq_step_list[0]);
			//  printf(" cdc=%d\n ",cdc_len_temp);

 				// print section



			  if (lcd_downcount) { lcd_message();lcd_downcount--; }

 				 lcd_menu_vars();


 				//		printf(" %d ", midi_cue[3]);
 				//		printf(" %d\n ", midi_cue[6]);

 				  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,led_blink);
 				  led_blink=!led_blink;

 				  flash_write(); // works only if button pressed , not during pause ?
 				  note_off_enable=1;
 				//  arrows();
 				  if (!pause) seq_step = seq_pos>> 3; else seq_step=seq_step;

 				//  if( (USBD_MIDI_GetState(&hUsbDeviceFS) == MIDI_IDLE) )  USB_send();  // fast     	//temp disable




 				  s_temp = seq_pos>>3;

 			}   // blink steps , end of s_temp

	  lcd_mem(); // this sends too fast when run constant
	  USB_send();


/*


 	  if ((seq_pos == 64) &&(mtc_tick)) {    // only if mtc is incoming

 		  if (mtc_tick > 64) {
 				timer_value = timer_value - (mtc_tick - 64);
 				mtc_tick = seq_pos;
 			} // chase mtc ,reasonable
 			if (mtc_tick < 64) {
 				timer_value = timer_value + (64 - mtc_tick);
 				mtc_tick = seq_pos;
 			}
 			if ((timer_value > 700) | (timer_value < 300))
 				timer_value = 512; // fix in case

 		} else seq_enable=1;  // if no mtc then just run sequencer


 	  if (serial1_hold[5])	{//printf("%d",timer_value);printf(" %d ",seq_pos);printf(" %d \n",mtc_tick);


 		  if (serial1_hold[0]>247) realtime=serial1_hold[0];
 		  else {realtime=0;message_cue[message_counter]=serial1_hold[0];message_counter++; }
 		  if(message_counter>2) message_counter=0;   // set to typical midi for now


 		  if (realtime==248 ) {realtime=0;
 		  if(mtc_tick==191) mtc_tick=0; else mtc_tick++;realtime=0;}   // 8/4 or 32/16
 		//  if (realtime==248 ) {mtc_tick++;realtime=0;printf(" %d",seq_pos);printf(" %d \n",mtc_tick);} //always running

 		  if (realtime==250 )
 					  {seq_enable=1;realtime=0;seq_pos=0;seq_step=0;mtc_tick=0;} // reset to 0 on real time message :start
 		  if (realtime==252)
 							  {seq_enable=0;
 							  send_buffer[5]=0;  // clear last step
 							  realtime=0; } // reset to 0 on real time message :stop



			//  note_handling(serial1_hold[0]);note_replace(note_replace_enable);    // notes generate after each !!!
			 // 	note_handling(serial1_hold[1]);note_replace(note_replace_enable);
			 // 	note_handling(serial1_hold[2]);note_replace(note_replace_enable);

		  serial1_hold[5]=0;serial1_hold[0]=0;
		  //HAL_UART_Receive_IT(&huart1,&serial1_temp,1); // finish 4 bytes and blink , reset irq



		  }  // end of midi in
	*/


			 // if( (USBD_MIDI_GetState(&hUsbDeviceFS) == MIDI_IDLE) )  USB_send();  // fast     	//temp disable


			  if (cdc_buffer[0] | cdc_buffer[3] |   cdc_buffer[6]                   ) {      //  when cdc buffer incoming, need change
				  if (cdc_buffer[6] )cdc_start=6;
				  if (cdc_buffer[3] )cdc_start=3;
				  if (cdc_buffer[0] )cdc_start=0;
				//  if (first_message) {first_message=0;  memset(other_buttons_hold,9,70);} // only runs after getting a midi message from usb


				  if ((cdc_buffer[0]==145)) {keyboard[0]=cdc_buffer[1]-47;keyboard[1]=127;memset(cdc_buffer,0,9); } // works ,keyboard send ,quick
				  if ((cdc_buffer[0]==129)) {keyboard[0]=cdc_buffer[1]-47;keyboard[1]=0;memset(cdc_buffer,0,9); }

				  // sends straight to keyboard on receive
			 // printf(" 1=%d ",cdc_buffer[0] ); printf(" 2=%d ",cdc_buffer[1] ); printf(" 3=%d \n ",cdc_buffer[2] );



			  if (pause==5)  { all_notes_off();}    // runs only once during pause


		  }  // end of cdc message

				  if (keyboard[0])  {    // keyboard play live

						  uint8_t note_flag=144; // just use vel 0 for off
						  uint8_t incoming=keyboard[0];
						  uint8_t velocity=keyboard[1];
						  uint8_t buffer_pos=keyboard_buffer[31];
						  uint8_t current_seq_pos=seq_pos&255;
						//  if (incoming>>7)  {note_flag=128;velocity=0;}
						  if (buffer_pos>23) buffer_pos=0;   // reset buffer in case


						  if (midi_channel_list[selected_scene]==9)   // drums send
						  {  midi_extra_cue[0]=(note_flag+9);         // use drumlist for now

						  midi_extra_cue[1]=(drum_list[selected_scene]);midi_extra_cue[2]=velocity; midi_extra_cue[28]=midi_extra_cue[28]+3;   // send midi

						  incoming=0;}
						  else  {midi_extra_cue[0]=note_flag+midi_channel_list[selected_scene];  midi_extra_cue[1]=((incoming+32))&127 ;
						  midi_extra_cue[2]=velocity; midi_extra_cue[28]=midi_extra_cue[28]+3;incoming=0;}  // send normal then keyboard off
						  keyboard[0]=incoming;

						  memcpy(keyboard_buffer+buffer_pos,midi_extra_cue+1,2); keyboard_buffer[buffer_pos+2]=current_seq_pos; keyboard_buffer[31]=buffer_pos+3;  // store in buffer



				  }//end of keyboard

					  if(((seq_pos&7)>3) && (cdc_buffer[0]))   buttons_store();   // only runs after receive , might delay a bit


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  } // while loop
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 191;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 5000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 31250;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, PPQ_Pin|CS1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PPQ_Pin */
  GPIO_InitStruct.Pin = PPQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PPQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CS1_Pin */
  GPIO_InitStruct.Pin = CS1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS1_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
	void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	{
		if (huart->Instance == USART1) { 	// midi irq

	//		if (serial1_hold2[0]==248)  {	memcpy(serial1_hold,serial1_hold2,4)  ;   // copy mem

		//	serial1_hold[5]=1;seq_pos++;   HAL_UART_Receive_DMA(&huart1, serial1_hold2,1); }


	//		else

			memcpy(serial1_hold,serial1_hold2,1)  ;   // copy mem

			serial1_hold[5]=1;   HAL_UART_Receive_DMA(&huart1, serial1_hold2,1);
			//HAL_UART_Receive_IT(&huart1,&serial1_temp,1);}  //enable read  or reset irq

		}




		//memcpy((serial_hold+8),(serial_hold2+8),8);
	}





	void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)    // unreliable
		{
			if(TIM2==htim->Instance){     // send spi to display , ok

				//button_send_trigger=(button_send_trigger+1)&255; // sets interval for buttons

				if (!skip_enable)HAL_GPIO_TogglePin(GPIOA,PPQ_Pin);

				if (!skip_enable) skip_enable=skip_setting; else {skip_enable--;} // 48 ppq here

				if (ppq_count>5) {ppq_count=0; seq_pos=(seq_pos+1)&127;}   // 32 ppq
				 ppq_count++;
				 if (ppq_send>95) ppq_send=0;  else ppq_send++;   // 96 ppq


			}
		}
	void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef * hspi)   // when finished sending
		{

		   if (SPI1==hspi->Instance) {				// keep it short if possible

			   HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);  //  end
			   flash_flag=2;


			   //    if (sample_dma_counter==0)		{  memcpy(flash_read_block3,flash_read_block2+4,1024);  flash_flag=2;  }
			//   if (sample_dma_counter==1) sample_dma_counter=3;
					   //			      if (sample_dma_counter==2) sample_dma_counter=4;
		  //     if (sample_dma_counter==3)   {memcpy(flash_read_block,flash_read_block2+4,1024);sampler_1k_load(sample_flash_address[0]);sample_dma_counter=2;return;}
		  //     if (sample_dma_counter==4)			{  memcpy(flash_read_block3,flash_read_block2+4,1024); sampler_1k_load(sample_flash_address[1]); sample_dma_counter=1; }
		//	if (sample_dma_counter)   {memcpy(flash_read_block,flash_read_block2+4,1024); sampler_1k_load(sample_flash_address[1]); sample_dma_counter=0;flash_flag=2; }

		   }
		}

//	HAL_I2C_SlaveRxCpltCallback(){}










	void stop_start(void)             {
	  //  if (TIM10==htim ->Instance)
		if  (stop_toggle ==1) {HAL_TIM_Base_Stop_IT(&htim2);stop_toggle =2;}
		if  (stop_toggle ==4) {HAL_TIM_Base_Start_IT(&htim2);stop_toggle=0;}

	}
	void all_notes_off(void){
		uint8_t data[48];
		for (i=0;i<16;i++){
			data[i*3]=176+i;
			data[(i*3)+1]=123;
			data[(i*3)+2]=0;
		}
		 HAL_UART_Transmit(&huart1,data,48,100); //   send all notes off on serial
		 HAL_Delay(100);
		 pause=1;
		 //CDC_Transmit_FS(data+3, 45);

	}
	/*void HAL_I2C_MasterTxCpltCallback (I2C_HandleTypeDef * hi2c)
	{
	  I2C_transmit=1;
	}*/

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	  /* User can add his own implementation to report the HAL error return state */
	  __disable_irq();
	  while (1)
	  {
	  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	  /* User can add his own implementation to report the file name and line number,
		 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
