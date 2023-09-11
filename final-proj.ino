#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <time.h>
#include <vector>
#include <chrono>
#include <Ticker.h>
#include <sys/time.h>
#include <ctime>

using std::vector;
//using namespace std::chrono;

#ifndef STASSID
#define STASSID "a-r1"
#define STAPSK  "asd12345"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

Ticker Saver;
File f;
vector<vector<String>> v ;

unsigned long previousMil = millis();
unsigned long pmillis = millis();
float timezone = 3.5;
int dst = 0;

const char *TZstr = "IRDT-3:30IRST,M3.2.0/2:00:00,M11.1.0/2:00:00";
const char* mqttServer = "broker.emqx.io";  //public.mqtthq.com, mqtt.flespi.io, broker.hivemq.com
const char* mqttUserName = "scllam123"; 
const char* mqttPwd = "somekey!194";  
const char* clientID = "usernnmm0001"; 
const char* topic = "LEDStatuu"; 

unsigned long previousMillis = 0;
const long interval = 5000;

String msgStr = ""; 
float temp;

WiFiClient espClient;
PubSubClient client(espClient);

AsyncWebServer server(80);

int soundsensor = 4;
int Led_Y = 15;
int Led_R = 13;

int clap = 0;
long detection_range_start = 0;
long detection_range = 0;
boolean status_light1 = false;
boolean status_light2 = false;

String SendH(uint8_t led1stat,uint8_t led2stat){
String ptr = "<!DOCTYPE html> <html>\n";
 ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
 ptr +="<meta http-equiv=\"refresh\" content=\"15; url=http://smartlamp.local \"/> \n";
 ptr +="<title>LED Control</title>\n";
 ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
 ptr +="body{margin-top: 50px;  display: flex;  justify-content: center;background: linear-gradient(325deg, #439596, #bdcc6d, #506bac, #d7611f, #c15d95, #72bd71, #b73461);background-size: 1400% 1400%;-webkit-animation: AnimationName 48s ease infinite;-moz-animation: AnimationName 48s ease infinite;animation: AnimationName 48s ease infinite;}\n"; 
 ptr +="h1 {color: white;margin: 50px auto 30px;}\n";
 ptr +="h3 {color: white;margin-bottom: 50px;}\n";
 ptr +=".button {display: block;width: 80px;background-color: #E9F30B;border: none;color: lightblack;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
 ptr +=".button-on { background-color: #D1D1D0 ;}\n";
 ptr +=".button-on:active {background-color: #ADADAA ;}\n";
 ptr +=".button-off {background-color: #E9F30B;}\n";
 ptr +=".button-off:active {background-color: #DBE503;}\n";
 ptr +=".button-rep { color: white; background-color: #84d4e0 ;}\n";
 ptr +=".button-rep:active {color: white; background-color: #6fc4d1 ;}\n";
 ptr +="p {font-size: 14px;color:black ;margin-bottom: 10px;}\n";
 ptr +="div{width: 700px; height:500px;background: rgba(255, 255, 255, 0.18);border-radius: 16px;box-shadow: 0 4px 30px rgba(0, 0, 0, 0.1);backdrop-filter: blur(4.6px);-webkit-backdrop-filter: blur(4.6px);border: 1px solid rgba(255, 255, 255, 0.35); }\n";
 ptr +="@-webkit-keyframes AnimationName {0%{background-position:0% 38%} 50%{background-position:100% 63%} 100%{background-position:0% 38%}}\n";
 ptr +="@-moz-keyframes AnimationName { 0%{background-position:0% 38%}  50%{background-position:100% 63%} 100%{background-position:0% 38%}}\n";
 ptr +="@keyframes AnimationName { 0%{background-position:0% 38%} 50%{background-position:100% 63%} 100%{background-position:0% 38%}}\n";
 ptr +="</style>\n";
 ptr +="</head>\n";
 ptr +="<body>\n";
 ptr +="<div>\n";
 ptr +="<h1>Smart Lamps</h1>\n";
 ptr +="<h3>Use the below buttos to switch on and off lamps</h3>\n";
 if(led1stat)
 {ptr +="<p>Yellow LED Status: ON</p><a class=\"button button-off\" href=\"/led1_off\">OFF</a>\n";}
 else
  {ptr +="<p>Yellow LED Status: OFF</p><a class=\"button button-on\" href=\"/led1_on\">ON</a>\n";}
 if(led2stat)
 {ptr +="<p>Red LED Status: ON</p><a class=\"button button-off\" href=\"/led2_off\">OFF</a>\n";}
 else
 {ptr +="<p>Red LED Status: OFF</p><a class=\"button button-on\" href=\"/led2_on\">ON</a>\n";}
 ptr +="<a class=\"button button-rep\" href=\"/report\">Report</a>\n";
 ptr +="</div>\n";
 ptr +="</body>\n";
 ptr +="</html>\n";
 
  return ptr;
}


