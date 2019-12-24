#include <ESP8266WiFi.h>
#include <IRCClient.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);
#define IRC_BUFSIZE  32  
#define ssid         "NamaSSID"                 //Disamakan dengan pengaturan Hotspot di Android atau AP kalian
#define password     "PassSSID"                 //Disamakan dengan pengaturan Hotspot di Android atau AP kalian
#define ON HIGH
#define OFF LOW

int lock_pin      = 16;
int unlock_pin    = 15;
int acc_pin       = 14;
int kontak_pin    = 12;
int stater_pin    = 5;
int ecu_pin       = 4;
int cool_pin      = 13;
int dome_pin      = 2;
int dusk_pin      = 0;

#define IRC_SERVER   "ZNC_server.com"             //Alamat Server irc atau server ZNC
#define IRC_PORT     123123                       //Port IRC atau port ZNC
#define IRC_NICKNAME randNumber
#define IRC_USER     "mobile"                     //Username ZNC
#define REPLY_TO     "shamy"                      //Nick pada IRC yg dapat memberi perintah
#define IRC_PASS     "passZNC"                    //Passwd IRC server atau ZNC server
const String Channel = "shamy";                   //Alamat channel IRC
String ircChannel = "";
long randNumber;
char randNick[50];

WiFiClient wiFiClient;
IRCClient client(IRC_SERVER, IRC_PORT, wiFiClient);

void setup() {
  Serial.begin(115200);
  pinMode(unlock_pin, OUTPUT);digitalWrite(unlock_pin, OFF);
  pinMode(lock_pin, OUTPUT);digitalWrite(lock_pin, OFF);
  pinMode(acc_pin, OUTPUT);digitalWrite(acc_pin, OFF);
  pinMode(kontak_pin, OUTPUT);digitalWrite(kontak_pin, OFF);
  pinMode(stater_pin, OUTPUT);digitalWrite(stater_pin, OFF);
  pinMode(cool_pin, OUTPUT);digitalWrite(cool_pin, OFF);
  pinMode(dome_pin, OUTPUT);digitalWrite(dome_pin, OFF);
  pinMode(dusk_pin, OUTPUT);digitalWrite(dusk_pin, OFF);

  delay(100);
  Serial.println("");
  Serial.print("Menghubungi Wifi ");
  Serial.println(ssid);
 
  WiFi.mode(WIFI_STA);
  WiFi.hostname("MeAyla");
  WiFi.begin(ssid, password);
  IPAddress ip(192, 168, 43, 77);                     //ini alamat IP local, biar dapat diakses lewat Aplikasi Android atau PC
  IPAddress dns(192, 168, 43, 1);
  IPAddress gateway(192, 168, 43, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, dns, gateway, subnet);
     
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi terhubung");
  Serial.println("Alamat IP: ");
  Serial.println(WiFi.localIP());
  WiFi.printDiag(Serial);
  ircChannel = "#" + Channel;

  client.setCallback(callback);
  client.setSentCallback(debugSentCallback);

  server.on ( "/", HTTP_handleRoot );
  server.onNotFound ( HTTP_handleRoot );
  server.begin();
}

void loop() {
     server.handleClient();
     delay(50);
     random_nick();
     cek_koneksi();
     client.loop();
}

void random_nick(){
     unsigned long randNumber = random(1000);
     sprintf(randNick, "ESP%lu", randNumber);
    }

void cek_koneksi(){
    if (!client.connected()) {
    Serial.println("Menghubungi IRC server");
    if (client.connect(randNick, IRC_USER, IRC_PASS)) {
      Serial.println("terhubung");
      delay(5000);
      join_channel();
      } else {
      Serial.println("Konkesi gagal, mengulang dalam 5 detik");      //mencoba kembali dalam 5 detik
      delay(5000);
      }
    return;
  }
}

void join_channel(){
       if (client.connected()) {
       Serial.println("Join Channel");
       client.sendRaw("JOIN " + ircChannel);
       }
     }

void HTTP_handleRoot(void) {
     if ( server.hasArg("Unlock")) {
          Unlock();}
     if ( server.hasArg("Lock")) {
          Lock();}
     if ( server.hasArg("Acc_on")) {  
          Acc_on();}
     if ( server.hasArg("Acc_off")) {
          Acc_off();}
     if ( server.hasArg("Kontak_on")) {
          Kontak_on();}
     if ( server.hasArg("Kontak_off")) {
          Kontak_off();}
     if ( server.hasArg("Stater") && digitalRead(kontak_pin)== ON) {
          Stater();}
     if ( server.hasArg("Ecu_on")) {
          Ecu_on();}
     if ( server.hasArg("Ecu_off")) {
          Ecu_off();}
     if ( server.hasArg("Dome_on")) {
          Dome_on();}
     if ( server.hasArg("Dome_off")) {
          Dome_off();}  
     if ( server.hasArg("Cool_on")) {
          Cool_on();}
     if ( server.hasArg("Cool_off")) {
          Cool_off();}
     if ( server.hasArg("Dusk_on")) {
          Dusk_on();}
     if ( server.hasArg("Dusk_off")) {
          Dusk_off();}      
}

