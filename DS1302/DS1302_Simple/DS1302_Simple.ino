
// CONNECTIONS:
// DS1302 I2C_SDA - DAT --> 21
// DS1302 I2C_SCL - CLK --> 22
// DS1302 RST/CE --> 0
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND


#include <RtcDS1302.h>

#define countof(a) (sizeof(a) / sizeof(a[0]))

ThreeWire myWire(21,22,0); // SDA - DAT, SCL - CLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

void setup () 
{
    Serial.begin(115200);
    Rtc.Begin();

    Serial.print("compilado: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        Serial.println("Fecha y hora invalida del sensor");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("El sensor tiene la escritura protejida, habilitado escritura...");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC no esta activado, iniciando...");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();

    if (now < compiled) 
    {
        Serial.println("Actualizando fecha del sensor...");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("Fecha guardada del sensor");
    }
    else if (now == compiled) 
    {
        Serial.println("Fecha actualizada");
    }
}

void loop () 
{
    RtcDateTime now = Rtc.GetDateTime();

    printDateTime(now);
    Serial.println();

    if (!now.IsValid())
    {
        Serial.println("No conectado...");
    }

    delay(1000); // ten seconds
}


void printDateTime(const RtcDateTime& dt)
{
    char datestring[26];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Day(),
            dt.Month(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}

