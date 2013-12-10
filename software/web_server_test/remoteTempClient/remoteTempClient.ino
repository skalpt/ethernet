// Demo using DHCP and DNS to perform a web client request.
// 2011-06-08 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <OneWire.h>
#include <RF12.h>
#include <Ports.h>
#include <EtherCard.h>

OneWire ds(3);

MilliTimer readTimer;
MilliTimer readDoneTimer;
byte readStatus = 1;
byte needToSend = 0;
byte addr[8];
char DReading[10];

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[700];
static uint32_t timer;

char website[] PROGMEM = "thenewdabbs.com";

// called when the client request is complete
static void my_callback (byte status, word off, word len) {
  Serial.println(">>>");
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
}

void setup () {
  Serial.begin(57600);
  Serial.println("\n[webClient]");

  Serial.print("Initialising RF12...");
  rf12_initialize(1, RF12_433MHZ);
  Serial.println("done.");

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");
  if (!ether.dhcpSetup())
    Serial.println("DHCP failed");

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  

// do we need this?
  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");

//and this?    
  ether.printIp("SRV: ", ether.hisip);
}

void loop () {
  byte i;
  
  ether.packetLoop(ether.packetReceive());

  if (millis() > timer) {
    timer = millis() + 5000;

    byte addr[8];
    if (!ds.search(addr))
    {
      ds.reset_search();
      return;
    }
    if (ds.crc8(addr, 7) != addr[7]) return;
    if (addr[0] != 0x28) return;
    
    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1);

   delay(1000);

    ds.reset();
    ds.select(addr);
    ds.write(0xBE);
  
    byte data[9];
    for (int i = 0; i < 9; i++) data[i] = ds.read();
    if (ds.crc8(data, 8) != data[8]) return;
    
    ds.reset_search();
    
    int TReading = (data[1] << 8) + data[0];
    sprintf(DReading, "data=%d", TReading);

    Serial.println();
    Serial.print("<<< REQ /arduinoconnectiontest/?");
    Serial.print(DReading);
    Serial.print(" ");
    ether.browseUrl(PSTR("/arduinoconnectiontest/?"), DReading, website, my_callback);
  }
}
