#include <Wire.h> //wired
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#define TxD 10
#define RxD 9

SoftwareSerial BLE(TxD, RxD);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

String code = "";
int len = 0;
char ch;
char new_char;
String sentence = "";
String package = "";
//Comment 1
const int but = 2;
const int but2 = 3;
const int led = 13;
const int led2= 12;
int lenght = 0;
unsigned long pres_len = 0, rel_time, pres_time = 0, old_time_len = 0, old_pres = 0, space = 0, startPressed = 0, endPressed = 0;
int state = 0;
int unit_delay = 125;
int min_delay = 5;


void setup() {
  Serial.begin(9600);
  BLE.begin(9600);
  pinMode(but, INPUT_PULLUP);
  pinMode(but2, INPUT_PULLUP);
  pinMode(led, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
}

void but2State() {
    startPressed = millis();
    while (digitalRead(but2) == LOW) {}
    endPressed = millis();
    if ((endPressed - startPressed) > 2000) {
      sendPackage();
    }else{  
      delay(200);
      lenght = sentence.length();
      sentence.remove(lenght - 1);
      ClearDisplay();
      display.print(sentence);
      display.display();
      }   
  }

void sendPackage() {
   BLE.print(sentence);
   display.clearDisplay();
   display.setCursor(22, 0);
   display.print(sentence);
   display.display();
   delay(2000);     
   sentence = "";   
   display.clearDisplay();
   display.display();   
}

void receivePackage(){
    package = package + char(BLE.read());
    if(BLE.available()){
      receivePackage();
    }else{
    ClearDisplay();
    display.print(package);
    display.display();
    delay(2000);
    ClearDisplay();
    }
}


char MakeString() {
  if (pres_len < (unit_delay*3) && pres_len > 25)
  {
    return '.';                       
  }
  else if (pres_len > (unit_delay*3))
  {
    return '-';                     
  }
}

void loop() {
label:
  while (digitalRead(but) == HIGH) 
  {
    if(BLE.available()){
      receivePackage();
    }
    
    if (digitalRead(but2) == LOW) 
    {
      but2State();
    }
  }
  old_pres = rel_time;
  pres_time = millis();
  digitalWrite(led, HIGH);
  while (digitalRead(but) == LOW) {}
  rel_time = millis();
  digitalWrite(led, LOW);
  pres_len = rel_time - pres_time;
  space = pres_time - old_pres;
  if (pres_len > min_delay)
  {
    code += MakeString();
  }
  while ((millis() - rel_time) < (unit_delay * 3))
  {
    if (digitalRead(but) == LOW)
    {
      goto label;
    }
  }

  Morse_decod();
}
void ClearDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.display();
}
void Morse_decod()
{
  static String morse[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....",
                             "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-",
                             ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", "!"
                            };

int i = 0;
  while (morse[i] != "!")  
  {
    if (morse[i] == code)
    {
      ClearDisplay();
      sentence += (char('A' + i));
      display.print(sentence);
      display.display();
      break;
    }
    i++;
  }
  if (morse[i] == "!")
  {

  }

  code = "";

}