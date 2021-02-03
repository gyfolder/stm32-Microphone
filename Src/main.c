/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "MyMath.h"
#include "MyOLED.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
uint16_t MCP3202_GetData(SPI_HandleTypeDef* MCP3202SPI){
	uint16_t Data = 0;
	uint8_t send[3],receive[3];
	GPIOA->BSRR = SPI1_SS_Pin<<16;
	send[0] = 0x01;send[1] = 0xE0;send[2] = 0xFF;
	HAL_SPI_TransmitReceive(MCP3202SPI,send,receive,3,100);
	GPIOA->BSRR = SPI1_SS_Pin;
	Data = (receive[1]&0xf)<<8 | receive[2];
	return Data;
}
char Str[250] = {0,};
uint16_t Sample[512];
uint32_t AmountSum = 0;
int Button[5] = {0,};
void OLED_SPI_Transmit(unsigned char *pData, uint16_t Size, uint32_t Timeout){
	HAL_SPI_Transmit(&hspi2, pData, Size, Timeout);
}
float pow2(float Value){
	return Value*Value;
}
float VecDistance(Complex Vect){
	return sqrt(pow2(Vect.re)+pow2(Vect.im));
}
uint32_t Delay=0;
#define delay(time) Delay=time;while(Delay--){;}
#define UpBt		if(GPIOA->IDR&(1<<(8)))Button[0]++;else
#define DnBt		if(GPIOA->IDR&(1<<(9)))Button[1]++;else
#define ModeBt		if(GPIOB->IDR&(1<<(3)))Button[2]++;else Button[2]=0;if(Button[2]==1)
#define RenderBt	if(GPIOA->IDR&(1<<(11)))Button[3]++;else Button[3]=0;if(Button[3]==1)
#define Bt1Bt		if(GPIOB->IDR&(1<<(4)))Button[4]++;else Button[4]=0;if(Button[4]==1)
uint32_t tick0 = 0,Getms = 0,GetFTms=0;
int bufflen = 512,Autorender=0,mode=0,Getdelaytime = 500,displaymode = 0,AddDot=0;
double Sensitivity = 1.0;//°¨µµ
vecRef input = 0;
void FTRender(){
	input = vecAlloc(bufflen);
	tick0 = HAL_GetTick();
	for(int i=0;i<bufflen;i++){
		input->arr[i].re = (double)((double)MCP3202_GetData(&hspi1)-2048.0)/128.0;
		delay(Getdelaytime);
	}
	Getms = HAL_GetTick()-tick0;
	tick0 = HAL_GetTick();
	if(displaymode==0)fft(input, &input);
	GetFTms = HAL_GetTick()-tick0;
}
uint8_t HzBuf[5] = {0xAE,0xA2,0xE4,0xA8,0xAE};
uint8_t CommaBuf[5] = {0x00,0x00,0x00,0x40,0x40};
uint8_t AutoBuf[6] = {0xf8,0xd8,0xa8,0x88,0xa8,0xf8};
uint8_t StatusBarBuf[4][14] = {
	{0xff,0xfc
		,0x90,0x24
		,0xa0,0x14
		,0xff,0xfc
		,0xa0,0x14
		,0x90,0x24
		,0xff,0xfc
	},{0xff,0xfc
		,0x90,0x04
		,0x90,0x04
		,0xfc,0xfc
		,0x90,0x04
		,0x90,0x04
		,0xff,0xfc
	},{0xff,0xfc
		,0x90,0x24
		,0xb8,0x24
		,0xd4,0xac
		,0x90,0x74
		,0x90,0x24
		,0xff,0xfc
	},{0xff,0xfc
		,0xbf,0x6c
		,0xbe,0xc4
		,0x94,0x6c
		,0xa4,0xfc
		,0x8a,0xc4
		,0xff,0xfc
	}
};
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
  //{begin
  SetRESPin((void*)GPIOB, 11);
  SetDCPin((void*)GPIOB, 12);
  SetSSPin((void*)GPIOB, 14);

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_SPI1_Init();
  MX_SPI2_Init();

  /* USER CODE BEGIN 2 */
  HAL_Delay(100);
	OLEDinit();
	NoShowDrawString(0,0,OLED_Char8X16,"STM32F103 FT V1");
	ShowBuffer();HAL_Delay(2000);
	NoShowDrawString(0,0,OLED_Char8X16,"FourierTransform");ShowBuffer();HAL_Delay(1000);ClearBuffer(0);
	NoShowDrawString(0,0,OLED_Char8X16,"STM32F103 FT V1");
	NoShowDrawString(0,24,OLED_Char8X16,"    Made by");ShowBuffer();HAL_Delay(1000);
	NoShowDrawString(0,40,OLED_Char8X16,"  Park Giyong.");ShowBuffer();HAL_Delay(1000);
	NoShowDrawString(0,56,OLED_Char6X8,"  2018.03.04 21:55");ShowBuffer();HAL_Delay(2000);
	FTRender();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //}
  int len=0;
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  len=0;
	ModeBt{if(mode!=3)mode++;else mode=0;}
	RenderBt{if(!Autorender)FTRender();else {AddDot=0;Sensitivity=1;if(displaymode)displaymode=0;else displaymode=1;}}
	else if(Autorender){NoShowDrawPicture(119, 0, 5, 6, AutoBuf, 1);FTRender();}
	Bt1Bt{if(Autorender)Autorender=0;else Autorender=1;}
	//sprintf(Str,"%ld",Getms);NoShowDrawNumStr(1, 0, OLED_Num3X5, Str, 0);int len=strlen(Str);
	//sprintf(Str,"%d %d %d %d %d %d %d %d",Button[0],Button[1],Button[2],Button[3],Button[4],Autorender,displaymode,mode);
	//NoShowDrawNumStr(1, 6, OLED_Num3X5, Str, 0);
	//NoShowDrawPicture((len<<2)+1, 0, 3, 5, CommaBuf, 1);len++;
	//sprintf(Str,"%ld",GetFTms);NoShowDrawNumStr((len<<2)+1, 0, OLED_Num3X5, Str, 0);len+=strlen(Str);
	//NoShowDrawPicture((len<<2)+1, 0, 3, 5, CommaBuf, 1);len++;
	sprintf(Str,"%ld",(uint32_t)((1000.0/(double)Getms)*(AddDot+1)));NoShowDrawNumStr((len<<2)+1, 0, OLED_Num3X5, Str, 0);len+=strlen(Str);
	NoShowDrawPicture((len<<2)+1, 0, 7, 5, HzBuf, 1);len+=2;
	sprintf(Str," %ld",(uint32_t)((1000.0/(double)Getms)*(128.0+AddDot)));NoShowDrawNumStr((len<<2)+1, 0, OLED_Num3X5, Str, 0);len+=strlen(Str);
	NoShowDrawPicture((len<<2)+1, 0, 7, 5, HzBuf, 1);len+=2;
	sprintf(Str," %d",Button[0]+Button[1]+Button[2]+Button[3]+Button[4]);NoShowDrawNumStr((len<<2)+1, 0, OLED_Num3X5, Str, 0);len+=strlen(Str);
	if(mode==0){sprintf(Str," %d",AddDot);NoShowDrawNumStr((len<<2)+1, 0, OLED_Num3X5, Str, 0);len+=strlen(Str);}
	else if(mode==1){sprintf(Str," %d",Getdelaytime);NoShowDrawNumStr((len<<2)+1, 0, OLED_Num3X5, Str, 0);len+=strlen(Str);}
	else if(mode==2){sprintf(Str," %ld",(uint32_t)Sensitivity);NoShowDrawNumStr((len<<2)+1, 0, OLED_Num3X5, Str, 0);len+=strlen(Str);}
	else if(mode==3){sprintf(Str," %d",bufflen);NoShowDrawNumStr((len<<2)+1, 0, OLED_Num3X5, Str, 0);len+=strlen(Str);}
	NoShowDrawPicture(105, 0, 14, 7, StatusBarBuf[mode], 1);
	UpBt{
		switch(mode){
		case 0:if(AddDot+Button[0]<=bufflen-128)AddDot+=Button[0];break;
		case 1:Getdelaytime+=Button[0];break;
		case 2:Sensitivity *= 1.5;break;
		case 3:if(bufflen+Button[0] <= 2048)bufflen+=Button[0];break;
		}
		Button[0]=0;
	}
	DnBt{
		switch(mode){
		case 0:if(AddDot-Button[1]>=0)AddDot-=Button[1];break;
		case 1:if(Getdelaytime-Button[1] >= 0)Getdelaytime-=Button[1];break;
		case 2:Sensitivity /= 1.5;break;
		case 3:if(bufflen-Button[1] >= 0)bufflen-=Button[1];break;
		}
		Button[1]=0;
	}
	if(displaymode){
		for(int i=0;i<127;i++){
			NoShowDrawLine(i,60-(input->arr[i+AddDot].re+1.0)*22.5*Sensitivity,i+1,60-(input->arr[i+1+AddDot].re+1.0)*22.5*Sensitivity,1);
		}
	}else{
		for(int i=0;i<127;i++){
			NoShowDrawLine(i,60-VecDistance(input->arr[i+AddDot])*22.5*Sensitivity,i+1,60-VecDistance(input->arr[i+1+AddDot])*22.5*Sensitivity,1);
		}
	}
	ShowBuffer();ClearBuffer(0);
    vecRelease(&input);
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI2 init function */
static void MX_SPI2_Init(void)
{

  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_1LINE;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_SS_GPIO_Port, SPI1_SS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, OLED_RES_Pin|OLED_DC_Pin|OLED_SS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI1_SS_Pin */
  GPIO_InitStruct.Pin = SPI1_SS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI1_SS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : OLED_RES_Pin OLED_DC_Pin OLED_SS_Pin */
  GPIO_InitStruct.Pin = OLED_RES_Pin|OLED_DC_Pin|OLED_SS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : Up_Pin Down_Pin render_Pin */
  GPIO_InitStruct.Pin = Up_Pin|Down_Pin|render_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : mode_Pin Bt1_Pin */
  GPIO_InitStruct.Pin = mode_Pin|Bt1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
