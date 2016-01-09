/*
 * CANBusHello.ino
 * "Hello, world" for CAN Bus, using Arduino and MCP2515-based controller
 *
 * Copyright (C) 2013-2016 Chad Gibbons
 * All Rights Reserved
 */

#include <AVRDebug.h>

#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mcp2515.h>                    // CAN Bus library for Sparkfun / SK Pang shield
                                        // https://github.com/sparkfun/SparkFun_CAN-Bus_Arduino_Library

#define CANSPEED_125 	7		// CAN speed at 125 kbps
#define CANSPEED_250  	3		// CAN speed at 250 kbps
#define CANSPEED_500	1		// CAN speed at 500 kbps

#define LED2            7               // LED #2 pin
#define LED3            13              // LED #3 pin
#define MCP2515_INT     2               // MCP2515 interrupt pin
#define MCP2515_CS      10              // MCP2515 chip-select pin

void setup()
{
  Debug.begin(9600);

  delay(1000);
  PRINT("Starting!\n");
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  
  for (int tries = 0; tries < 10; tries++)
  {  
    PRINT("Attempting #%d to initialize CAN-Bus\n", tries+1);

    char ret = mcp2515_init(CANSPEED_500);
    if (ret)
    {
      Serial.println("mcp2515 CAN-Bus controller successfully initialized\n");
      digitalWrite(LED2, HIGH);
      break;
    }
    else
    {
      Serial.println("mcp2515 CAN-Bus controller could not initialize\n");
    }
    delay(500);
  }
}

void print_can_message(tCAN *message)
{
  char buffer[80];

  uint8_t length = message->header.length;

  int n = sprintf(&buffer[0], "id:0x%3x len:%d rtr:%d ", message->id, length, message->header.rtr);

  if (!message->header.rtr) {
    n += sprintf(&buffer[n], "data:");

    for (uint8_t i = 0; i < length; i++) {
      n += sprintf(&buffer[n], "0x%02x ", message->data[i]);
    }

    for (uint8_t i = 0; i < length; i++) {
      n += sprintf(&buffer[n], "%c", isprint(message->data[i]) ? (const char)message->data[i] : '.');
    }
  }
  PRINT("%s\n", buffer);
}

uint32_t messages = 0;

void loop()
{
  tCAN can_message;
  memset(&can_message, 0, sizeof(can_message));
  can_message.id = 0x1ff;
  can_message.header.length = 8;
  snprintf((char*)&can_message.data, 8, "%d", messages);

  PRINT("Sending ");
  print_can_message(&can_message);
  
  digitalWrite(LED2, HIGH);
  if (!mcp2515_send_message(&can_message))
  {
    PRINT("Unable to send message to mcp2515\n");
    digitalWrite(LED3, HIGH);
  }
  else
  {
    digitalWrite(LED3, LOW);
  }
  digitalWrite(LED2, LOW);
  delay(500);
  messages++;
}

