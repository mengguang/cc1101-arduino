#include <Arduino.h>
#include <cc1100_arduino.h>

//---------------------------=[Global variables]=----------------------------

//--------------------------[Global Task variables]--------------------------
uint32_t prev_millis_1s_timer = 0;

const uint16_t INTERVAL_1S_TIMER = 1000; // interval at which to blink (milliseconds)

//--------------------------[Global CC1100 variables]------------------------
uint8_t Tx_fifo[FIFOBUFFER], Rx_fifo[FIFOBUFFER];
uint8_t My_addr = 6;
uint8_t Rx_addr = 8;
uint8_t Channel = 10;
uint8_t Power = 0; // 0db
uint8_t Band = 2;  //set frequency 1=315MHz; 2=433MHz; 3=868MHz; 4=915MHz
uint8_t Pktlen, pktlen, Lqi, Rssi;
uint8_t rx_addr, sender, lqi;
int8_t rssi_dbm;

volatile uint8_t cc1101_packet_available;
void rf_available_int(void);

//--------------------------[class constructors]-----------------------------
//init CC1100 constructor
CC1100 cc1100;

//---------------------------------[SETUP]-----------------------------------
void setup()
{
  // init serial Port for debugging
  Serial.begin(115200);
  Serial.println();

  // init CC1101 RF-module and get My_address from EEPROM
  cc1100.begin(My_addr); //inits RF module with main default settings

  cc1100.sidle();                   //set to ILDE first
  cc1100.set_ISM(Band);                //set frequency 1=315MHz; 2=433MHz; 3=868MHz; 4=915MHz
  cc1100.set_channel(Channel);           //set channel
  cc1100.set_output_power_level(Power); //set PA level in dbm

  cc1100.show_main_settings();     //shows setting debug messages to UART
  cc1100.show_register_settings(); //shows current CC1101 register values
  cc1100.receive();                //set to RECEIVE mode

  // init interrrupt function for available packet
  attachInterrupt(GDO2, rf_available_int, RISING);

  Serial.println(F("CC1101 TX Demo")); //welcome message
}
//---------------------------------[LOOP]-----------------------------------
void loop()
{

  // one second update timer
  if (millis() - prev_millis_1s_timer >= INTERVAL_1S_TIMER)
  {
    uint32_t time_stamp = millis(); 
    Serial.println(time_stamp);
    Tx_fifo[3] = (uint8_t)(time_stamp >> 24);
    Tx_fifo[4] = (uint8_t)(time_stamp >> 16);
    Tx_fifo[5] = (uint8_t)(time_stamp >> 8);
    Tx_fifo[6] = (uint8_t)(time_stamp);

    Pktlen = 0x07; //set packet len to 0x13

    detachInterrupt(GDO2);                                     //disable pin change interrupt
    cc1100.sent_packet(My_addr, Rx_addr, Tx_fifo, Pktlen, 0); //sents package over air. ACK is received via GPIO polling
    attachInterrupt(GDO2, rf_available_int, RISING);           //enable pin change interrupt

    Serial.print(F("tx_time: "));
    Serial.print(millis() - time_stamp);
    Serial.println(F("ms"));
    prev_millis_1s_timer = millis();
  }
}
//--------------------------[end loop]----------------------------

//-----interrrupt function not needed for this demo
//---------------------[ check incomming RF packet ]-----------------------
void rf_available_int(void)
{
  detachInterrupt(GDO2);

  if (cc1100.packet_available() == true)
  {
    if (cc1100.get_payload(Rx_fifo, pktlen, rx_addr, sender, rssi_dbm, lqi) == true) //stores the payload data to Rx_fifo
    {
      cc1101_packet_available = true; //set flag that a package is in RX buffer
    }
    else
    {
      cc1101_packet_available = false; //set flag that an package is corrupted
    }
  }

  attachInterrupt(GDO2, rf_available_int, RISING);
}
