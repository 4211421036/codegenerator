/*
 * This code is purposed to run with the Artemis OBC and Artemis PDU.
 *
 * Functionality:
 * Send commands to the Artemis PDU.
 *
 * For example:
 *    Controlling switches on a PDU
 *    Reading PDU switch status
 *
 */

#include <stdint.h>
#include <string>
#include <string.h>

#define PDU_CMD_OFFSET 48

enum class PDU_Type : uint8_t
{
  NOP,
  CommandPing,
  CommandSetSwitch,
  CommandGetSwitchStatus,
  DataPong,
  DataSwitchStatus,
  DataSwitchTelem,
};

enum class PDU_SW : uint8_t
{
  None,
  All,
  SW_3V3_1,
  SW_3V3_2,
  SW_5V_1,
  SW_5V_2,
  SW_5V_3,
  SW_5V_4,
  SW_12V,
  VBATT,
  WDT,
  HBRIDGE1,
  HBRIDGE2,
  BURN,
  BURN1,
  BURN2,
  RPI,
};

struct __attribute__((packed)) pdu_packet
{
  PDU_Type type = PDU_Type::NOP;
  PDU_SW sw = PDU_SW::None;
  uint8_t sw_state = 0;
};

struct __attribute__((packed)) pdu_telem
{
  PDU_Type type = PDU_Type::DataSwitchTelem;
  uint8_t sw_state[12];
};

int32_t pdu_send(pdu_packet packet);
int32_t pdu_recv(String &response);
uint8_t get_sw(String str);
void display_options();
void display_switches();

elapsedMillis timeout;

void setup()
{
  pdu_packet pdu_packet;
  String response;

  Serial.begin(115200);
  Serial1.begin(115200); // Begin Serial Connection with the Artemis PDU on Serial1

  while (!Serial || !Serial1)
    ;

  // Ensure PDU is communicating with Teensy
  pdu_packet.type = PDU_Type::CommandPing;
  while (1)
  {
    pdu_send(pdu_packet);
    pdu_recv(response);
    if (response[0] == (uint8_t)PDU_Type::DataPong + PDU_CMD_OFFSET)
    {
      Serial.println("PDU Connection Established");
      break;
    }
    delay(100);
  }

  display_options();
}

void loop()
{
  pdu_packet pdu_packet;
  String response;

  if (Serial.available() > 0)
  {
    String input = Serial.readString();
    Serial.println(input);
    input = input.toLowerCase();

    if (input.indexOf("help") >= 0)
    {
      display_options();
    }
    else if (input.indexOf("list") >= 0)
    {
      display_switches();
    }
    else if (input.indexOf("ping") >= 0)
    {
      pdu_packet.type = PDU_Type::CommandPing;

      timeout = 0;
      while (1)
      {
        pdu_send(pdu_packet);
        pdu_recv(response);
        if (response[0] == (uint8_t)PDU_Type::DataPong + PDU_CMD_OFFSET)
        {
          Serial.println("Got Pong");
          break;
        }

        if (timeout > 5000)
        {
          Serial.println("Unable to Ping PDU");
          break;
        }

        delay(100);
      }

      Serial.print("$ ");
    }
    else if (input.indexOf("get") >= 0 || input.indexOf("set") >= 0)
    {
      uint8_t sw = get_sw(input);
      if (sw < 0)
      {
        Serial.println("Invalid Switch");
        display_options();
        goto end;
      }
      pdu_packet.sw = (PDU_SW)sw;

      if (input.indexOf("set") >= 0)
      {
        pdu_packet.type = PDU_Type::CommandSetSwitch;

        if (input.indexOf("on") > 0)
        {
          pdu_packet.sw_state = 1;
        }
        else if (input.indexOf("off") > 0)
        {
          pdu_packet.sw_state = 0;
        }
        else
        {
          Serial.println("Please indicate ON or OFF");
          goto end;
        }
      }
      else if (input.indexOf("get") >= 0)
      {
        pdu_packet.type = PDU_Type::CommandGetSwitchStatus;
      }

      while (1)
      {
        pdu_send(pdu_packet);

        timeout = 0;
        int attempts = 1;
        while (pdu_recv(response) < 0)
        {
          if (timeout > 5000)
          {
            Serial.print("Attempt ");
            Serial.print(attempts);
            Serial.println(": FAIL TO SEND CMD TO PDU");
            timeout = 0;

            if (++attempts == 5)
            {
              goto end;
            }
          }
        }
        if ((response[1] == sw + PDU_CMD_OFFSET) || (sw == (uint8_t)PDU_SW::All && response[0] == (uint8_t)PDU_Type::DataSwitchTelem + PDU_CMD_OFFSET))
        {
          break;
        }

        delay(100);
      }

      Serial.print("UART RECV: ");
      Serial.println(response.c_str());
      Serial.print("$ ");
      goto end;
    }
    else
    {
      Serial.println("Invalid Command (Type 'help' for help)");
      Serial.print("$ ");
      goto end;
    }
  }

end:;
}

