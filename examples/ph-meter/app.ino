#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h>

namespace pin
{
    const byte ph_sensor = A0;
    const int lcd_rs = 2;
    const int lcd_rw = 3;
    const int lcd_en = 4;
    const int lcd_db4 = 9;
    const int lcd_db5 = 10;
    const int lcd_db6 = 11;
    const int lcd_db7 = 12;
} // namespace pin

namespace sensor
{
    float ph = 0;
} // namespace sensor

hd44780_pinIO lcd(pin::lcd_rs, pin::lcd_rw, pin::lcd_en, pin::lcd_db4, pin::lcd_db5, pin::lcd_db6, pin::lcd_db7);


void printPh()
{
    Serial.print("    pH:");
    Serial.print(sensor::ph, 2);
    Serial.println(" ");
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number Ph:
    lcd.print("pH:");
    lcd.print(sensor::ph, 2);
}

void checkPh()
{
    unsigned long int avgValue; //Store the average value of the sensor feedback
    float b;
    int buf[10], temp;
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
    sensor::ph = 3.5 * phValue;                       //convert the millivolt into pH value
    printPh();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Initializing..."); //Test the serial monitor

    lcd.begin(16, 2);
    lcd.print("Initializing..."); // Print a message to the LCD.
}

void loop()
{
    // set the cursor to column 10, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(10, 1);
    // print the number of seconds since reset:
    lcd.print(millis() / 1000);

    checkPh();
    delay(1000); // run every second
}
