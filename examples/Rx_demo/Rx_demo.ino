#include <Arduino.h>
#include <cc1100_arduino.h>

uint8_t Tx_fifo[FIFOBUFFER], Rx_fifo[FIFOBUFFER];
uint8_t Tx_addr;
uint8_t Pktlen, pktlen, Lqi, Rssi;
uint8_t rx_addr, sender, lqi;
int8_t rssi_dbm;
volatile uint8_t cc1101_packet_available;

uint8_t My_addr = 6;
//set ISM Band 1=315MHz; 2=433MHz; 3=868MHz; 4=915MHz
uint8_t Band = 2;
uint8_t Channel = 10;
//set PA level in dbm
uint8_t Power = 0;

CC1100 cc1100;
void rf_available_int(void);

void setup()
{
  Serial.begin(115200);
  Serial.println();

  cc1100.begin(My_addr);
  cc1100.sidle();

  cc1100.set_ISM(Band);
  cc1100.set_channel(Channel);
  cc1100.set_output_power_level(Power);

  cc1100.show_main_settings();
  cc1100.show_register_settings();

  cc1100.receive();

  // init interrrupt function for available packet
  attachInterrupt(GDO2, rf_available_int, RISING);

  Serial.println(F("CC1101 RX Demo")); //welcome message
}

//---------------------------------[LOOP]-----------------------------------
void loop()
{
  //if valid package is received
  if (cc1101_packet_available == true)
  {
    uint32_t rf_timecode = ((uint32_t)Rx_fifo[3] << 24) +
                           ((uint32_t)Rx_fifo[4] << 16) +
                           ((uint16_t)Rx_fifo[5] << 8) +
                           Rx_fifo[6];
    Serial.print(F("TX_Timecode: "));
    Serial.print(rf_timecode);
    Serial.println(F("ms\n"));
    cc1101_packet_available = false;
  }
}

void rf_available_int(void)
{
  detachInterrupt(GDO2);
  uint32_t time_stamp = millis();
  Serial.println("rf_available_int");
  if (cc1100.packet_available() == true)
  {
    //stores the payload data to Rx_fifo
    if (cc1100.get_payload(Rx_fifo, pktlen, rx_addr, sender, rssi_dbm, lqi) == true)
    {
      //set flag that a package is in RX buffer
      cc1101_packet_available = true;
      Serial.print(F("rx_time: "));
      Serial.print(millis() - time_stamp);
      Serial.println(F("ms"));
    }
    else
    {
      //set flag that an package is corrupted
      cc1101_packet_available = false;
    }
  }
  attachInterrupt(GDO2, rf_available_int, RISING);
}
