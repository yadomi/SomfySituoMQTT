// #include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>

// WIFI
char ssid[] = "*REDACTED*";
char password[] = "*REDACTED*";

// MQTT
char mqtt_broker[] = "*REDACTED*";
char mqtt_username[] = "*REDACTED*";
char mqtt_password[] = "*REDACTED*";
char mqtt_topic[] = "somfy/channel/#";

// SOMFY
enum SomfyButton {
  UP = 15,
  MY = 13,
  DOWN = 12,
  CHANNEL = 4
};

WiFiClient ESPClient;
PubSubClient MQTTClient(ESPClient);

/**
 * This assume the remote is on the first channel when powered-on
 * It can be a problematic since the remote memorize the last channel.
 * Idealy, this should be stored in the EEPROM
 */
int currentChannel = 1; 

void SomfyPress(SomfyButton button, int pressTime = 100) {
  Serial.print("SomfyPress: ");
  Serial.println(button);

  digitalWrite(button, LOW);
  delay(pressTime);
  digitalWrite(button, HIGH);
  delay(pressTime);
}

void SomfySelectChannel(int channel) {
  /**
   * When the remote is asleep, pressing the channel button only shows the current channel
   * But when the remote is awake (eg: channel indicator is lit), pressing the channel will +1 the channel.
   * 
   * TODO: adds more logic here, pressing CHANNEL consecutively will lose track of the internal counter. 
   * Needs either to wake for the remote to be alseep again or keep track of time between channel press
   */
  SomfyPress(SomfyButton::CHANNEL);

  /**
  * Iterate until we reach the desired channel  
  */
  int index;
  for (int i = 1; i < 5; i++) {
    index = (i + currentChannel) % 5;
    SomfyPress(SomfyButton::CHANNEL);
    if (index == channel) {
      break;
    }
  }

  SomfySaveCurrentChannel(channel);
}

void SomfySaveCurrentChannel(int channel) {
  currentChannel = channel;

  // TODO: save to eeprom
}

void MQTTCallback(char *t, byte *payload, unsigned int lenght) {
  String topic = String(t);
  String command = topic.substring(16); // up, down, my or prog
  int channel = topic.substring(14, 15).toInt(); // channel number, 0 to 4

  if (!(1 <= channel && channel <= 5)) return;

  if (command.equals("up")) {
    SomfySelectChannel(channel);
    SomfyPress(SomfyButton::UP);
    return;
  }

  if (command.equals("down")) {
    SomfySelectChannel(channel);
    SomfyPress(SomfyButton::DOWN);
    return;
  }

  if (command.equals("my")) {
    SomfySelectChannel(channel);
    SomfyPress(SomfyButton::MY);
    return;
  }

}

void setup() {
  pinMode(SomfyButton::UP, OUTPUT);
  pinMode(SomfyButton::DOWN, OUTPUT);
  pinMode(SomfyButton::MY, OUTPUT);
  pinMode(SomfyButton::CHANNEL, OUTPUT);
  digitalWrite(SomfyButton::UP, HIGH);
  digitalWrite(SomfyButton::DOWN, HIGH);
  digitalWrite(SomfyButton::MY, HIGH);
  digitalWrite(SomfyButton::CHANNEL, HIGH);

  Serial.begin(9600);
  Serial.println("Network: Connecting");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Network: conected.");
  MQTTClient.setServer(mqtt_broker, 1883);
  MQTTClient.setCallback(MQTTCallback);

  while (!MQTTClient.connected()) {
    Serial.println("MQTT: Connecting");
    String clientId = "SomfyRemote-";
    clientId += String(random(0xffff), HEX);

    if (MQTTClient.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("MQTT: Connected.");
    } else {
      Serial.print("MQTT: Failed");
      Serial.print(MQTTClient.state());
      delay(2000);
    }
  }

  MQTTClient.subscribe(mqtt_topic);
}

void loop() { MQTTClient.loop(); }