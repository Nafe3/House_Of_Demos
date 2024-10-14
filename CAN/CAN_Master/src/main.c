/**
  ******************************************************************************
  * @file    BSP/Src/main.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the STM32746G Discovery BSP Drivers
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stlogo.h"
#include "lcd_log.h"

/** @addtogroup STM32F7xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define KEY_PRESSED     0x01
#define KEY_NOT_PRESSED 0x00
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t ubKeyNumber = 0x0;

/* Global extern variables ---------------------------------------------------*/
uint8_t NbLoop = 1;
#ifndef USE_FULL_ASSERT
uint32_t    ErrorCounter = 0;
#endif

CAN_HandleTypeDef     CanHandle;
CAN_TxHeaderTypeDef   TxHeader;
CAN_RxHeaderTypeDef   RxHeader;
uint8_t               TxData[8];
uint8_t               RxData[8];
uint32_t              TxMailbox;

/* Private function prototypes -----------------------------------------------*/
static void MPU_Config(void);
static void SystemClock_Config(void);
static void Display_DemoDescription(void);
static void CAN_Config(void);
static void CPU_CACHE_Enable(void);
static HAL_StatusTypeDef CAN_Polling(void);
static void Error_Handler(int);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  uint8_t  lcd_status = LCD_OK;

  /* Configure the MPU attributes */
  MPU_Config();

  /* Enable the CPU Cache */
  CPU_CACHE_Enable();

  /* STM32F7xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();

  //Disable PLL and switch to HSI (8 MHz), correct PLL setting will take place inside SystemClock_Config()
  if((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL)
  {
	  RCC->CFGR &= (uint32_t) (~RCC_CFGR_SW);
	  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
  }

  /* Configure the system clock to 200 Mhz */
  SystemClock_Config();

  BSP_LED_Init(LED1);

  /* Configure the User Button in GPIO Mode */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

  /*##-1- Initialize the LCD #################################################*/
  /* Initialize the LCD */
  lcd_status = BSP_LCD_Init();
  ASSERT(lcd_status != LCD_OK);

  /* Initialize the LCD Layers */
  BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FRAME_BUFFER);

  Display_DemoDescription();


  /* Initialize LCD Log module */
  LCD_LOG_Init();

  /* Show Header and Footer texts */
  LCD_LOG_SetHeader((uint8_t *)"CAN Demo");


  /*Loopback*/
  //LCD_UsrLog("Starting CAN Polling...\n");
  //if(CAN_Polling() == HAL_OK)
  //{
  //  /* OK: Turn on LED1 */
  //  BSP_LED_On(LED1);
  //}
  //else
  //{
  //  /* KO: Turn on LED3 */
  //  BSP_LED_Off(LED1);
  //}
  //LCD_UsrLog("CAN Polling done\n");


  /* Configure the CAN peripheral */
  LCD_UsrLog("Starting CAN config...\n");
  CAN_Config();
  LCD_UsrLog("CAN config done\n");

  /* Output User logs */
  //for (int i = 0; i < 50; i++)
  //{
  //  LCD_UsrLog ("Printing from main %d \n", i);
  //}



//HAL_Delay(5000);
  /* Wait For User inputs */
  //Log_demo();

  /* Infinite loop */
  while (1)
  {
	  HAL_Delay(1000);
	  if (ubKeyNumber == 0x9)
      {
        ubKeyNumber = 0x00;
      }
      else
      {
        //LED_Display(++ubKeyNumber);
        ubKeyNumber++;
        /* Set the data to be transmitted */
        TxData[0] = ubKeyNumber;
        TxData[1] = 0xAD;

        /* Start the Transmission process */
        if (HAL_CAN_AddTxMessage(&CanHandle, &TxHeader, TxData, &TxMailbox) != HAL_OK)
        {
          /* Transmission request Error */
        	Error_Handler(10);

        }
        //LCD_UsrLog("ctr = %d\n",ubKeyNumber);
      }
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 200000000
  *            HCLK(Hz)                       = 200000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 400
  *            PLL_P                          = 2
  *            PLL_Q                          = 8
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  HAL_StatusTypeDef ret = HAL_OK;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  ASSERT(ret != HAL_OK);

  /* activate the OverDrive */
  ret = HAL_PWREx_ActivateOverDrive();
  ASSERT(ret != HAL_OK);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
  ASSERT(ret != HAL_OK);
}

