
#define TEMPERATURE_PRECISION 9 // Lower resolution

namespace timing
{
    // 1 second
    const long oneSecond = 1000;
    const long thirtySeconds = oneSecond * 30;
    // one minute
    const long oneMinute = oneSecond * 60;
    // 60 minutes
    const long oneHour = oneMinute * 60;
    // six hours
    const long sixHours = oneHour * 6;
    // 8 hours
    const long eightHours = oneHour * 8;
    // 16 hours
    const long sixteenHours = oneHour * 16;

    // pump start millis
    unsigned long pumpMillis = 0;

    // time since last change (light on/off)
    unsigned long lightMillis = 0;

    // Last run
    unsigned long lastRun = 0;

} // namespace timing

namespace pin
{
    const byte ph_sensor = A0;
    const byte tds_sensor = A1;
    const int light = 13;
    const int light_button = 9;
    const int ph_button = 8;
    const int pump_button = 7;
    const int one_wire_bus = 6;
    const int lcd_db4 = 5;
    const int lcd_db5 = 4;
    const int lcd_db6 = 3;
    const int lcd_db7 = 2;
    const int pump = 12;
} // namespace pin

namespace device
{
    float aref = 4.3;
}

namespace sensor
{
    float previosEc = 0;
    float ec = 0;
    unsigned int previosTds = 0;
    unsigned int tds = 0;
    float previosWaterTemp = 0;
    float waterTemp = 0;
    float ecCalibration = 1;
    float ph = 0;
    bool lightStatus = false;
    bool pumpStatus = false;
} // namespace sensor

void setupLight()
{
    pinMode(pin::light, OUTPUT);
    // Always try to avoid duplicate code.
    // Instead of writing digitalWrite(pin, LOW) here,
    // call the function off() which already does that
    lightOff();
}

void lightOn()
{
    digitalWrite(pin::light, HIGH);
    sensor::lightStatus = true;
    Serial.println("Light has been turned on");
}

void lightOff()
{
    digitalWrite(pin::light, LOW);
    sensor::lightStatus = false;
    Serial.println("Light has been turned off");
}

void checkLights(unsigned long currentMillis)
{
    // check to see if the light is on
    if (sensor::lightStatus)
    {
        if (currentMillis - timing::lightMillis >= timing::sixteenHours)
        {
            lightOff();
            timing::lightMillis = currentMillis;
        }
    }
    else if (!sensor::lightStatus)
    {
        if (currentMillis - timing::lightMillis >= timing::eightHours)
        {
            lightOn();
            timing::lightMillis = currentMillis;
        }
    }
}

void setupPump()
{
    pinMode(pin::pump, OUTPUT);
    // Always try to avoid duplicate code.
    // Instead of writing digitalWrite(pin, LOW) here,
    // call the function off() which already does that
    pumpOff();
}

void pumpOn()
{
    digitalWrite(pin::pump, HIGH);
    sensor::pumpStatus = true;
    Serial.println("Pump has been turned on");
}

void pumpOff()
{
    digitalWrite(pin::pump, LOW);
    sensor::pumpStatus = false;
    Serial.println("Pump has been turned off");
}

void checkPump(unsigned long currentMillis)
{
    // Pump is on
    if (sensor::pumpStatus)
    {
        // make sure the pump only runs for 4.5 minutes
        if (currentMillis - timing::pumpMillis >= timing::thirtySeconds)
        {
            pumpOff();
            timing::pumpMillis = currentMillis;
        }
    }
    // pump is off
    else if (!sensor::pumpStatus)
    {
        if (currentMillis - timing::pumpMillis >= timing::sixHours)
        {
            pumpOn();
            timing::pumpMillis = currentMillis;
        }
    }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        ;
    }
    Serial.println("Initializing..."); //Test the serial monitor
    setupLight();
    lightOn();
    setupPump();
}

void loop()
{
    unsigned long currentMillis = millis();
    checkLights(currentMillis);
    checkPump(currentMillis);
    delay(1000);
}
