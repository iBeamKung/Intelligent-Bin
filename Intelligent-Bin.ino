#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
// ultrasonic
#define echoPin_1 D2 
#define trigPin_1 D1
#define echoPin_2 D4
#define trigPin_2 D3

long duration; 
int distance;
#define BLYNK_PRINT Serial
// water detection
#define SIGNAL_PIN D5

WidgetLED led_wet(V0);
WidgetLED led_full(V2);

int value = 0; // variable to store the sensor value

#include <Servo.h>
Servo myservo;

char auth[] = "----------------";   
char ssid[] = "----------------"; 
char pass[] = "----------------";

//int state = 0;


void setup()
{
  Serial.begin(9600);
  //OUTPUT
  pinMode(D0,OUTPUT);
  myservo.attach(12);

  digitalWrite(D0, HIGH);

  Blynk.begin(auth, ssid, pass);

  //Ultrasonic
  pinMode(trigPin_1, OUTPUT); 
  pinMode(echoPin_1, INPUT);
}
void loop()
{
  check_wet();
  check_full();
  person_detect();
  Blynk.run();
}

void servo_open(int state)
{
  if(state == 0)
  {
    
    //Blynk.virtualWrite(V1, 0);
    myservo.write(0);
    Serial.println("servo 0");
    delay(500);
  }
  else if(state == 1)
  {
    //Blynk.virtualWrite(V1, 1);
    myservo.write(180);
    Serial.println("servo 180");
    delay(500);
  }
}

BLYNK_WRITE(V1){ 
  int state = 0;
  Serial.println(param.asInt()); 
    if (param.asInt()){
        digitalWrite(D0, LOW);
//        servo_open(1);
//        state = 1; //open
        person_detect(1);
    } else {
        digitalWrite(D0, HIGH);
//        servo_open(0);
//        state = 0; //close
        person_detect(0);
    }
}

void check_wet(){
  
  value = digitalRead(SIGNAL_PIN); // read the analog value from sensor
  Serial.print("Check Wet : ");
  Serial.println(value);

  if(value == 1)//no wet
  {
    led_wet.off();
  }
  else
  {
    led_wet.on();
  }
}

void check_full(){
  
  int trash_distance = distance_detect(trigPin_2,echoPin_2);
  Serial.print("Distance Full : ");
  Serial.print(trash_distance);
  Serial.println(" cm");
  if(trash_distance < 5)//full
  {
    led_full.on();
  }
  else
  {
    led_full.off();
  }
}

int distance_detect(uint8_t  trigPin,uint8_t  echoPin){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  //Serial.print("Distance: ");
  //Serial.print(distance);
  //Serial.println(" cm");
  return distance;
}

void person_detect(int s=0)
{
  int state = s;
  if(state == 1)
  {
    servo_open(1);
  }
  else
  {
    int person_distance = distance_detect(trigPin_1,echoPin_1);
    Serial.print("Distance OpenFull : ");
    Serial.print(person_distance);
    Serial.println(" cm");
    if(person_distance < 80)
    {
      servo_open(1);
    }
    else
    {
      servo_open(0);
    }
  }
}
