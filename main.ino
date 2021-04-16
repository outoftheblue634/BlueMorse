#include <Wire.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
SoftwareSerial BLE(10, 9);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

String code = "";
String sentence = "";
String holding_sentence = "";
String package = "";
const int but = 2;
const int but2 = 3;
const int led = 13;
const int led2= 12;
int lenght = 0;
unsigned long pres_len = 0, rel_time, pres_time = 0, old_time_len = 0, old_pres = 0, space = 0, startPressed = 0, endPressed = 0;
int unit_delay = 125;
int min_delay = 5;
int current = 0;
int row[] = { 8, 16, 24};
int char_counter = 0;
int cycle = 0;
int num_char = 5;

void setup() {
  Serial.begin(9600);
  BLE.begin(9600);
  pinMode(but, INPUT_PULLUP);
  pinMode(but2, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3D for 128x64
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
}

void but2State() {
    startPressed = millis();
    while (digitalRead(but2) == LOW) {}
    endPressed = millis();
    if ((endPressed - startPressed) > 1000) {
      sendPackage();
    }else{  
      delay(100);
      lenght = sentence.length();
      sentence.remove(lenght - 1);
      ClearDisplay();
      display.print(sentence);
      display.display();
      }   
  }
String shiftTextOver(){
  holding_sentence = sentence.substring(current);
  current += 1;
  return holding_sentence;
}

void sendPackage() {
   BLE.print(sentence);
   ClearDisplay();
   display.print("SENT");
   display.display();
   delay(2000);     
   sentence = "";  
   holding_sentence = ""; 
   current = 0;
   display.clearDisplay();
   display.display();   
}

void receivePackage(){
    char_counter += 1;
    package = package + char(BLE.read());
    if(BLE.available()){
      receivePackage();
    }
    cycle = round((char_counter/num_char) + 0.5); 
    for(int i = 0; i != cycle; i++){
      display.setCursor(0, row[i]);
      display.print(package.substring(i*num_char, num_char+(i*num_char)));
    }
    display.display();
    delay(2000);
    cycle = 0;
    package = "";
    char_counter = 0;
    ClearDisplay();
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
                             ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", "....."
                            };

int i = 0;
  while (morse[i] != "!")  
  {
    if (morse[i] == code)
    {
      if (i == 26) {i = -33;} //Spaces = ASCII 32
      if (sentence.length() > 17){
        ClearDisplay();
        sentence += (char('A' + i));
        display.print(shiftTextOver());
        display.display();
        break;
      }
      else{
      ClearDisplay();
      sentence += (char('A' + i));
      display.print(sentence);
      display.display();
      break;
      }
    }
    i++;
  }
  code = "";
}