#include <OneWire.h>
#include <DallasTemperature.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h>

#define TEMPERATURE_PRECISION 9 // Lower resolution

namespace pin
{
    const byte ph_sensor = A0;
    const byte tds_sensor = A1;
    const int one_wire_bus = 6;
    const int lcd_rs = 12;
    const int lcd_rw = 11;
    const int lcd_en = 10;
    const int lcd_db4 = 5;
    const int lcd_db5 = 4;
    const int lcd_db6 = 3;
    const int lcd_db7 = 2;
} // namespace pin

namespace device
{
    float aref = 4.3;
}

namespace sensor
{
    float ec = 0;
    unsigned int tds = 0;
    float waterTemp = 0;
    float ecCalibration = 1;
} // namespace sensor

hd44780_pinIO lcd(pin::lcd_rs, pin::lcd_rw, pin::lcd_en, pin::lcd_db4, pin::lcd_db5, pin::lcd_db6, pin::lcd_db7);

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(pin::one_wire_bus);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

DeviceAddress waterTemperatureDeviceAddress;

unsigned long int avgValue; //Store the average value of the sensor feedback
float b;
int buf[10], temp;

// function to print a device address
void printDeviceAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void setupTemperatureDevices()
{
    // Start up the library
    sensors.begin();

    // report parasite power requirements
    Serial.print("Parasite power is: ");
    if (sensors.isParasitePowerMode())
    {
        Serial.println("ON");
    }
    else
    {
        Serial.println("OFF");
    }

    // Search the wire for address
    if (sensors.getAddress(waterTemperatureDeviceAddress, 0))
    {
        Serial.print("Found device ");
        Serial.print(0, DEC);
        Serial.print(" with address: ");
        printDeviceAddress(waterTemperatureDeviceAddress);
        Serial.println();

        Serial.print("Setting resolution to ");
        Serial.println(TEMPERATURE_PRECISION, DEC);

        // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
        sensors.setResolution(waterTemperatureDeviceAddress, TEMPERATURE_PRECISION);

        Serial.print("Resolution actually set to: ");
        Serial.print(sensors.getResolution(waterTemperatureDeviceAddress), DEC);
        Serial.println();
    }
    else
    {
        Serial.print("Found ghost device at ");
        Serial.print(0, DEC);
        Serial.print(" but could not detect address. Check power and cabling");
    }
}

void checkWaterTemperature()
{
    sensors.requestTemperatures();
    sensor::waterTemp = sensors.getTempC(waterTemperatureDeviceAddress);
}

void printTemperature(float tempC)
{
    Serial.print("Temp C: ");
    Serial.print(tempC);
    Serial.print(" Temp F: ");
    Serial.print(DallasTemperature::toFahrenheit(tempC), 2);
}

void checkTDS()
{

    float rawEc = analogRead(pin::tds_sensor) * device::aref / 1024.0;                                          // read the analog value more stable by the median filtering algorithm, and convert to voltage value
    float temperatureCoefficient = 1.0 + 0.02 * (sensor::waterTemp - 25.0);                                     // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    sensor::ec = (rawEc / temperatureCoefficient) * sensor::ecCalibration;                                      // temperature and calibration compensation
    sensor::tds = (133.42 * pow(sensor::ec, 3) - 255.86 * sensor::ec * sensor::ec + 857.39 * sensor::ec) * 0.5; //convert voltage value to tds value
    Serial.print("TDS:");
    Serial.println(sensor::tds);
    Serial.print("EC:");
    Serial.println(sensor::ec, 2);

    lcd.setCursor(0, 0);
    lcd.print("TDS: ");
    lcd.print(sensor::tds);
    lcd.print(" EC:");
    lcd.print(sensor::ec, 2);
    lcd.setCursor(7, 1);
    lcd.print(" Temp:");
    lcd.print(DallasTemperature::toFahrenheit(sensor::waterTemp), 0);
}

void setup()
{
    //pinMode(13, OUTPUT);
    Serial.begin(115200);
    Serial.println("Ready"); //Test the serial monitor
        // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    // Print a message to the LCD.
    lcd.print("Ready");
    setupTemperatureDevices();
}

void loop()
{
    for (int i = 0; i < 10; i++) //Get 10 sample value from the sensor for smooth the value
    {
        buf[i] = analogRead(pin::ph_sensor);
        delay(10);
    }
    for (int i = 0; i < 9; i++) //sort the analog from small to large
    {
        for (int j = i + 1; j < 10; j++)
        {
            if (buf[i] > buf[j])
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    avgValue = 0;
    for (int i = 2; i < 8; i++) //take the average value of 6 center sample
        avgValue += buf[i];
    float phValue = (float)avgValue * 5.0 / 1024 / 6; //convert the analog into millivolt
    phValue = 3.5 * phValue;                          //convert the millivolt into pH value
    Serial.print("    pH:");
    Serial.print(phValue, 2);
    Serial.println(" ");
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    lcd.print("pH:");
    lcd.print(phValue, 2);
    // checkTemperaure();
    checkTDS();
    //digitalWrite(13, HIGH);
    delay(800);
    //digitalWrite(13, LOW);
}
