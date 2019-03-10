#include <EEPROM.h>

#define tempCycle 1000 U //Multi threading...
unsigned long tempLastMillis = 0;



int mode;

int ledPinBlue  = 6; // LED connected to digital pin 9
int ledPinRed   = 3;
int ledPinGreen = 5;

//
int r = 255, g = 0, b = 0;
int red, green, blue ;
//

//Kontrol Yapısı Değişkenleri
static boolean recvInProgress  = false;
static byte ndx                = 0;
char startMarker               = '<';
char faded                     = 'Q';
char stat                      = '@';
char kapat                     = 'K';
char flashed                   = 'F';
char endMarker                 = '>';
String rc;
//Kontrol yapısı Değişkenleri

//Flash LED Varible
int flashRed   = 3 ;
int flashGreen = 5 ;
int flashBlue  = 6 ;
//
boolean newData = false;

void setup()
{
    Serial.begin(9600);

    pinMode(3, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(13, OUTPUT);


    digitalWrite(13,LOW);

    analogWrite(ledPinRed, 255); //kırmızı
    delay(1000);
    analogWrite(ledPinGreen, 255); //yeşil
    delay(1000);
    analogWrite(ledPinBlue, 255); //mavi
    delay(1000);

    red      = EEPROM.read(0);
    green    = EEPROM.read(1);
    blue     = EEPROM.read(2);
    mode     = EEPROM.read(3);
    //speed  = EEPROM.read(4);
    
}

void loop()
{
    //Mode Kontrol

    recvWithStartEndMarkers();

    if (mode == 1)  //Fade
    {
        fade();


        analogWrite(ledPinRed   , r);
        analogWrite(ledPinGreen , g);
        analogWrite(ledPinBlue  , b);
    }
    else if (mode == 2)//Static
    {
        analogWrite(ledPinRed   , red);
        analogWrite(ledPinGreen , green);
        analogWrite(ledPinBlue  , blue);
    }
    else if (mode == 3)//Flash
    {
        flash();
    }
    else if (mode == 555)//KAPAT
    {
        analogWrite(ledPinRed   , 0);
        analogWrite(ledPinGreen , 0);
        analogWrite(ledPinBlue  , 0);
    }
  
}

void recvWithStartEndMarkers()
{

    while (Serial.available() > 0)
    {
        rc = Serial.readString();
        Serial.print("new data:");
        Serial.println(rc);
        
        
        if(rc.indexOf(startMarker) != -1){
            //bizimki geldi yine;

            if(rc.indexOf(faded) != -1){ // Fade
                mode = 1;
                EEPROM.update(3,mode);
            }else if(rc.indexOf(stat) != -1){ //R G B
                rc.remove(0,2);
               
               
                rc.remove(rc.length()-2);
                
                red   = getValue(rc,',',0).toInt();
                green = getValue(rc,',',1).toInt();
                blue  = getValue(rc,',',2).toInt();
                
                mode  = 2 ;
               
                EEPROM.update(0,red);
                EEPROM.update(1,green);
                EEPROM.update(2,blue);
                EEPROM.update(3,mode);
                
                showParsedData();

            } else if(rc.indexOf(kapat) != -1){ //KAPAT
                mode = 555;
                EEPROM.update(3,mode);
            }
            else if(rc.indexOf(flashed) != -1){
                mode = 3;
                EEPROM.update(3,mode);
            }
        }

      
    }
    
}

void fade()
{
    if (r > 0 && b == 0)
    {
        r--;
        g++;
    }
    if (g > 0 && r == 0)
    {
        g--;
        b++;
    }
    if (b > 0 && g == 0)
    {
        r++;
        b--;
    }

    delay(30);
}
void flash(){
    digitalWrite(flashRed,255);
    delay(1000);
    digitalWrite(flashRed,0);
    digitalWrite(flashBlue,255);
    delay(1000);
    digitalWrite(flashBlue,0);
    digitalWrite(flashGreen,255);
    delay(1000);
    digitalWrite(flashGreen,0);
}



void showParsedData()
{

    Serial.print("R ");
    Serial.println(red);
    Serial.print("G ");
    Serial.println(green);
    Serial.print("B ");
    Serial.println(blue);

    Serial.print("EEPROM 1(R): ");
    Serial.println(EEPROM.read(0));
    Serial.print("EEPROM 2(G): ");
    Serial.println(EEPROM.read(1));
    Serial.print("EEPROM 3(B): ");
    Serial.println(EEPROM.read(2));
    Serial.print("EEPROM 4(Mode): ");
    Serial.println(EEPROM.read(3));
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}