/**
  * @brief  Display main demo messages.
  * @param  None
  * @retval None
  */
static void Display_DemoDescription(void)
{

  /* Set LCD Foreground Layer  */
  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);

  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

  /* Clear the LCD */
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
  BSP_LCD_Clear(LCD_COLOR_WHITE);

  /* Set the LCD Text Color */
  BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);

  /* Display LCD messages */
  BSP_LCD_DisplayStringAt(0, 10, (uint8_t *)"STM32F746G BSP", CENTER_MODE);
  BSP_LCD_DisplayStringAt(0, 35, (uint8_t *)"Drivers examples", CENTER_MODE);

  /* Draw Bitmap */
  BSP_LCD_DrawBitmap((BSP_LCD_GetXSize() - 80) / 2, 65, (uint8_t *)stlogo);

  BSP_LCD_SetFont(&Font12);
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 20, (uint8_t *)"Copyright (c) STMicroelectronics 2015", CENTER_MODE);

  BSP_LCD_SetFont(&Font16);
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, BSP_LCD_GetYSize() / 2 + 15, BSP_LCD_GetXSize(), 60);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() / 2 + 30, (uint8_t *)"Press User Button to start :", CENTER_MODE);
  //sprintf((char *)desc, "%s example", BSP_examples[DemoIndex].DemoName);
  //BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() / 2 + 45, (uint8_t *)desc, CENTER_MODE);
}

/**
  * @brief  Check for user input.
  * @param  None
  * @retval Input state (1 : active / 0 : Inactive)
  */
uint8_t CheckForUserInput(void)
{
  if (BSP_PB_GetState(BUTTON_KEY) != RESET)
  {
    HAL_Delay(10);
    while (BSP_PB_GetState(BUTTON_KEY) != RESET);
    return 1 ;
  }
  return 0;
}

/**
  * @brief EXTI line detection callbacks.
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  static uint32_t debounce_time = 0;

  if (GPIO_Pin == KEY_BUTTON_PIN)
  {
    /* Prevent debounce effect for user key */
    if ((HAL_GetTick() - debounce_time) > 50)
    {
      debounce_time = HAL_GetTick();
    }
  }
  //else if (GPIO_Pin == AUDIO_IN_INT_GPIO_PIN)
  //{
  //  /* Audio IN interrupt */
  //}
}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/**
  * @brief  Configure the MPU attributes
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU as Strongly ordered for not defined regions */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x00;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes as WT for SDRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0xC0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU QSPI flash */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x90000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Configure the MPU attributes FMC control registers */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0xA0000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_8KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * @brief  Configures the CAN, transmit and receive by polling
  * @param  None
  * @retval PASSED if the reception is well done, FAILED in other case
  */
