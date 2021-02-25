#include "display.h"
#include "imu.h"
#include "ambient.h"
#include "network.h"
//#include "sd_card.h"
#include "rgb_led.h"

/*** Function statement ***/

/*** Component objects ***/
Display screen;
IMU mpu;
Pixel rgb;
Ambient ambLight;
//SdCard tf;

/*** 修改的变量，仅使用2.4G WiFi 和 WiFi配网***/
String wife_ssid = "95-201";            // line-1 for WiFi ssid
String wife_pass = "13815203225";       // line-2 for WiFi password
String BILIBILI_UID = "20259914";       //bilibili UID

/*** Function  ***/
long time1;    
void setup()
{
	Serial.begin(115200);

	/*** Init screen ***/
	screen.init(4);
	screen.setBackLight(1);
    
    rgb.init(); 

    #if 0
        /*** Init IMU as input device ***/
        // lv_port_indev_init();
        // mpu.init();
    
        /*** Init on-board RGB ***/
        // rgb.init();
        // rgb.setBrightness(0.5).setRGB(255, 0, 255);

        /*** Init ambient-light sensor ***/
        ambLight.init(ONE_TIME_H_RESOLUTION_MODE);
        rgb.setBrightness(ambLight.getLux() / 500.0);
        //Serial.println(ambLight.getLux());

        /*** Init micro SD-Card ***/
        // tf.init();
        // String ssid = tf.readFileLine("/wifi.txt", 1);		// line-1 for WiFi ssid
        // String password = tf.readFileLine("/wifi.txt", 2);	// line-2 for WiFi password
        
        /*** Read WiFi info in SD-Card, then scan & connect WiFi ***/

    #endif

/***  以下两句只能二选一  ***/
/***    使用WIFI默认配网取消注释此句    ***/
    //while(  screen.PreWIFIConnect(wife_ssid, wife_pass)   );
    screen.ShowERWEIMA();
/***  使用微信小程序自动配网取消注释此句  ***/
	while(  screen.PreWebConnect("WIFI-WEB")   );
    
    
    screen.cleanCRT();
    screen.ShowBILIBILI();

    screen.ShowTime(0);//此处还有bug
    screen.ShowFansNum(BILIBILI_UID);
    time1 = millis();
    
}

int time_sec = 0,time_min = 0;;
void loop()
{
	if (millis() - time1 > 1000)
	{
		time1 = millis();
        time_sec += 1;
        if(time_sec == 30)
        {
            time_sec = 0;       time_min +=1;
        }    
            
        if (timeStatus() != timeNotSet) 
            screen.ShowTime(0);
        
        if(time_min == 2)
        {
            screen.SyncTime();
        }
        
        if (time_min == 2 * 2) 
        {
            time_min = 0;
            rgb.setBrightness(0.5).setRGB(0, 0, 255);
            screen.ShowFansNum(BILIBILI_UID);
            
        }	
    }
}