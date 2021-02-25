#include "display.h"
#include <TFT_eSPI.h>

/*
TFT pins should be set in path/to/Arduino/libraries/TFT_eSPI/User_Setups/Setup24_ST7789.h
*/
TFT_eSPI tft = TFT_eSPI();
Network wifi;
WiFiUDP Udp;
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];


time_t getNtpTime();
void sendNTPpacket(IPAddress& address);
bool AutoConfig(void);
void SmartConfig(void);


void my_print(lv_log_level_t level, const char* file, uint32_t line, const char* fun, const char* dsc)
{
	Serial.printf("%s@%d %s->%s\r\n", file, line, fun, dsc);
	Serial.flush();
}


void my_disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p)
{
	uint32_t w = (area->x2 - area->x1 + 1);
	uint32_t h = (area->y2 - area->y1 + 1);

	tft.startWrite();
	tft.setAddrWindow(area->x1, area->y1, w, h);
	tft.pushColors(&color_p->full, w * h, true);
	tft.endWrite();

	lv_disp_flush_ready(disp);
}


void Display::init(uint8_t location)
{
	ledcSetup(LCD_BL_PWM_CHANNEL, 5000, 8);
	ledcAttachPin(LCD_BL_PIN, LCD_BL_PWM_CHANNEL);

    tft.init();
    tft.setRotation(location); /* mirror */
    tft.fillScreen(TFT_BLACK);

#if 0
	lv_init();

	lv_log_register_print_cb(my_print); /* register print function for debugging */

	tft.begin(); /* TFT init */
	tft.setRotation(0); /* mirror */

	lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

	/*Initialize the display*/
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.hor_res = 240;
	disp_drv.ver_res = 240;
	disp_drv.flush_cb = my_disp_flush;
	disp_drv.buffer = &disp_buf;
	lv_disp_drv_register(&disp_drv);
#endif
}

void Display::routine()
{
	lv_task_handler();
}

void Display::cleanCRT()
{
    tft.fillScreen(TFT_BLACK);
}

void Display::setBackLight(float duty)
{
	duty = constrain(duty, 0, 1);
	duty = 1 - duty;
	ledcWrite(LCD_BL_PWM_CHANNEL, (int)(duty * 255));
}

void Display::ShowERWEIMA(void)
{
    tft.fillScreen(TFT_BLACK);
    tft.setSwapBytes(true);
    tft.pushImage(0,0,240,239,PEI);

}


void Display::ShowBILIBILI()
{
    tft.fillScreen(TFT_BLACK);
    tft.setSwapBytes(true);
    tft.pushImage(20,170,70,25,bilibili);
}


//wifi连接
int Display::PreWIFIConnect(String ssid, String pass)
{
    String str;
    
    tft.setTextColor(TFT_WHITE);
    str = "Waiting for WiFi";   tft.drawString(str,10, 10, 2);
    str = "connection...";      tft.drawString(str,10, 30, 2);

	
    wifi.init(ssid.c_str(), pass.c_str());
    uint8_t t=0;
    while (WiFi.status() != WL_CONNECTED)
	{
		
        delay(500);
		Serial.print(".");
        
        if(t == 75)
        {
            tft.setTextColor(TFT_RED);
            str = "WiFi connected ERR ";
            tft.drawString(str,0,50,4);
            //tft.setCursor(10, 50, 4);tft.println(str); 
            
        }

        if(t == 150)
        {
            str = "Reset in 5s later.. ";
            tft.drawString(str,0,70,4);
            
            t=0;

            ESP.restart();
        }

        t++; 
	}

    tft.setTextColor(TFT_GREEN);
    str = "WiFi connected OK  ";
    tft.drawString(str,0,50,4);

    tft.setTextColor(TFT_WHITE);
	str = "IP address: ";
    tft.drawString(str,10, 70, 2);
    str = WiFi.localIP().toString().c_str();
    tft.drawString(str,120, 70, 2);
    
    str = "MAC address: ";
    tft.drawString(str,10, 90, 2);
    str = WiFi.macAddress().c_str();
    tft.drawString(str,120, 90, 2);
    
    setSyncProvider(getNtpTime);
    setSyncInterval(300); //每300秒同步一次时间

    return 0;

}
//web连接
int Display::PreWebConnect(String WIFI_Name)
{ 
    if (!AutoConfig())
    {
        SmartConfig();
        esp_restart();
    }  
    return 0;

}

