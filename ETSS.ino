#include <LiquidCrystal.h>
#include <Wire.h>
#include <Servo.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Buzzer.h>

#define codeLength 5

#define SS_PIN 53
#define RST_PIN 48

//rfid
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

//buzzer
const int BPin = 45;
//pushbutton
int BUT = 32;
int BV = 0;

//servo
Servo myservo;

//PIR
int pirPIN = 22;

//define LCD pins
//LiquidCrystal lcd(RS, E, DB4, DB5, DB6, DB7);
LiquidCrystal lcd(36, 37, 38, 39, 40, 41);
  
//Numpad:
char Code[codeLength];
char PW[codeLength]="98C4";
byte keycount=0;
const byte ROWS = 4;
const byte COLS = 4;

char hexakeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {11, 10, 9, 8};
byte colPins[COLS] = {7, 6, 5, 4};
Keypad customKeypad = Keypad(makeKeymap(hexakeys), rowPins, colPins, ROWS, COLS);

//

void setup() {
  Serial.begin(9600);
  myservo.attach(29);
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  
  pinMode (pirPIN, INPUT); //PIR
  pinMode(BUT,INPUT); //Pushbutton
  pinMode(BPin, OUTPUT); //Buzzer
  digitalWrite(BPin, LOW);

  SPI.begin(); // Initiate  SPI bus
  mfrc522.PCD_Init(); // Initialize MFRC522

  Serial.println("Enter your Passcode and Scan your Employee Tag please!");
  lcd.print("Enter Passcode &");
  lcd.setCursor(0,1);
  lcd.print("Scan EmployeeTag");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
}

void loop() {
  //If PIR is triggered:
  int pirState = digitalRead(pirPIN);
  BV = digitalRead(32);
  if (pirState==HIGH){
    lcd.clear();
    Serial.println("Intruder Alert");
    lcd.print("Intruder Alert");
    
    do {
      tone(45,200,850);    
      delay(500);
      tone(45,0,850);
      delay(500);
      BV = digitalRead(32);
    }
    while (BV==0);
    
    lcd.clear();
  }

  //Adding "*" when a key is pressed:
  char customKey = customKeypad.getKey();
  if(customKey!=NO_KEY){
    Code[keycount]=customKey;
    Serial.print(Code[keycount]);
    keycount++;
    lcd.print("*");}
  
  if ( ! mfrc522.PICC_IsNewCardPresent()){
    return;}
  
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()){
    return;}

  //Show UID on serial monitor
  Serial.println(" ");
  Serial.println("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));}
  Serial.println(" ");
  content.toUpperCase();

  //Checking the passcode:
  if(keycount==codeLength-1){
    lcd.clear();
    Serial.println(" ");
    delay(1000);
    
    //checking if both the passcode AND employee tag are correct
    //if both passcode AND employee tag are correct
    if(!strcmp(Code,PW) && (content.substring(1) == "9C ED 49 23")) {
      Serial.println("Access Granted");
      lcd.print("Access Granted");
      myservo.write(450);
      delay(1000);} 

      //if the passcode is incorrect but the employee tag is correct.
      else if(strcmp(Code,PW) && (content.substring(1) == "9C ED 49 23")){
        Serial.println("Please enter the correct passcode.");
        lcd.print("Please enter the");
        lcd.setCursor(0,1);
        lcd.print("correct passcode.");
        delay(1000);
        lcd.setCursor(0,0);}

      //if the passcode is correct but the incorrect employee tag is entered
      else if(!strcmp(Code,PW) && (content.substring(1) != "9C ED 49 23")){
        Serial.println("Please scan an authorized tag.");
        lcd.print("Please scan an");
        lcd.setCursor(0,1); 
        lcd.print("authorized tag.");
        delay(1000);
        lcd.setCursor(0,0);}

      //if both the passcode and employee tag inputted are incorrect  
      else{
        Serial.println("Access Denied");
    	  lcd.print("Access Denied");}
   
   deleteCount();
   lcd.clear();}
         	 
  //Notes the tools/toolboxes used.    
  if(content.substring(1) == "96 1C E9 25"){
    Serial.println("Toolbox 1 registered.");
    lcd.print("Toolbox 1");
    lcd.setCursor(0,1);
    lcd.print("registered.");
    delay(2500);
    lcd.setCursor(0,0);}
}

void deleteCount(){
  while(keycount != 0){Code[keycount--]=0;}
  return;
}