void storeintofile(){
   f = SPIFFS.open("/sound_data.txt" , "a+");
   if (f) {
     for(int i=0;i<v.size();i++){
          
          f.print(v[i][0]);
          f.print(",");
          f.println(v[i][1]);    
     }     
   }else{  Serial.println("file could not be opened") ;  }
    
  Serial.println("   Stored in the file.... ");
  f.close();
  v.clear();  
}


void reconnect() {
  while (!client.connected()) {
    if (client.connect(clientID, mqttUserName, mqttPwd)) {
      Serial.println("MQTT connected");
      client.subscribe("lights_yrLed159");
      Serial.println("Topic Subscribed");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);  
    }
  }
}


void callback(char*topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String data = "";  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    data += (char)payload[i];
  }
  Serial.println();
  Serial.print("Message size :");
  Serial.println(length);
  Serial.println();
  Serial.println("-----------------------");
  Serial.println(data);

if(data=="OFF"){
  status_light1 = false;
  digitalWrite(Led_Y,LOW);
}
else if(data=="ON"){
 status_light1 = true;
 digitalWrite(Led_Y, HIGH);
}
else if(data=="ON2"){
  status_light2 = true;
  digitalWrite(Led_R, HIGH);
}
else if(data=="OFF2"){
  status_light2 = false;
  digitalWrite(Led_R,LOW);
}
}

void setup() {
  pinMode(soundsensor, INPUT);
  pinMode(Led_Y, OUTPUT);
  pinMode(Led_R, OUTPUT);
  pinMode(5 , OUTPUT);
  pinMode(14 , OUTPUT);
  pinMode(16 , OUTPUT);
  Serial.begin(9600);

  if (! SPIFFS.begin ()) {
    Serial.println ("An Error has occurred while mounting SPIFFS");
    return;
  }
  SPIFFS.remove("/sound_data.txt");
  if ( SPIFFS.exists("/index1.html") ) Serial.println("\n/index1.html exists"); 
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("Smart_Lamps","123456987");
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  configTime(TZstr ,"pool.ntp.org", "time.nist.gov");

  
  server.on ("/", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request-> send (200, "text/html", SendH(digitalRead(Led_Y) , digitalRead(Led_R)));
  });

  server.on ("/led1_on", HTTP_GET, [] (AsyncWebServerRequest * request) {
    digitalWrite(Led_Y, HIGH);
    status_light1 = true;
    request-> send (200, "text/html", SendH(digitalRead(Led_Y) , digitalRead(Led_R)));
  });
  
  server.on("/led1_off", HTTP_GET, [] (AsyncWebServerRequest * request){
    digitalWrite(Led_Y, LOW);
    status_light1 = false;
    request-> send (200, "text/html", SendH(digitalRead(Led_Y) , digitalRead(Led_R)));
  });
 
  server.on("/led2_on", HTTP_GET, [] (AsyncWebServerRequest * request){
    digitalWrite(Led_R, HIGH);
    status_light2 = true;
    request-> send (200, "text/html", SendH(digitalRead(Led_Y) , digitalRead(Led_R)));
  });
  
  server.on("/led2_off", HTTP_GET, [] (AsyncWebServerRequest * request){
    digitalWrite(Led_R, LOW);
    status_light2 = false;
    request-> send (200, "text/html", SendH(digitalRead(Led_Y) , digitalRead(Led_R)));
  });

  server.on ("/report", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request-> send (SPIFFS, "/index1.html");
  }); 
  
  server.on ("/report_data", HTTP_ANY, [] (AsyncWebServerRequest * request) {
    request-> send (SPIFFS, "/sound_data.txt");
  });

  server.on ("/download", HTTP_ANY, [] (AsyncWebServerRequest * request) {
    request-> send (SPIFFS, "/sound_data.txt" , String() , true);
  });  
  
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "404 : Not Found!!");
  });
  
  
  server.begin(); 

  if (!MDNS.begin("smartlamp") ) {
    Serial.println("Error setting up MDNS responder!");
  }
  else{Serial.println("mDNS responder started");}
  MDNS.addService("http", "tcp", 80);
  
 
  client.setServer(mqttServer, 1883); //setting MQTT server
  client.setCallback(callback); 
  if (!client.connected()) {
    reconnect(); 
  }
  Saver.attach(6 ,storeintofile);
}


