/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    subghz_phy_app.cpp
  * @author  MCD Application Team
  * @brief   Application of the SubGHz_Phy Middleware
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
#define HUB_DEVICE 0
/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "sys_app.h"
#include "subghz_phy_app.hpp"
#include "radio.h"

/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
#include "stm32_seq.h"
#include "utilities_def.h"
#include "app_version.h"
#include "subghz_phy_version.h"
#include "ProtocolYMessages.hpp"

static std::vector<uint8_t> messageBuffer;

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  RX,
  RX_TIMEOUT,
  RX_ERROR,
  TX,
  TX_TIMEOUT,
} States_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Configurations */
/*Timeout*/
#define RX_TIMEOUT_VALUE              10000
#define TX_TIMEOUT_VALUE              000
/* PING string*/
#define PING "PING"
/* PONG string*/
#define PONG "PONG"
/*Size of the payload to be sent*/
/* Size must be greater of equal the PING and PONG*/
#define MAX_APP_BUFFER_SIZE          255
#if (PAYLOAD_LEN > MAX_APP_BUFFER_SIZE)
#error PAYLOAD_LEN must be less or equal than MAX_APP_BUFFER_SIZE
#endif /* (PAYLOAD_LEN > MAX_APP_BUFFER_SIZE) */
/* wait for remote to be in Rx, before sending a Tx frame*/
#define RX_TIME_MARGIN                200
/* LED blink Period*/
#if(HUB_DEVICE == 1)
#define LED_PERIOD_MS                 1000
#else
#define LED_PERIOD_MS                 100
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Radio events function pointer */
static RadioEvents_t RadioEvents;

/* USER CODE BEGIN PV */
/*Ping Pong FSM states */
#if(HUB_DEVICE == 1)
static States_t State = TX;
#else
static States_t State = RX;
#endif
/* App Rx Buffer*/
static uint8_t BufferRx[MAX_APP_BUFFER_SIZE];
/* App Tx Buffer*/
static uint8_t BufferTx[MAX_APP_BUFFER_SIZE];
/* Last  Received Buffer Size*/
uint16_t RxBufferSize = 0;
/* Last  Received packer Rssi*/
int8_t RssiValue = 0;
/* Last  Received packer SNR (in Lora modulation)*/
int8_t SnrValue = 0;
/* Led Timers objects*/
static UTIL_TIMER_Object_t timerLed;
/* device state. Master: true, Slave: false*/
bool isMaster = true;
/* random delay to make sure 2 devices will sync*/
/* the closest the random delays are, the longer it will
   take for the devices to sync when started simultaneously*/
static int32_t random_delay;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/*!
 * @brief Function to be executed on Radio Tx Done event
 */
static void OnTxDone(void);

/**
  * @brief Function to be executed on Radio Rx Done event
  * @param  payload ptr of buffer received
  * @param  size buffer size
  * @param  rssi
  * @param  LoraSnr_FskCfo
  */
static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo);

/**
  * @brief Function executed on Radio Tx Timeout event
  */
static void OnTxTimeout(void);

/**
  * @brief Function executed on Radio Rx Timeout event
  */
static void OnRxTimeout(void);

/**
  * @brief Function executed on Radio Rx Error event
  */
static void OnRxError(void);

/* USER CODE BEGIN PFP */
/**
  * @brief  Function executed on when led timer elapses
  * @param  context ptr of LED context
  */
static void OnledEvent(void *context);

/**
  * @brief PingPong state machine implementation
  */
static void PingPong_Process(void);

