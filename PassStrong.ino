#include <BleKeyboard.h>
#include <M5StickC.h>
#include <WiFi.h>
#include "Parameters.h"
#include "Bitmaps.h"
#include "EEPROM.h"
#include <hwcrypto/aes.h>

BleKeyboard bleKeyboard(DEVICE_NAME,"PassStrong",99,HID_TYPE); // The last parameter only work with a modified version of Blekeyboard library. Discard it if you don't have it.
int refresh=3000/DELAYL;// Main loop Counter 
int dmode=-1;           // Mode -1:Try to connect 0: Main Menu 1,2,3,4,5: Passn choice menu
int canwrite=0;         // Pin write mode ? 
char pin[CODELEN];      // Pin code
char opin[CODELEN];     // Pin backup for code change
int passn=0;            // Mode qwerty or azerty
int confgen=0;          // Regen confirmation mode

// Encoding routines taken from CNLohr
// You must be encrypting/decrypting data that is in BLOCKSIZE chunks!!!
char cryptpass[PASSLEN+1];

// Genkey : for added security, you need to change AFACTOR and BFACTOR for each unit, or one can guess the key, and uncrypt the password
void genkey(char *p){
  uint8_t j=0,r=0;
  for (int i=0;i<32;++i){
    r=(p[j]*AFACTOR+BFACTOR*r)&255;
    key[i]=r;
    j=(j+1)%CODELEN;}
}

// Clear password
void clearpass(){
  memset((uint8_t*) password, 0, PASSLEN ); // Clear 
}

// Crypts password into EEPROM and clears password
void encrypt(char *p=pin){
  esp_aes_context ctx;
  memset( iv, 0, sizeof( iv ) ); 
  esp_aes_init( &ctx );
  genkey(p);
  esp_aes_setkey( &ctx, key, 256 );
  esp_aes_crypt_cbc( &ctx, ESP_AES_ENCRYPT, PASSLEN, iv, (uint8_t*)password, (uint8_t*)cryptpass );
  for (int i=0;i<PASSLEN;++i) EEPROM.writeChar(CODELEN+2+i,cryptpass[i]);
  clearpass();
  EEPROM.commit();
  esp_aes_free( &ctx );
}

// Uncrypts password from EEPROM. Nb : do clearpass() as soon as possible 
void decrypt(char *p=pin){
  esp_aes_context ctx;
  memset( iv, 0, sizeof( iv ) );  
  esp_aes_init( &ctx );
  for (int i=0;i<PASSLEN;++i) cryptpass[i]=EEPROM.readChar(CODELEN+2+i);
  genkey(p);
  esp_aes_setkey( &ctx, key, 256 );
  esp_aes_crypt_cbc( &ctx, ESP_AES_DECRYPT, PASSLEN, iv, (uint8_t*)cryptpass, (uint8_t*)password );
  esp_aes_free( &ctx );
}

// Regenerate random password and flashit
void regen(){
  for (int i=0;i<PASSLEN;++i) password[i]=possletters[random(possletters.length())];
  encrypt();
}

// Qwerty printer
void qwprint(String text){
 for (int i=0;i<text.length();++i){
   bleKeyboard.press(text[i]&127);
   delay(50);
   bleKeyboard.releaseAll();}
}

// Azerty printer
void azkprint(String text){
 for (int i=0;i<text.length();++i){
   uint16_t c=qwert_conv[(unsigned int)(text[i]&127)];
   if (c&SHIFT) bleKeyboard.press(KEY_LEFT_SHIFT);
   if (c&ALTGR) bleKeyboard.press(KEY_RIGHT_ALT);
   bleKeyboard.press(c&127);
   delay(50);
   bleKeyboard.releaseAll();
   }
}

// Types password
void bttype(){
  bleKeyboard.releaseAll();
  delay(DELUL*4);
  decrypt();
  if (passn) azkprint(password);
  else qwprint(password);
  clearpass();
  delay(DELUL*4);
}

// Shows bitmap
void showbit(const unsigned char *btm){
  M5.Lcd.fillScreen(TFT_BLACK); // Black screen fill
  M5.Lcd.drawXBitmap(0,0, btm, 80, 160, TFT_WHITE);
}
  