void loop() {
  MDNS.update();
    
  client.loop();
  if (!client.connected()) {
    reconnect(); 
  }
    unsigned long currentMil = millis();
  if( currentMil - previousMil > 180000){ 
       /* f = SPIFFS.open("/sound_data.txt" , "a+");
        Serial.println(f.readString());
        f.close();*/
        SPIFFS.remove("/sound_data.txt");
        Serial.println("************** File was Removed  :)    ... **********");
        previousMil = millis();
        v.clear(); 
        delay(1000);
  }
  else{   
      unsigned long currentMillis1 = millis();
      if ( currentMillis1 - pmillis > 3500 || digitalRead(4) == HIGH ){
      float value = analogRead(A0);
        uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()+12600000 ;
      Serial.println(ms);Serial.println("*************");
    
      v.push_back({String(ms), String(value)});          
      pmillis = millis();
    }
  }


  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) { //if current time - last time > 5 sec
    previousMillis = currentMillis;
    msgStr = String(digitalRead(Led_Y)) + ","+String(digitalRead(Led_R));
    byte arrSize = msgStr.length() + 1;
    char msg[arrSize];

    Serial.print("PUBLISH DATA:");
    Serial.println(msgStr);
    msgStr.toCharArray(msg, arrSize);
    client.publish(topic, msg);
    msgStr = "";
    delay(50);
  }
  
  int status_sensor = digitalRead(soundsensor);
  if (status_sensor == HIGH)
  {
    if (clap == 0)
    {
      detection_range_start = detection_range = millis();
      clap++;
      digitalWrite(5 , HIGH);
    }
    else if (clap > 0 && millis()-detection_range <= 700 && millis()-detection_range >= 100)
    {
      detection_range = millis();
      clap++;
      Serial.print(clap);
      digitalWrite(14, HIGH);
      if(clap == 3) {digitalWrite(16 , HIGH);}
    }
  }
  
  if (millis()-detection_range_start >= 1500)
  {
    if (clap == 2)
    {
      if (!status_light1)
        {
          status_light1 = true;
          digitalWrite(Led_Y, HIGH);
        }
        else if (status_light1)
        {
          status_light1 = false;
          digitalWrite(Led_Y, LOW);
        }
    }
    else if (clap == 3)
    {
      Serial.println(clap);
      if (!status_light2)
        {
          status_light2 = true;
          digitalWrite(Led_R, HIGH);
        }
        else if (status_light2)
        {
          status_light2 = false;
          digitalWrite(Led_R, LOW);
        }
    }
    digitalWrite(5 , LOW);
    digitalWrite(14 , LOW);
    digitalWrite(16 , LOW);
    clap = 0;
  }
 
}

