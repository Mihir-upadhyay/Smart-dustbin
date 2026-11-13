#define BLYNK_TEMPLATE_ID "TMPL3ye1SsmOT"
#define BLYNK_TEMPLATE_NAME "smart dustbin 1"
#define BLYNK_AUTH_TOKEN "oFRoqZX_3FGwvbX7swtSdFmZEyUT65jy"

#include <Servo.h>
#include <BlynkSimpleEsp8266.h>

// Auth Token, SSID, and Password
char auth[] = "oFRoqZX_3FGwvbX7swtSdFmZEyUT65jy"; // Replace with your Blynk Auth Token
char ssid[] = "MIHIR"; // Replace with your WiFi SSID
char pass[] = "mihir6868"; // Replace with your WiFi Password

Servo servo1;
Servo servo2;

// Ultrasonic sensor pins
const int TRIG_PIN1 = D5;
const int ECHO_PIN1 = D6;
const int TRIG_PIN2 = D7;
const int ECHO_PIN2 = D8;

const int IR_SENSOR1 = D1;
const int IR_SENSOR2 = D2;

bool sensor1Active = false;
bool sensor2Active = false;

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  servo1.attach(D0);  // attach servo 1 to pin D3
  servo2.attach(D4);  // attach servo 2 to pin D4
  servo1.write(0);
  servo2.write(100);

  pinMode(IR_SENSOR1, INPUT);
  pinMode(IR_SENSOR2, INPUT);

  pinMode(TRIG_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);

  Serial.println("Setup complete.");
}

long readUltrasonicDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  Blynk.run();

  long distance1 = readUltrasonicDistance(TRIG_PIN1, ECHO_PIN1);
  long distance2 = readUltrasonicDistance(TRIG_PIN2, ECHO_PIN2);

  int fillLevel1 = map(distance1, 2, 10, 100, 0);
  int fillLevel2 = map(distance2, 2, 10, 100, 0);

  Blynk.virtualWrite(V0, fillLevel1);
  Blynk.virtualWrite(V1, fillLevel2);

  Serial.print("Distance1: ");
  Serial.println(distance1);
  Serial.print("FillLevel1: ");
  Serial.println(fillLevel1);
  Serial.print("Distance2: ");
  Serial.println(distance2);
  Serial.print("FillLevel2: ");
  Serial.println(fillLevel2);

  bool sensor1State = digitalRead(IR_SENSOR1) == LOW;
  bool sensor2State = digitalRead(IR_SENSOR2) == LOW;

  Serial.print("IR_SENSOR1: ");
  Serial.println(sensor1State ? "LOW" : "HIGH");
  Serial.print("IR_SENSOR2: ");
  Serial.println(sensor2State ? "LOW" : "HIGH");

  // Check IR Sensor 1
  if (sensor1State && !sensor1Active) {
    sensor1Active = true;
    // Move servo1 to 100 degrees
    Serial.println("Moving servo1 to 100");
    servo1.write(100);
    delay(5000);  // Wait 5 seconds at 100 position
    // Move servo1 to 0 degrees
    Serial.println("Moving servo1 to 0");
    servo1.write(0);
  } else if (!sensor1State) {
    sensor1Active = false;
  }

  // Small delay between actions
  delay(200);

  // Check IR Sensor 2
  if (sensor2State && !sensor2Active) {
    sensor2Active = true;
    // Move servo2 to 0 degrees
    Serial.println("Moving servo2 to 0");
    servo2.write(0);
    delay(5000);  // Wait 5 seconds at 0 position
    // Move servo2 to 100 degrees
    Serial.println("Moving servo2 to 100");
    servo2.write(100);
  } else if (!sensor2State) {
    sensor2Active = false;
  }

  // Small delay before next loop iteration
  delay(200);
}
