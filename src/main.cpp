#define BLINKER_WIFI
#include <Blinker.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#define auth ""
#define ssid "TP-LINK_5AFF"
#define pswd ""
#define buttonName "btn"

int getPower = 16;
int power = 4;
int powerLight = 0;

bool powerLightFlag = true;
bool flag = true;

ESP8266WiFiMulti wifiMulti;
BlinkerButton Button1(buttonName);
Ticker changeLed;

void normalShutdown()
{
    if (digitalRead(power))
    {
        digitalWrite(power, LOW);
        delay(500);
        digitalWrite(power, HIGH);
    }
    else
    {
        normalShutdown();
    }
}

void forceShutdown()
{
    if (digitalRead(power))
    {
        digitalWrite(power, LOW);
        delay(5000);
        digitalWrite(power, HIGH);
    }
    else
    {
        forceShutdown();
    }
}
void changeButton()
{
    if (digitalRead(power))
    {
        Button1.print("on");
    }
    else
    {
        Button1.print("off");
    }
}

void changeTheLEDWhileComupterWakeup()
{
    if (powerLightFlag)
    {
        analogWrite(LED_BUILTIN, powerLight);
        powerLight += 8;
    }
    else
    {
        analogWrite(LED_BUILTIN, powerLight);
        powerLight -= 8;
    }

    if ((powerLight > 1024) & powerLightFlag)
    {
        powerLightFlag = false;
    }
    else if ((powerLight < 0) & !powerLightFlag)
    {
        powerLightFlag = true;
    }
}

void button1_callback(const String &state)
{
    BLINKER_LOG("get button state: ", state);
    changeButton();
    normalShutdown();
}

void dataRead(const String &data)
{
    BLINKER_LOG("Blinker readString: ", data);
}
void setup()
{
    pinMode(power, OUTPUT);
    digitalWrite(power, HIGH);
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);
    // 初始化有LED的IO
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(getPower, INPUT_PULLUP);

    WiFi.setSleepMode(WIFI_LIGHT_SLEEP);

    Serial.print("\n connecting to the wifi ... \n ");
    wifiMulti.addAP(ssid, pswd);
    int i = 0;
    while (wifiMulti.run() != WL_CONNECTED)
    {
        delay(500);
        Serial.print('.');
    }
    Serial.print("\n");

    // 初始化blinker
    Blinker.begin(auth, ssid, pswd);
    Blinker.attachData(dataRead);
    Button1.attach(button1_callback);
}

void loop()
{
    //根据VBUS的状态确定是否进入低功耗模式

    if (digitalRead(getPower))
    {
        Blinker.run();
        if (!changeLed.active())
        {
            changeLed.attach(0.01, changeTheLEDWhileComupterWakeup);
        }
    }
    else
    {
        Blinker.run();
        if (changeLed.active())
        {
            changeLed.detach();
        }
        if (!digitalRead(LED_BUILTIN))
        {
            digitalWrite(LED_BUILTIN, HIGH);
        }
        delay(5000);
    }
}