// Device setup
void setup(void) {
  while (!EEPROM.begin(EEPROM_SIZE)) true;
  M5.begin();
  WiFi.mode(WIFI_MODE_NULL);
  pinMode(37,INPUT);
  showbit(init1);
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("Starting");
#endif
  for (int i=0;i<CODELEN+1;++i) pin[i]='0';
  pin[CODELEN]='\0';
  bleKeyboard.begin();
  randomSeed(esp_random());
  if ((EEPROM.readChar(CODELEN)!='O')||(EEPROM.readChar(CODELEN+1)!='K')){
   for (int i=0;i<CODELEN;++i) EEPROM.writeChar(i,'0');
   regen(); // Generate a password to avoid problem with random non ascii characters
   canwrite=1; // Allow pin change
  } else canwrite=0;
}

// Pincheck
int pincheck(){
   for (int i=0;i<CODELEN;++i) if (pin[i]!=EEPROM.readChar(i)) return(0);
   return(1);}

// Settings status
void setstat(){
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(TFT_WHITE);  // Adding a background colour erases previous text automatically
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0,00);  
  M5.Lcd.print(dmode<5?">Pin:":"Pin:");
  M5.Lcd.setCursor(0,18);  
  M5.Lcd.print(String(pin));  
  M5.Lcd.setCursor(0,36);  
  M5.Lcd.print(dmode==5?">Chnge":"Change");  
  M5.Lcd.setCursor(0,54);  
  M5.Lcd.print(dmode==6?">Mode:":"Mode:");  
  M5.Lcd.setCursor(0,72);  
  M5.Lcd.print(modest[passn]);  
  M5.Lcd.setCursor(0,90);  
  M5.Lcd.print(dmode==7?(confgen==1?"*Regen":">Regen"):"Regen");  
  M5.Lcd.setCursor(0,108);  
  M5.Lcd.print((EEPROM.readChar(CODELEN)=='O')&&(EEPROM.readChar(CODELEN+1)=='K')?"Pinset":"No pin");  
}

// Action if small button pressed
void dosmall(){
  switch(dmode){
    case 0: // Home menu
        if (pincheck()){
         showbit(sending);
         bttype();
         dmode=-1;
         confgen=canwrite=0;
         refresh=80;}
        break;
    case 7: // Regen menu
        if (confgen==0) confgen=1; // Ask for confirmation
        else{    
          confgen=0;               // Confirmed     
          if (pincheck()){
           bttype();
           regen();
           M5.Lcd.setCursor(0,90);  
           M5.Lcd.print("Regnd");
           delay(300);  
           refresh=80;
           dmode=-1;}
        break;
    case 6: 
        canwrite=0;
        passn=(passn+1)%2; // AZ/QW ERTY shift
        break;
    case 5: // Change pin
        if (canwrite){  // Is it possible (either entered correct pin, or no pin set)
          M5.Lcd.setCursor(0,36);  
          M5.Lcd.print("Chnged");
          delay(300); 
          for (int i=0;i<CODELEN;++i) opin[i]=EEPROM.readChar(i); 
          decrypt(opin); // Decryot password with old pin
          for (int i=0;i<CODELEN;++i) EEPROM.writeChar(i,pin[i]);
          EEPROM.writeChar(CODELEN,'O');
          EEPROM.writeChar(CODELEN+1,'K');
          encrypt();
          dmode+=1;}
        else{  // First pass on the menu
          if (pincheck()){ // If pin is correct then back to pin to enter the new one
           dmode=1;
           canwrite=1;
           M5.Lcd.setCursor(0,36);  
           M5.Lcd.print("=>Chg");
           delay(300);}  
        }
        break;
    case 1: case 2: case 3: case 4:
        pin[dmode-1]=pin[dmode-1]>='9'?'0':pin[dmode-1]+1;}
        setstat();
        refresh=2*300/DELAYL;
        break;
  }
}

void dobig(){
 dmode=(dmode+1)%8;
 if (dmode==0) showbit(menu);
 else setstat();
 refresh=20;
}

void loop() {
 refresh-=2;
 if (refresh<=0){
  refresh+=11;
  if (bleKeyboard.isConnected()){
    if (dmode<0){
      if (dmode==-2){
      showbit(connectok);
      delay(1000);}
      showbit(menu);
      dmode=0;
      }
    if (digitalRead(BUTTON_B_PIN)==LOW){
      dosmall();}
   else if (digitalRead(BUTTON_A_PIN)==LOW){
    dobig();}
   }
  else {
    showbit(refresh&1?init1:init2);
    dmode=-2;
 #ifdef DEBUG
  Serial.println(refresh);
 #endif
    }
  }
 delay(DELAYL);
}