/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SubghzApp_Init(void)
{
  /* USER CODE BEGIN SubghzApp_Init_1 */

  APP_LOG(TS_OFF, VLEVEL_M, "\n\rPING PONG\n\r");
  /* Get SubGHY_Phy APP version*/
  APP_LOG(TS_OFF, VLEVEL_M, "APPLICATION_VERSION: V%X.%X.%X\r\n",
          (uint8_t)(APP_VERSION_MAIN),
          (uint8_t)(APP_VERSION_SUB1),
          (uint8_t)(APP_VERSION_SUB2));

  /* Get MW SubGhz_Phy info */
  APP_LOG(TS_OFF, VLEVEL_M, "MW_RADIO_VERSION:    V%X.%X.%X\r\n",
          (uint8_t)(SUBGHZ_PHY_VERSION_MAIN),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB1),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB2));

  /* Led Timers*/
  UTIL_TIMER_Create(&timerLed, LED_PERIOD_MS, UTIL_TIMER_ONESHOT, OnledEvent, NULL);
  UTIL_TIMER_Start(&timerLed);
  /* USER CODE END SubghzApp_Init_1 */

  /* Radio initialization */
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;

  Radio.Init(&RadioEvents);

  /* USER CODE BEGIN SubghzApp_Init_2 */
  /*calculate random delay for synchronization*/
  random_delay = 0;//(Radio.Random()) >> 22; /*10bits random e.g. from 0 to 1023 ms*/

  /* Radio Set frequency */
  Radio.SetChannel(RF_FREQUENCY);

  /* Radio configuration */
APP_LOG(TS_OFF, VLEVEL_M, "---------------\n\r");
APP_LOG(TS_OFF, VLEVEL_M, "LORA_MODULATION\n\r");
APP_LOG(TS_OFF, VLEVEL_M, "LORA_BW=%d kHz\n\r", (1 << LORA_BANDWIDTH) * 125);
APP_LOG(TS_OFF, VLEVEL_M, "LORA_SF=%d\n\r", LORA_SPREADING_FACTOR);

Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                  LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                  LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                  true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);

Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                  LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                  LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                  0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

Radio.SetMaxPayloadLength(MODEM_LORA, MAX_APP_BUFFER_SIZE);

  /*fills tx buffer*/
  memset(BufferTx, 0x0, MAX_APP_BUFFER_SIZE);

  APP_LOG(TS_ON, VLEVEL_L, "rand=%d\n\r", random_delay);
  /*starts reception*/
  Radio.Rx(RX_TIMEOUT_VALUE + random_delay);

  /*register task to to be run in while(1) after Radio IT*/
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), UTIL_SEQ_RFU, PingPong_Process);
  /* USER CODE END SubghzApp_Init_2 */

  ProtocolYMessages message(PROTOCOL_Y::MessageTypes::SYNC_BEACON);
  int retValue = message.buildMessage();
  

  if (retValue == static_cast<int>(RCODE::SUCCESS))
  {
      message.buildMessage();
      messageBuffer = message.getMessageBuffer();
  }
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
static void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone */
  APP_LOG(TS_ON, VLEVEL_L, "OnTxDone\n\r");
  /* Update the State of the FSM*/
  State = TX;
  /* Run PingPong process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxDone */
}

static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo)
{
  /* USER CODE BEGIN OnRxDone */
  APP_LOG(TS_ON, VLEVEL_L, "OnRxDone\n\r");
  APP_LOG(TS_ON, VLEVEL_L, "RssiValue=%d dBm, SnrValue=%ddB\n\r", rssi, LoraSnr_FskCfo);
  /* Record payload Signal to noise ratio in Lora*/
  SnrValue = LoraSnr_FskCfo;

  /* Update the State of the FSM*/
  State = RX;
  /* Clear BufferRx*/
  memset(BufferRx, 0, MAX_APP_BUFFER_SIZE);
  /* Record payload size*/
  RxBufferSize = size;
  if (RxBufferSize <= MAX_APP_BUFFER_SIZE)
  {
    memcpy(BufferRx, payload, RxBufferSize);
  }
  /* Record Received Signal Strength*/
  RssiValue = rssi;
  /* Record payload content*/
  APP_LOG(TS_ON, VLEVEL_L, "Rx: %s\n\r", BufferRx);
  /* Run PingPong process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxDone */
}

static void OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout */
  APP_LOG(TS_ON, VLEVEL_L, "OnTxTimeout\n\r");
  /* Update the State of the FSM*/
  State = TX_TIMEOUT;
  /* Run PingPong process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxTimeout */
}

static void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout */
  APP_LOG(TS_ON, VLEVEL_L, "OnRxTimeout\n\r");
  /* Update the State of the FSM*/
  State = RX_TIMEOUT;
  /* Run PingPong process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxTimeout */
}

static void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError */
  APP_LOG(TS_ON, VLEVEL_L, "OnRxError\n\r");
  /* Update the State of the FSM*/
  State = RX_ERROR;
  /* Run PingPong process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxError */
}