HAL_StatusTypeDef CAN_Polling(void)
{
  CAN_FilterTypeDef  sFilterConfig;

  /*##-1- Configure the CAN peripheral #######################################*/
  CanHandle.Instance = CANx;

  CanHandle.Init.TimeTriggeredMode = DISABLE;
  CanHandle.Init.AutoBusOff = DISABLE;
  CanHandle.Init.AutoWakeUp = DISABLE;
  CanHandle.Init.AutoRetransmission = ENABLE;
  CanHandle.Init.ReceiveFifoLocked = DISABLE;
  CanHandle.Init.TransmitFifoPriority = DISABLE;
  CanHandle.Init.Mode = CAN_MODE_LOOPBACK;
  CanHandle.Init.SyncJumpWidth = CAN_SJW_1TQ;
  CanHandle.Init.TimeSeg1 = CAN_BS1_6TQ;
  CanHandle.Init.TimeSeg2 = CAN_BS2_2TQ;
  CanHandle.Init.Prescaler = 6;

  if(HAL_CAN_Init(&CanHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(5);
  }

  /*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if(HAL_CAN_ConfigFilter(&CanHandle, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    Error_Handler(6);
  }

  //HAL_CAN_Stop(&CanHandle);
  /*##-3- Start the CAN peripheral ###########################################*/
  if (HAL_CAN_Start(&CanHandle) != HAL_OK)
  {
    /* Start Error */
    Error_Handler(7);
  }

  /*##-4- Start the Transmission process #####################################*/
  TxHeader.StdId = 0x11;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.DLC = 2;
  TxHeader.TransmitGlobalTime = DISABLE;
  TxData[0] = 0xCA;
  TxData[1] = 0xFE;

  /* Request transmission */
  if(HAL_CAN_AddTxMessage(&CanHandle, &TxHeader, TxData, &TxMailbox) != HAL_OK)
  {
    /* Transmission request Error */
    Error_Handler(8);
  }

  /* Wait transmission complete */
  while(HAL_CAN_GetTxMailboxesFreeLevel(&CanHandle) != 3) {}

  /*##-5- Start the Reception process ########################################*/
  if(HAL_CAN_GetRxFifoFillLevel(&CanHandle, CAN_RX_FIFO0) != 1)
  {
    /* Reception Missing */
    Error_Handler(9);
  }

  if(HAL_CAN_GetRxMessage(&CanHandle, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler(10);
  }

  for(int i =0;i<8;i++)
  {
	  LCD_UsrLog("rxdata [%d] = 0X%x\n",i,RxData[i]);
  }


  if((RxHeader.StdId != 0x11)                     ||
     (RxHeader.RTR != CAN_RTR_DATA)               ||
     (RxHeader.IDE != CAN_ID_STD)                 ||
     (RxHeader.DLC != 2)                          ||
     ((RxData[0]<<8 | RxData[1]) != 0xCAFE))
  {
    /* Rx message Error */
    return HAL_ERROR;
  }

  return HAL_OK; /* Test Passed */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(int err_code)
{
	/* User may add here some code to deal with this error */
	/* Turn LED3 on */
	LCD_ErrLog("Error handler: %d\n",err_code);
	BSP_LED_On(LED1);
	//while(1)
	//{
	//}
}

/**
  * @brief  Configures the CAN.
  * @param  None
  * @retval None
  */
static void CAN_Config(void)
{
  CAN_FilterTypeDef  sFilterConfig;

  /*##-1- Configure the CAN peripheral #######################################*/
  CanHandle.Instance = CANx;

  CanHandle.Init.TimeTriggeredMode = DISABLE;
  CanHandle.Init.AutoBusOff = DISABLE;
  CanHandle.Init.AutoWakeUp = DISABLE;
  CanHandle.Init.AutoRetransmission = ENABLE;
  CanHandle.Init.ReceiveFifoLocked = DISABLE;
  CanHandle.Init.TransmitFifoPriority = DISABLE;
  CanHandle.Init.Mode = CAN_MODE_LOOPBACK;
  CanHandle.Init.SyncJumpWidth = CAN_SJW_1TQ;
  CanHandle.Init.TimeSeg1 = CAN_BS1_6TQ;
  CanHandle.Init.TimeSeg2 = CAN_BS2_2TQ;
  CanHandle.Init.Prescaler = 6;

  if (HAL_CAN_Init(&CanHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(1);
  }

  /*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&CanHandle, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    Error_Handler(2);
  }
  /*##-4- Activate CAN RX notification #######################################*/
  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler(4);
  }
  //HAL_Delay(2000);

  /*##-3- Start the CAN peripheral ###########################################*/
  if (HAL_CAN_Start(&CanHandle) != HAL_OK)
  {
    /* Start Error */
    Error_Handler(3);
  }



  /*##-5- Configure Transmission process #####################################*/
  TxHeader.StdId = 0x321;
  TxHeader.ExtId = 0x01;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.DLC = 2;
  TxHeader.TransmitGlobalTime = DISABLE;
}

/**
  * @brief  Rx Fifo 0 message pending callback
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	printf("no. of available rxmsgs = %lu -- ",HAL_CAN_GetRxFifoFillLevel(&CanHandle, CAN_RX_FIFO0));
  /* Get RX message */
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler(0);
  }

  //LCD_UsrLog("rxdata = %x\n",RxData[0]);

  /* Display LEDx */
  if ((RxHeader.StdId == 0x321) && (RxHeader.IDE == CAN_ID_STD) && (RxHeader.DLC == 2))
  {
    LCD_UsrLog("rxdata = %x\n",RxData[0]);
    ubKeyNumber = RxData[0];
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