bool AutoConfig(void)
{
    WiFi.begin();
    //如果觉得时间太长可改
    for (int i = 0; i < 20; i++)
    {
        int wstatus = WiFi.status();
        if (wstatus == WL_CONNECTED)
        {
	        Serial.println("WIFI SmartConfig Success");
	        Serial.printf("SSID:%s", WiFi.SSID().c_str());
	        Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
	        Serial.print("LocalIP:");
	        Serial.print(WiFi.localIP());
	        Serial.print(" ,GateIP:");
	        Serial.println(WiFi.gatewayIP());
	        return true;
        }
        else
        {
	        Serial.print("WIFI AutoConfig Waiting......");
	        Serial.println(wstatus);
	        delay(1000);
        }
    }
    Serial.println("WIFI AutoConfig Faild!" );
    return false;
}

void SmartConfig(void)
{
  WiFi.mode(WIFI_STA);
  Serial.println("\r\nWait for Smartconfig...");
  WiFi.beginSmartConfig();
  while (1)
  {
    Serial.print(".");
    delay(500);                   // wait for a second
    if (WiFi.smartConfigDone())
    {
    Serial.println("SmartConfig Success");
    Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
    Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      break;
    }
  }
}



//显示粉丝数量
void Display::ShowFansNum(String biliuid)
{
    String str;
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    str = "Fans";tft.drawCentreString(str,170,170,4);   


    #if 1
    tft.setTextColor(TFT_BLUE,TFT_BLACK);
    str = " RXD... ";
    tft.drawCentreString(str,170,210,4); 

    str = wifi.getBilibiliFans("http://api.bilibili.com/x/relation/stat?vmid=" + biliuid);
    
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    tft.drawCentreString(str,170,210,4); 
        //tft.println(str);
    #endif
}

/*** Vaule statement ***/
boolean isNTPConnected = false;
static const char ntpServerName[] = "ntp1.aliyun.com"; //NTP服务器，阿里云
const int timeZone = 8;                                //时区，北京时间为+8
const int NTP_PACKET_SIZE = 48;     // NTP时间在消息的前48个字节里
byte packetBuffer[NTP_PACKET_SIZE]; // 输入输出包的缓冲区
//显示NTP时间
void Display::ShowTime(bool FlagSerialShow)
{
    int years, months, days, hours, minutes, seconds, weekdays;
    String str;

    if (isNTPConnected)
    {
        // Serial.println("(UTC+8)");
        tft.setTextColor(TFT_GREEN,TFT_BLACK);
        str = "   UTC+8 ";
        tft.drawRightString(str,220, 20, 4);
    }   
    else
    {
        //Serial.println("无网络!"); //如果上次对时失败，则会显示无网络
        tft.setTextColor(TFT_RED,TFT_BLACK);
        str = " Sync X";
        tft.drawRightString(str,220, 20, 4);
    }
        
    years = year();
    months = month();
    days = day();
    hours = hour();
    minutes = minute();
    seconds = second();
    weekdays = weekday();
    
    if (FlagSerialShow)
        Serial.printf("%d/%d/%d %d:%d:%d\n", years, months, days, hours, minutes, seconds, weekdays);


    
    char currentTime[15];
    char currentDay[15];

     sprintf((char *)currentTime,"%02d:%02d:%02d",hours,minutes,seconds);
    //sprintf((char *)currentTime,"%02d:%02d",hours,minutes);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    tft.drawCentreString(currentTime,120, 100, 7);


    sprintf((char *)currentDay,"%d/%02d/%02d",years,months,days);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    tft.drawString(currentDay,20, 20, 2);
    //Serial.println(currentTime);

}

void Display::SyncTime()
{
    setSyncProvider(getNtpTime);
    setSyncInterval(300); //每300秒同步一次时间
}
time_t getNtpTime()
{
  IPAddress ntpServerIP;          // NTP服务器的地址

  while (Udp.parsePacket() > 0);  // 丢弃以前接收的任何数据包
  Serial.println("Transmit NTP Request");
  // 从池中获取随机服务器
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500)
  {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE)
    {
      Serial.println("Receive NTP Response");
      isNTPConnected = true;
      Udp.read(packetBuffer, NTP_PACKET_SIZE); // 将数据包读取到缓冲区
      unsigned long secsSince1900;
      // 将从位置40开始的四个字节转换为长整型，只取前32位整数部分
      secsSince1900 = (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      Serial.println(secsSince1900);
      Serial.println(secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR);
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-("); //无NTP响应
  isNTPConnected = false;
  return 0; //如果未得到时间则返回0
}

// 向给定地址的时间服务器发送NTP请求
void sendNTPpacket(IPAddress& address)
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0;          // Stratum, or type of clock
  packetBuffer[2] = 6;          // Polling Interval
  packetBuffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  Udp.beginPacket(address, 123); //NTP需要使用的UDP端口号为123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
