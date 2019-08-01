CC1101
======

This Project is based on :  
https://github.com/SpaceTeddy/CC1101  

driver library for Ti CC1100 / CC1101.<br />

Hardware connection
===================

CC1101 Vdd = 3.3V
CC1101 max. digital voltage level = 3.3V (not 5V tolerant)

```
CC1101<->Arduino

Vdd    -    3.3V
SI     -    MOSI (11)
SO     -    MISO (12)
CS     -    SS   (10)
SCLK   -    SCK  (13)
GDO2   -    GPIO ( 3)
GDO0   -    not used in this demo
GND    -    GND


General description of RF packet
================================

```
-> pkt_len [1byte] | rx_addr [1byte] | tx_addr [1byte] | payload data [1..60bytes]
```

pkt_len = count of bytes which shall transfered over air (rx_addr + tx_addr + payload data)<br />
rx_addr = address of device, which shall receive the message (0x00 = broadcast to all devices)<br />
tx_addr = transmitter or my address. the receiver should know who has sent a message.<br />
payload = 1 to 60 bytes payload data.<br />

TX Bytes example:<br />
-> 0x06 0x03 0x01 0x00 0x01 0x02 0x03<br />

Basic configuration
===================

use **uint8_t CC1100::begin(volatile uint8_t &My_addr)** always as first configuration step. 

Device address
--------------
you should set a unique device address for the transmitter and a unique device address for the receiver. 
This can be done with **void CC1100::set_myaddr(uint8_t addr)**.

i.E. -> TX = 0x01 ; RX = 0x03


Modulation modes
----------------
the following modulation modes can be set by **void CC1100::set_mode(uint8_t mode)**. Transmitter and receiver must have the same Mode setting.

```
1 = GFSK_1_2_kb
2 = GFSK_38_4_kb
3 = GFSK_100_kb
4 = MSK_250_kb
5 = MSK_500_kb
6 = OOK_4_8_kb
```

ISM frequency band
------------------
you can set a frequency operation band by **void CC1100::set_ISM(uint8_t ism_freq)** to make it compatible with your hardware.

```
1 = 315
2 = 433
3 = 868
4 = 915
```

