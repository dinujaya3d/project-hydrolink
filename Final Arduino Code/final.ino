  /*-----------------------------------------------------------------------------------------------
   #                                    WELLCOME TO HYDROLINK!                                    #
   #                                    Every drop does matter                                    #
   #                                                                                              #
   #                    Are you ready to be confident with your home water level                  #
  -----------------------------------------------------------------------------------------------*/


//include libraries
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>


// Provide the token generation process info.
#include <addons/TokenHelper.h>

/* Define the WiFi credentials */
#define WIFI_SSID "Abc"
#define WIFI_PASSWORD "abcdefgh"

/* Define the Firebase configuration */
/* 2. Define the API Key */
#define API_KEY "AIzaSyCbFKic6XziEe4FJEeWFW-ea3Qt-rEhDE0"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://hydrolink8-default-rtdb.firebaseio.com/" 

/* Define the user email and password */
#define USER_EMAIL "test01@gamil.com"
#define USER_PASSWORD "test01_DLP"

int get_val = 0;

// Define Firebase objects
FirebaseAuth auth;
FirebaseConfig config;
FirebaseData fbdo;

unsigned long sendDataPrevMillis = 0;
unsigned long count = 0;



//-----------------------ULTASONIC-------------------------------
// Define pins for ultrasonic
const int trigPin = 4;  // D6 Use the actual GPIO pin number connected to the trigger pin of the ultrasonic sensor
const int echoPin = 5;  // D5 Use the actual GPIO pin number connected to the echo pin of the ultrasonic sensor
const float SOUND_VELOCITY = 0.034;

const int BAT = A0;  //pin for battry
float RatioFactor = 5.2;  //Resistors Ration Factor

//const int seloPin = 10;  // pin for selonoid valve
int seloPin = LED_BUILTIN;
const int relay = 15;
int flow;

long duration;
float distanceCm;

float initial_height_of_tank;

float Tvoltage = 0;
float Vvalue = 0.0, Rvalue = 0.0;
float H_max;
float Error_distance;

float App_user_LTH;
float App_user_UTH;

bool App_control_method;
bool Is_Flow;
bool Is_switch_ON;
int Maintain_percentage;

int loop_count = 0;
int height = 0;

void setup() {
  Serial.begin(9600);
  //-----------------------------------------------------------------

  pinMode(seloPin, OUTPUT);
  //-----------------Ultrasonic----------------------------
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);   // Sets the echoPin as an Input
  //------------------------------------------------

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  // Set Firebase configuration
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Set token status callback
  config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  Serial.println("Connected to Firebase!");
  Firebase.setBool(fbdo, "/Tanks/HYD00001/Connected", true);
}


void loop()

{
  //-----------------------------------------------------------------------------------------------
  //                                      Get Intitial Hight                                      #
  //-----------------------------------------------------------------------------------------------
 
 //Get the values of H_max and Error_distance from Firebase   
  Firebase.getFloat(fbdo, "/Tanks/HYD00001/H_max"); H_max = fbdo.floatData();
  Firebase.getFloat(fbdo, "/Tanks/HYD00001/Error_distance");  Error_distance = fbdo.floatData();

  // Calculate the initial height of the tank
  float full_tank_capacity = H_max - Error_distance;

  // Print the initial height of the tank
  Serial.print(full_tank_capacity);

  //-----------------------------------------------------------------------------------------------
  //                                      Battry Values                                           #
  //-----------------------------------------------------------------------------------------------
 
  for (unsigned int i = 0; i < 10; i++) {
    Vvalue = Vvalue + analogRead(BAT);  //Read analog Voltage
    delay(5);                           //ADC stable
  }
  Vvalue = (float)Vvalue / 10.0;  //Find average of 10 values

  Rvalue = (float)(Vvalue / 1023.0) * 3.1;  //Convert Voltage in 3.3v factor
  Tvoltage = Rvalue * RatioFactor;          //Find original voltage by multiplying with factor
  Firebase.setString(fbdo, "/Tanks/HYD00001/Battery", Tvoltage);
  //__________________________________________________________________________________

  //---- Get Flow type from Firebase
  Firebase.getString(fbdo, "/Tanks/HYD00001/Flow_type");
  Serial.print("FLow type");
  String flow_type = fbdo.to<String>();
  if (flow_type == "sole") {
    flow = seloPin;
  } else if (flow_type == "relay") {
    flow = relay;
  }

  int Maintain_percentage = fbdo.to<int>();
  //-----------------------------------------------------------------------------------------------
  //                                      Ultrasonic                                              #
  //-----------------------------------------------------------------------------------------------

  Firebase.getString(fbdo, "/Tanks/HYD00001/H_max");
  int H_max = fbdo.to<int>();
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(200);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  unsigned long duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  float distanceCm = duration * SOUND_VELOCITY / 2;

  float water_level_percentage = (H_max - distanceCm) / full_tank_capacity;

  Firebase.setFloat(fbdo, "/Tanks/HYD00001/Distance_from_US", distanceCm);

  //-----------------------------------------------------------------------------------------------
  //                                      Get Flow State                                          #
  //-----------------------------------------------------------------------------------------------
 
  Firebase.getString(fbdo, "/Tanks/HYD00001/flow"); bool Is_Flow = fbdo.to<bool>();

  //_______________________________________________________________________________
  Firebase.getBool(fbdo, "/Tanks/HYD00001/Auto"); bool App_control_method = fbdo.boolData();  //True = Auto

  //---- Get control method from Firebase
  Firebase.getString(fbdo, "/Tanks/HYD00001/SwitchOn"); bool Is_switch_ON = fbdo.to<bool>();
  
  int seloPin = LED_BUILTIN;

  Firebase.getString(fbdo, "/Tanks/HYD00001/Upper_threshold"); int App_user_UTH = fbdo.to<int>();
  Firebase.getString(fbdo, "/Tanks/HYD00001/Lower_threshold"); int App_user_LTH = fbdo.to<int>();

  if (App_user_UTH >= distanceCm) {
    digitalWrite(flow, LOW);
    Firebase.setBool(fbdo, "/Tanks/HYD00001/SwitchOn", false);

  } else if (Is_switch_ON) {
    digitalWrite(flow, HIGH);
    Firebase.setBool(fbdo, "/Tanks/HYD00001/SwitchOn", true);

  } else {
    if ((App_user_LTH < distanceCm) && (App_control_method == true)) {
      digitalWrite(flow, HIGH);
      Firebase.setBool(fbdo, "/Tanks/HYD00001/SwitchOn", true);
    } else {
      digitalWrite(flow, LOW);
      Firebase.setBool(fbdo, "/Tanks/HYD00001/SwitchOn", false);
    }
  }
  //------------------------------------------------------------

  //Set water level percentage value in DB
  Firebase.setFloat(fbdo, "/Tanks/HYD00001/Percentage", water_level_percentage);
  loop_count++;
  height = height + distanceCm;

  if (loop_count == 80) {
    float deltaH = height;
    if (deltaH != 0) {
      Firebase.setBool(fbdo, "/Tanks/HYD00001/flow", true);
    } else {
      Firebase.setBool(fbdo, "/Tanks/HYD00001/flow", false);
    }
  }
}
