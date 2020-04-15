#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Update these with values suitable for your network.

const char* ssid = "interactive robotics"; //SSID
const char* password = "password"; //PASSWORD
const char* mqtt_server = "iwancilibur.my.id"; //Server Hostname 
      char* clientid="iwanciliburDevice01"; //DEVICE NAME *Harus Unix tidak sama dengan yang lain

int led1=D5; //RELAY1

#define DHTTYPE DHT11
#define DHTPIN  D2
DHT dht(DHTPIN, DHTTYPE);

#define kelembaban_topic "sensor/kelembaban" //Topik unutkmengirim ke Server MQTT
#define suhu_topic "sensor/suhu" //Topik unutkmengirim ke Server MQTT
#define analog_topic "sensor/analog" //Topik unutkmengirim ke Server MQTT

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(led1, OUTPUT);     // Initialize Output

  Serial.begin(115200);
  setup_wifi();
  dht.begin();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *Topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(Topic);
  Serial.println("] ");
  if (strcmp(Topic, "lampu") == 0) //Membandingkan 2 buah String
  { 
    for (int i=0;i<length;i++) {
    char receivedChar = (char)payload[i];
    if (receivedChar == '1')
       digitalWrite(led1, HIGH);
    if (receivedChar == '0')
       digitalWrite(led1, LOW);
    }
  }
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT....");
    // AtsuhuPublisht to connect
    if (client.connect(clientid)) {
      Serial.println("MQTT Terhubung");
      client.subscribe("lampu"); //Initialitation Suscribe untuk RELAY
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {

  if (!client.connected()) {
    reconnect();
  }
    client.loop();
    delay(1000);
    
    float suhuPublish = 0.0;
    float kelembabanPublish = 0.0;
    int   analog=0;

    //Aktifkan apabila menggunakan Sensor DHT11
    //float dataSUHU = dht.readTemperature();
    //float dataKELEMBABAN = dht.readHumidity();
    //int dataAnalog=analogRead(A0);

    //Menggunakan data Random jika tidak memiliki sensor DHT11
    float dataSUHU = random(0,100);
    float dataKELEMBABAN = random(0,100);
    int dataAnalog=random(0,1024);

    //POST To Serial
    Serial.println(String(dataSUHU).c_str());
    Serial.println(String(dataKELEMBABAN).c_str());
    Serial.println(String(dataAnalog).c_str());

    //POST to MQTT Broker
    client.publish(suhu_topic, String(dataSUHU).c_str(), true);
    client.publish(kelembaban_topic, String(dataKELEMBABAN).c_str(), true);
    client.publish(analog_topic, String(dataAnalog).c_str(), true);
}
