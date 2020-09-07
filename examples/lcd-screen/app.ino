#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h>

namespace pin
{
    const int lcd_rs = 12;
    const int lcd_rw = 11;
    const int lcd_en = 10;
    const int lcd_db4 = 5;
    const int lcd_db5 = 4;
    const int lcd_db6 = 3;
    const int lcd_db7 = 2;
} // namespace pin

hd44780_pinIO lcd(pin::lcd_rs, pin::lcd_rw, pin::lcd_en, pin::lcd_db4, pin::lcd_db5, pin::lcd_db6, pin::lcd_db7);

void setup()
{
    lcd.begin(16, 2);
    // Print a message to the LCD.
    lcd.print("Initializing...");
}

void loop()
{
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    lcd.print(millis() / 1000);
}
