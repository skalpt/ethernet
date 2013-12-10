// Libraries:
#include <OneWire.h>
#include <RF12.h>
#include <Ports.h>

// Pins:
const int PIN_STATUSGREEN = 6;          // Pin for status LED (green mode)
const int PIN_STATUSRED = 7;            // Pin for status LED (red mode)
const int PIN_ONEWIRE = 8;              // Pin for DS18B20

OneWire ds(PIN_ONEWIRE);

MilliTimer readTimer;
MilliTimer readDoneTimer;
byte readStatus = 1;
byte needToSend = 0;
byte addr[8];
int TReading;

class PacketBuffer : public Print
{
  public:
    PacketBuffer () : fill(0) {}
    const byte* buffer() { return buf; }
    byte length() { return fill; }
    void reset() { fill = 0; }
    virtual size_t write(uint8_t ch) { if (fill < sizeof buf) buf[fill++] = ch; }
  private:
    byte fill, buf[RF12_MAXDATA];
};

void setup()
{
  pinMode(PIN_STATUSGREEN, OUTPUT);     // Initialise the status LED (green mode)
  pinMode(PIN_STATUSRED, OUTPUT);       // Initialise the status LED (red mode)
  digitalWrite(PIN_STATUSRED, HIGH);

  Serial.begin(57600);
  Serial.println("Node started.");

  rf12_initialize(1, RF12_433MHZ);
}

void loop()
{
  if (rf12_recvDone() && rf12_crc == 0)
  {
  }

  if (needToSend && rf12_canSend())
  {
    PacketBuffer payload;
    payload.print(TReading);
    rf12_sendStart(0, payload.buffer(), payload.length());
    needToSend = 0;
    digitalWrite(PIN_STATUSRED, LOW);
    digitalWrite(PIN_STATUSGREEN, HIGH);
    Serial.println("RF packet sent");
  }

  if (readStatus == 0 && readTimer.poll(10000)) readStatus = 1;

  if (readStatus == 1)
  {
    digitalWrite(PIN_STATUSGREEN, LOW);
    digitalWrite(PIN_STATUSRED, HIGH);
    Serial.print("Reading temperature...");

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
    
    readDoneTimer.set(1000);
    readStatus = 2;
  }

  if (readStatus == 2 && readDoneTimer.poll())
  {
    ds.reset();
    ds.select(addr);
    ds.write(0xBE);
  
    byte data[9];
    for (int i = 0; i < 9; i++) data[i] = ds.read();
    if (ds.crc8(data, 8) != data[8]) return;
    ds.reset_search();
  
    TReading = (data[1] << 8) + data[0];
    Serial.println(TReading);

    needToSend = 1;
    readStatus = 0;
  }
}
