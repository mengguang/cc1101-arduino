#include <Arduino.h>
#include <cc1100_arduino.h>

uint32_t prev_millis_1s_timer = 0;
const uint16_t INTERVAL_1S_TIMER = 1000;

uint8_t Tx_fifo[FIFOBUFFER], Rx_fifo[FIFOBUFFER];

uint8_t SelfAddress = 6;
uint8_t PeerAddress = 8;
//set ISM Band 1=315MHz; 2=433MHz; 3=868MHz; 4=915MHz
uint8_t Band = 2;
uint8_t Channel = 10;
//set PA level in dbm
uint8_t Power = 0;

uint8_t Pktlen, pktlen, Lqi, Rssi;
uint8_t rx_addr, sender, lqi;
int8_t rssi_dbm;

volatile uint8_t cc1101_packet_available;
void rf_available_int(void);

CC1100 cc1100;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  cc1100.begin(SelfAddress, Band, Channel, Power);

  cc1100.sidle();

  // cc1100.set_ISM(Band);
  // cc1100.set_channel(Channel);
  // cc1100.set_output_power_level(Power);

  cc1100.show_register_settings();
  cc1100.receive();

  // init interrrupt function for available packet
  attachInterrupt(GDO2, rf_available_int, RISING);

  Serial.println(F("CC1101 TX Demo"));
}

void loop()
{
  if (millis() - prev_millis_1s_timer >= INTERVAL_1S_TIMER)
  {
    uint32_t time_stamp = millis();
    Serial.println(time_stamp);
    Tx_fifo[3] = (uint8_t)(time_stamp >> 24);
    Tx_fifo[4] = (uint8_t)(time_stamp >> 16);
    Tx_fifo[5] = (uint8_t)(time_stamp >> 8);
    Tx_fifo[6] = (uint8_t)(time_stamp);

    Pktlen = 0x07;

    detachInterrupt(GDO2);
    cc1100.sent_packet(SelfAddress, PeerAddress, Tx_fifo, Pktlen, 0);
    attachInterrupt(GDO2, rf_available_int, RISING);

    Serial.print(F("tx_time: "));
    Serial.print(millis() - time_stamp);
    Serial.println(F("ms"));
    prev_millis_1s_timer = millis();
  }
}

void rf_available_int(void)
{
  detachInterrupt(GDO2);

  if (cc1100.packet_available() == true)
  {
    //stores the payload data to Rx_fifo
    if (cc1100.get_payload(Rx_fifo, pktlen, rx_addr, sender, rssi_dbm, lqi) == true)
    {
      //set flag that a package is in RX buffer
      cc1101_packet_available = true;
    }
    else
    {
      //set flag that an package is corrupted
      cc1101_packet_available = false;
    }
  }
  attachInterrupt(GDO2, rf_available_int, RISING);
}
