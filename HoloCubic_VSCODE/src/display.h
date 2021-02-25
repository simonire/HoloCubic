#ifndef DISPLAY_H
#define DISPLAY_H

#include <lvgl.h>
#include "network.h"
#include "Time.h"
#include "images.h"

#define LCD_BL_PIN 5
#define LCD_BL_PWM_CHANNEL 0


class Display
{
private:


public:
	void init(uint8_t location);
	void routine();
    void cleanCRT();
	void setBackLight(float);
    void ShowERWEIMA(void);
    void ShowBILIBILI(void);
    int PreWIFIConnect(String,String);
    int PreWebConnect(String WIFI_Name);
    void ShowFansNum(String biliuid);
    void ShowTime(bool FlagSerialShow);
    void SyncTime(void);
};

#endif