void callback(IRCMessage ircMessage) {
    // PRIVMSG ignoring CTCP messages
  if (ircMessage.command == "PRIVMSG" && ircMessage.text[0] != '\001') {
    String message("<" + ircMessage.nick + "> " + ircMessage.text);
    Serial.println(message);

    if (ircMessage.nick == REPLY_TO) {
        if(ircMessage.text == "!HELP"){
           client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! Perintahnya adalah :");
           client.sendMessage(ircMessage.nick, "UNLOCK");
           client.sendMessage(ircMessage.nick, "LOCK");
           client.sendMessage(ircMessage.nick, "ACC");
           client.sendMessage(ircMessage.nick, "KONTAK");
           client.sendMessage(ircMessage.nick, "STATER");
           client.sendMessage(ircMessage.nick, "KONTAK OFF");
           client.sendMessage(ircMessage.nick, "ECU ON");
           client.sendMessage(ircMessage.nick, "ECU OFF");
           client.sendMessage(ircMessage.nick, "AC ON");
           client.sendMessage(ircMessage.nick, "AC OFF");
           client.sendMessage(ircMessage.nick, "DOME ON");
           client.sendMessage(ircMessage.nick, "DOME OFF");
        }else{
         if(ircMessage.text == "UNLOCK"){
           client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! Pintu sekarang tidak terkunci");
           Unlock();
        }else{
         if(ircMessage.text == "LOCK"){
           client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! Pintu sekarang terkunci");
           Lock();
        }else{
         if(ircMessage.text == "ACC"){
           client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! Kontak ACC Aktif");
           Acc_on(); 
        }else{
         if(ircMessage.text == "KONTAK"){
           client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! Kontak Ready, mesin siap menyala");
           Kontak_on();
        }else{
         if(ircMessage.text == "STATER" && digitalRead(kontak_pin)== ON){
           client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! Mesin menyala");
           Stater();   
        }else{
         if(ircMessage.text == "KONTAK OFF"){
           client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! Mesin OFF, Kelistrikan OFF");
           Kontak_off();   
        }else{
         if(ircMessage.text == "ECU ON"){
           client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! Electronic Control Unit ON");
           Ecu_on();
        }else{
         if(ircMessage.text == "ECU OFF"){
           client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! Electronic Control Unit OFF");
           Ecu_off();
         }else{
         if(ircMessage.text == "DOME ON"){
           client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! Lampu dalam menyala");
           Dome_on();  
         }else{
         if(ircMessage.text == "DOME OFF"){
           client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! Lampu dalam padam");
           Dome_off();
         }else{
         if(ircMessage.text == "AC ON"){
           client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! Pendingin Ruangan Menyala");
           Cool_on();
         }else{
         if(ircMessage.text == "AC OFF"){
           client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! Pendingin Ruangan padam");
           Cool_off();
         }else{
         if(ircMessage.text == "DUSK ON"){
           client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! Lampu senja menyala");
           Dusk_on();  
         }else{
         if(ircMessage.text == "DUSK OFF"){
           client.sendMessage(ircMessage.nick, "Hi " + ircMessage.nick + "! Lampu senja padam");
           Dusk_off();    
        }}}}}}}}}}}}}}}}
                 return;
    }
  Serial.println(ircMessage.original);
}

void debugSentCallback(String data) {
  Serial.println(data);
  }

void Unlock() {
    Serial.println("Unlock");
    digitalWrite(unlock_pin, ON);
    delay(500);
    digitalWrite(unlock_pin, OFF);
    }
    
void Lock(){
    Serial.println("Lock");
    digitalWrite(lock_pin, ON);
    delay(500);
    digitalWrite(lock_pin, OFF);
    }
    
void Acc_on(){
    Serial.println("Acc On");
    digitalWrite(acc_pin, ON);}

void Acc_off() {
    Serial.println("Acc Off");
    digitalWrite(acc_pin, OFF);
    }

void Kontak_on(){
    Serial.println("Kontak On");
    digitalWrite(kontak_pin, ON);
    digitalWrite(ecu_pin, ON);
    digitalWrite(acc_pin, ON);
    }

void Kontak_off(){
    Serial.println("Kontak Off");
    digitalWrite(acc_pin, OFF);
    digitalWrite(kontak_pin, OFF);
    digitalWrite(ecu_pin, OFF);
    digitalWrite(cool_pin, OFF);
    }
  
void Stater(){
    Serial.println("Stater");
    digitalWrite(stater_pin, ON);
    delay(1000);
    digitalWrite(stater_pin, OFF);}
    
void Ecu_on() {
    Serial.println("Ecu On");
    digitalWrite(ecu_pin, ON);
    }
    
void Ecu_off(){
    Serial.println("Ecu Off");
    digitalWrite(ecu_pin, OFF);
    }

void Dome_on(){
    Serial.println("Dome On");
    digitalWrite(dome_pin, ON);
    }

void Dome_off(){
    Serial.println("Dome Off");
    digitalWrite(dome_pin, OFF);
    }  

void Cool_on(){
    Serial.println("Cool On");
    digitalWrite(cool_pin, ON);
    }

void Cool_off(){
    Serial.println("Cool Off");
    digitalWrite(cool_pin, OFF);
    }     

void Dusk_on(){
    Serial.println("Dusk On");
    digitalWrite(dusk_pin, ON);
    }

void Dusk_off(){
    Serial.println("CDusk Off");
    digitalWrite(dusk_pin, OFF);
    }
