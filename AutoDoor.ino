/*
  Smart Door Control System using ESP32 & HiveMQ
  IoT Engineer Configuration
*/

#include <WiFi.h>
#include <PubSubClient.h>

// بيانات شبكة الواي فاي
const char* ssid = "YOUR_WIFI_NAME_HERE";
const char* password = "YOUR_WIFI_PASSWORD_HERE";

// إعدادات HiveMQ
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883; // منفذ TCP القياسي للـ ESP32

// مواضيع MQTT (يجب أن تتطابق مع الموجودة في صفحة الويب)
const char* topicCmd = "MySmartDoor_102030/cmd";
const char* topicStatus = "MySmartDoor_102030/status";

// تعريف أطراف التوصيل حسب الجدول المطلوب
#define ENA 14 // التحكم بالسرعة
#define IN1 26 // التحكم بالاتجاه 1
#define IN2 27 // التحكم بالاتجاه 2

#define IR1 32 // حساس الدخول / البداية
#define IR2 33 // حساس النهاية / التوقف

WiFiClient espClient;
PubSubClient client(espClient);

// متغيرات حالة الباب
enum DoorState { STOPPED, OPENING, CLOSING };
DoorState currentState = STOPPED;

// إعداد السرعة (من 0 إلى 255)
const int motorSpeed = 200; 

void setup_wifi() {
  delay(10);
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

// دالة استقبال الأوامر من الويب
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  Serial.println(message);

  if (message == "OPEN") {
    openDoor();
  } else if (message == "CLOSE") {
    closeDoor();
  } else if (message == "STOP") {
    stopDoor();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // إنشاء ID عشوائي للـ ESP32
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // الاشتراك في موضوع الأوامر
      client.subscribe(topicCmd);
      client.publish(topicStatus, "ESP32 Connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // إعداد دبابيس المحرك
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  
  // إيقاف المحرك كحالة افتراضية
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  // إعداد دبابيس الحساسات
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void openDoor() {
  if(currentState == OPENING) return;
  
  Serial.println("Command: OPENING DOOR");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, motorSpeed);
  
  currentState = OPENING;
  client.publish(topicStatus, "OPENING");
}

void closeDoor() {
  if(currentState == CLOSING) return;
  
  Serial.println("Command: CLOSING DOOR");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, motorSpeed);
  
  currentState = CLOSING;
  client.publish(topicStatus, "CLOSING");
}

void stopDoor() {
  Serial.println("Command: STOPPING DOOR");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
  
  currentState = STOPPED;
  client.publish(topicStatus, "STOPPED");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // إبقاء الاتصال نشطاً لمعالجة الرسائل

  // قراءة الحساسات (غالباً الحساس يعطي LOW عند اكتشاف عائق)
  // يرجى التحقق إذا كان الحساس الخاص بك يعطي HIGH أم LOW عند الاكتشاف وتعديله إذا لزم الأمر
  int ir1State = digitalRead(IR1);
  int ir2State = digitalRead(IR2);

  // منطق حساس الفتح (IR1)
  if (currentState == OPENING) {
    if (ir1State == LOW) { // إذا وصل الباب إلى حساس البداية
      Serial.println("Sensor 1 Triggered: Door Fully Opened");
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, 0);
      currentState = STOPPED;
      client.publish(topicStatus, "OPENED");
      delay(1000); // تأخير بسيط لمنع التكرار المستمر
    }
  }

  // منطق حساس الغلق (IR2)
  if (currentState == CLOSING) {
    if (ir2State == LOW) { // إذا وصل الباب إلى حساس النهاية
      Serial.println("Sensor 2 Triggered: Door Fully Closed");
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      analogWrite(ENA, 0);
      currentState = STOPPED;
      client.publish(topicStatus, "CLOSED");
      delay(1000); // تأخير بسيط لمنع التكرار المستمر
    }
  }
}
