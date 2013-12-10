#include <EtherCard.h>

#define LED_PIN 2

#define DEBUG   1   // set to 1 to display free RAM on web page
#define SERIAL  1   // set to 1 to show incoming requests on serial port

// ethernet interface mac address - must be unique on your network
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

static BufferFiller bfill;  // used as cursor while filling the buffer

byte Ethernet::buffer[1000];   // tcp/ip send and receive buffer

#if DEBUG
	static int freeRam () {
	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
#endif

void setup(){
#if SERIAL
    Serial.begin(57600);
    Serial.println("\n[etherNode Test]");
#endif
    
    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
      Serial.println( "Failed to access Ethernet controller");
    if (!ether.dhcpSetup())
      Serial.println("DHCP failed");
#if SERIAL
    ether.printIp("IP: ", ether.myip);
#endif
}

char okHeader[] PROGMEM = 
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
;

static void homePage(BufferFiller& buf) {
    buf.emit_p(PSTR("$F\r\n"
        "<meta http-equiv='refresh' content='30'/>"
        "<title>etherNode Test</title>" 
        "<a href='s'>send packet</a>"), okHeader);
    long t = millis() / 1000;
    word h = t / 3600;
    byte m = (t / 60) % 60;
    byte s = t % 60;
    buf.emit_p(PSTR(
        "<br><br>"
        "Uptime is $D$D:$D$D:$D$D"), h/10, h%10, m/10, m%10, s/10, s%10);
#if DEBUG
    buf.emit_p(PSTR(" ($D bytes free)"), freeRam());
#endif
}

static int getIntArg(const char* data, const char* key, int value =-1) {
    char temp[10];
    if (ether.findKeyVal(data + 7, temp, sizeof temp, key) > 0)
        value = atoi(temp);
    return value;
}

static void sendPage(const char* data, BufferFiller& buf) {
    // pick up submitted data, if present
    byte d = getIntArg(data, "d");
    if (data[6] == '?' && 0 <= d && d <= 1) {
        if (d == 1)
        {
          digitalWrite(LED_PIN, HIGH);
        }
          if (d == 0)
        {
          digitalWrite(LED_PIN, LOW);
	}
#if SERIAL
        Serial.print("Data received: ");
        Serial.println(data);
#endif
        // redirect to home page
        buf.emit_p(PSTR(
            "HTTP/1.0 302 found\r\n"
            "Location: /\r\n"
            "\r\n"));
        return;
    }
    // else show a send form
    buf.emit_p(PSTR("$F\r\n"
        "<h3>Send a data packet</h3>"
        "<form>"
          "<p>"
            "Data: <input type=text name=d size=3><br>"
          "</p>"
          "<input type=submit value=Send>"
        "</form>"), okHeader);
}

void loop(){
    word len = ether.packetReceive();
    word pos = ether.packetLoop(len);
    // check if valid tcp data is received
    if (pos) {
        bfill = ether.tcpOffset();
        char* data = (char *) Ethernet::buffer + pos;
#if SERIAL
        Serial.println(data);
#endif
        // receive buf hasn't been clobbered by reply yet
        if (strncmp("GET / ", data, 6) == 0)
            homePage(bfill);
//      else if (strncmp("GET /c", data, 6) == 0)
//          configPage(data, bfill);
        else if (strncmp("GET /s", data, 6) == 0)
            sendPage(data, bfill);
        else
            bfill.emit_p(PSTR(
                "HTTP/1.0 401 Unauthorized\r\n"
                "Content-Type: text/html\r\n"
                "\r\n"
                "<h1>401 Unauthorized</h1>"));  
        ether.httpServerReply(bfill.position()); // send web page data
    }
}
