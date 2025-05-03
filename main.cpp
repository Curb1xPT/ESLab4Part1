#include "mbed.h"
#include "arm_book_lib.h"

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

AnalogIn potentiometer(A0);
AnalogIn lm35(A1); // 10 mV/\xB0 C
DigitalInOut sirenPin(D9);
DigitalIn mq2(A3);

bool quit = false;

char receivedChar = '\0';

float lm35Reading = 0.0; // Raw ADC input A0 value
float lm35TempC = 0.0;   // Temperature in Celsius degrees [\xB0 C]

void availableCommands();
void uartTask();
void pcSerialComStringWrite( const char* str );
char pcSerialComCharRead();

float analogReadingScaledWithTheLM35Formula(float analogReading );
float celsiusToFahrenheit(float tempInCelsiusDegrees );
float analogValueToFahrenheit(float analogValue );
float potentiometerScaledToCelsius(float analogValue );
float potentiometerScaledToFahrenheit(float analogValue );

int main()
{
    sirenPin.mode(OpenDrain);
    sirenPin.input();
    availableCommands();
    while( true ) {
        uartTask();
    }
}

void availableCommands()
{
    pcSerialComStringWrite( "Print the temperature until 'q' is pressed:\r\n" );

    pcSerialComStringWrite("Press 'c' to display the reading of the temperature measured ");
    pcSerialComStringWrite("by the LM35 expressed in \xB0 Celsius\r\n");

}

void uartTask()
{
    char receivedChar = '\0';
    char str[100] = "";
    receivedChar = pcSerialComCharRead();
    if( receivedChar !=  '\0') {
        switch (receivedChar) {

        case 'c':
        case 'C':
            while( !(receivedChar == 'q' || receivedChar == 'Q') ) {             
                lm35Reading = lm35.read();
                lm35TempC = analogReadingScaledWithTheLM35Formula(lm35Reading);
                str[0] = '\0';
                sprintf ( str, "LM35: %.2f \xB0 C\r\n", lm35TempC);
                pcSerialComStringWrite( str );
                delay( 200 );
                receivedChar = pcSerialComCharRead();
            }
            break;

        default:
            availableCommands();
            break;
        }
    }
}

float analogReadingScaledWithTheLM35Formula( float analogReading )
{
    return analogReading * 330.0;
}

float potentiometerScaledToCelsius( float analogValue )
{
    return 148.0 * analogValue + 2.0;
}

void pcSerialComStringWrite( const char* str )
{
    uartUsb.write( str, strlen(str) );
}

char pcSerialComCharRead()
{
    char receivedChar = '\0';
    if( uartUsb.readable() ) {
        uartUsb.read( &receivedChar, 1 );
    }
    return receivedChar;
}