/* USER CODE BEGIN PrFD */
static void PingPong_Process(void)
{
  Radio.Sleep();
  #if(HUB_DEVICE == 1)
  // Do nothing
  #else
    if(State == RX && RxBufferSize > 0) {
      UTIL_TIMER_Stop(&timerLed);
      /* switch off green led */
      HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); /* LED_GREEN */
      /* master toggles red led */
      HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin); /* LED_RED */
      
      ProtocolYMessages message(PROTOCOL_Y::MessageTypes::RECEIVE_MODE);
    
      if (message.parseMessage(messageBuffer.data()) == static_cast<int>(RCODE::SUCCESS))
      {
        uint8_t messageType = message.getMessageType();
        APP_LOG(TS_ON, VLEVEL_L, "Received message type: %d\n\r", messageType);
        if (messageType == static_cast<uint8_t>(PROTOCOL_Y::MessageTypes::SYNC_BEACON))
        {
          APP_LOG(TS_ON, VLEVEL_L, "Received SYNC_BEACON\n\r");
          syncBeaconMessage_t syncBeaconMessage = message.getSyncBeaconMessage();
          APP_LOG(TS_ON, VLEVEL_L, "appId=%d\n\r", syncBeaconMessage.appId);
          APP_LOG(TS_ON, VLEVEL_L, "messageType=%d\n\r", syncBeaconMessage.messageType);
          APP_LOG(TS_ON, VLEVEL_L, "unencryptedMessageCounter=%d\n\r", syncBeaconMessage.unencryptedMessageCounter);
          APP_LOG(TS_ON, VLEVEL_L, "payloadLength=%d\n\r", syncBeaconMessage.payloadLength);
          APP_LOG(TS_ON, VLEVEL_L, "protocolVersion=%d\n\r", syncBeaconMessage.protocolVersion);
          APP_LOG(TS_ON, VLEVEL_L, "unencryptedMaxNodes=%d\n\r", syncBeaconMessage.unencryptedMaxNodes);
          APP_LOG(TS_ON, VLEVEL_L, "encryptedMessageCounter=%d\n\r", syncBeaconMessage.encryptedMessageCounter);
          APP_LOG(TS_ON, VLEVEL_L, "encryptedMaxNodes=%d\n\r", syncBeaconMessage.encryptedMaxNodes);
          APP_LOG(TS_ON, VLEVEL_L, "frequencyHoppingPlan=%d\n\r", syncBeaconMessage.frequencyHoppingPlan);
          APP_LOG(TS_ON, VLEVEL_L, "crc=%d\n\r", syncBeaconMessage.crc);
          for (size_t i = 0; i < syncBeaconMessage.payloadLength; i++)
          {
            APP_LOG(TS_ON, VLEVEL_L, "payload[%d]=%d\n\r", i, syncBeaconMessage.payload[i]);
          }
          APP_LOG(TS_ON, VLEVEL_L, "RSSI=%d\n\r", RssiValue);
          APP_LOG(TS_ON, VLEVEL_L, "SNR=%d\n\r", SnrValue);
          APP_LOG(TS_ON, VLEVEL_L, "RxBufferSize=%d\n\r", RxBufferSize);
        }
      }
    }
    else {
      APP_LOG(TS_ON, VLEVEL_L, "TP:RxTimeout\n\r");
      UTIL_TIMER_Start(&timerLed);
    }
    Radio.Rx(RX_TIMEOUT_VALUE);
  #endif
}

static void OnledEvent(void *context)
{
  #if(HUB_DEVICE == 1)
  /* switch off red led */
  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET); /* LED_RED */
  /* master toggles green led */
  HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin); /* LED_GREEN */
  Radio.Send(messageBuffer.data(), static_cast<uint8_t>(messageBuffer.size()));
  for (size_t i = 0; i < messageBuffer.size(); i++)
  {
    APP_LOG(TS_ON, VLEVEL_L, "TX[%d]=%d\n\r", i, messageBuffer[i]);
  }
  return;
  #endif
  HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin); /* LED_GREEN */
  HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin); /* LED_RED */
  UTIL_TIMER_Start(&timerLed);
}

/* USER CODE END PrFD */