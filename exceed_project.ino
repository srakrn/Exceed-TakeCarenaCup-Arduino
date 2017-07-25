// Defining Standard Library
#include <Wire.h>
#include <UnoWiFiDevEd.h>

// Defining API system
#define CONNECTOR "rest"
#define SERVER_ADDR "158.108.165.223"
CiaoData data;

// Defining button values
#define btn_port 10
bool btn_state_before = false;
bool btn_state_now = false;

// Defining ultrasonic
#define ultra_trig 11
#define ultra_echo 12
float ultra_distance;

// Defining DHT libraries
#include <dht.h>
#define dht_pin 13
dht DHT;
int dht_temp;

int btn_value, volume;

// Here goes the code
void setup(void) {
  // Board things
  Ciao.begin();
  Serial.begin(9600);

  // Button things
  pinMode(2, INPUT);

  // Ultrasonic things
  pinMode(ultra_trig, OUTPUT);
  pinMode(ultra_echo, INPUT);
}

void loop(void) {
  // triggers data submission only if button is pressed at its first click
  btn_value = btn_read();
  Serial.println(btn_value);
  if (btn_value == 1) {
    ultra_distance = ultra_read();
    dht_temp = dht_read();
    volume = calculate_volume(ultra_distance);
    exceed_write("temp", String(dht_temp));
    exceed_write("volume", String(volume));
    Serial.print("\t");
    Serial.print(dht_temp);
    Serial.print(" ");
    Serial.print(ultra_distance);
    Serial.print(" ");
    Serial.println(volume);
  }
  delay(200);
}

String exceed_read(String key) {
  data = Ciao.write(CONNECTOR, SERVER_ADDR, "/data/8/" + key);
  Ciao.println( "ResponseREAD: " + String (data.get(2)));
  return String(data.get(2));
}

bool exceed_write(String key, String value) {
  data = Ciao.write(CONNECTOR, SERVER_ADDR, "/data/8/" + key + "/set/" + value);
  return !data.isEmpty();
}

int btn_read() {
  btn_state_before = btn_state_now;
  btn_state_now = !digitalRead(btn_port);
  if (!btn_state_before && !btn_state_now) {
    // 0 is button off
    return 0;
  }
  else if (!btn_state_before && btn_state_now) {
    // 0 is button on click
    return 1;
  }
  else if (btn_state_before && btn_state_now) {
    // 2 is button on hold
    return 2;
  }
  else if (btn_state_before && !btn_state_now) {
    // 3 is button just off
    return 3;
  }
}

float ultra_read() {
  long duration;
  digitalWrite(ultra_trig, LOW);
  delayMicroseconds(2);
  digitalWrite(ultra_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultra_trig, LOW);
  duration = pulseIn(ultra_echo, HIGH);
  return (duration / 2) / 29.1;
}

int dht_read() {
  DHT.read11(dht_pin);
  return DHT.temperature;
}

int calculate_volume(int distant){
  return (int)43.02*(22-distant);
}