int32_t pdu_send(pdu_packet packet)
{
  char *cmd = (char *)malloc(sizeof(packet));
  memcpy(cmd, &packet, sizeof(packet));

  std::string msg = "";
  for (size_t i = 0; i < sizeof(packet); i++)
  {
    msg += (cmd[i] + PDU_CMD_OFFSET);
  }
  Serial1.print(msg.c_str());
  Serial1.print('\n');
  Serial1.flush();

  Serial.print("SENDING TO PDU: [");
  for (size_t i = 0; i < msg.length(); i++)
  {
    Serial.print((unsigned)(msg[i] - PDU_CMD_OFFSET));
  }
  Serial.println(']');

  free(cmd);
  delay(1);
  return 0;
}

int32_t pdu_recv(String &response)
{
  if (Serial1.available() > 0)
  {
    String UART1_RX = Serial1.readString();
    if (UART1_RX.length() > 0)
    {
      response = UART1_RX;

      Serial1.clear();
      return UART1_RX.length();
    }
  }

  return -1;
}

uint8_t get_sw(String str)
{
  if (str.indexOf("sw_3v3_1") > 0)
  {
    return (uint8_t)PDU_SW::SW_3V3_1;
  }
  if (str.indexOf("sw_3v3_2") > 0)
  {
    return (uint8_t)PDU_SW::SW_3V3_2;
  }
  if (str.indexOf("sw_5v_1") > 0)
  {
    return (uint8_t)PDU_SW::SW_5V_1;
  }
  if (str.indexOf("sw_5v_2") > 0)
  {
    return (uint8_t)PDU_SW::SW_5V_2;
  }
  if (str.indexOf("sw_5v_3") > 0)
  {
    return (uint8_t)PDU_SW::SW_5V_3;
  }
  if (str.indexOf("sw_5v_4") > 0)
  {
    return (uint8_t)PDU_SW::SW_5V_4;
  }
  if (str.indexOf("sw_12v") > 0)
  {
    return (uint8_t)PDU_SW::SW_12V;
  }
  if (str.indexOf("vbatt") > 0)
  {
    return (uint8_t)PDU_SW::VBATT;
  }
  if (str.indexOf("hbridge1") > 0)
  {
    return (uint8_t)PDU_SW::HBRIDGE1;
  }
  if (str.indexOf("hbridge2") > 0)
  {
    return (uint8_t)PDU_SW::HBRIDGE2;
  }
  if (str.indexOf("burnall") > 0)
  {
    return (uint8_t)PDU_SW::BURN;
  }
  if (str.indexOf("burn1") > 0)
  {
    return (uint8_t)PDU_SW::BURN1;
  }
  if (str.indexOf("burn2") > 0)
  {
    return (uint8_t)PDU_SW::BURN2;
  }
  if (str.indexOf("all") > 0)
  {
    return (uint8_t)PDU_SW::All;
  }
  return 0;
}

void display_options()
{
  Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  Serial.println("PDU Command Options");
  Serial.println("========================================================");
  Serial.println("(1) Ping");
  Serial.println("\tExample Usage:\t$ Ping");
  Serial.println("(2) Set Switch ON/OFF");
  Serial.println("\tExample Usage:\t$ SET SW_3V3_1 ON");
  Serial.println("(3) Get Switch Status");
  Serial.println("\tExample Usage:\t$ GET SW_3V3_1");
  Serial.println("(4) List Available Switches");
  Serial.println("\tExample Usage:\t$ List");
  Serial.print("\n\n$ ");
}

void display_switches()
{
  Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  Serial.println("Available Switches");
  Serial.println("========================================================");
  Serial.println("\t SW_3V3_1");
  Serial.println("\t SW_3V3_2");
  Serial.println("\t SW_5V_1");
  Serial.println("\t SW_5V_2");
  Serial.println("\t SW_5V_3");
  Serial.println("\t SW_5V_4");
  Serial.println("\t VBATT");
  Serial.println("\t HBRIDGE1");
  Serial.println("\t HBRIDGE2");
  Serial.println("\t BURNALL");
  Serial.println("\t BURN1");
  Serial.println("\t BURN2");
  Serial.print("\n\n$ ");
}