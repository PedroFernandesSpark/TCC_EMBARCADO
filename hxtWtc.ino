#include <Adafruit_AHTX0.h>
#include <MAX30105.h>
#include <Wire.h>
#include <GyverOLED.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

GyverOLED<SSH1106_128x64> oled;
MAX30105 particleSensor;

float t = 0.0, h = 0.0, pressure = 0.0;
int buttonPin = 0;
bool deviceConnected = false;

unsigned long previousMillis = 0;
const long interval = 100;

#define IR_THRESHOLD 20000
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define HEART_RATE_UUID "e5a1d466-344c-4be3-ab3f-189f80dd7518"

BLEServer* pServer = nullptr;
BLECharacteristic* pHeartRateCharacteristic = nullptr;

int bufferSize = 5;
long* hrBuffer = nullptr;
int bufferIndex = 0;
bool bufferFilled = false;
bool frequenciaRecebida = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) { deviceConnected = true; }
    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        frequenciaRecebida = false;
        if (hrBuffer) delete[] hrBuffer;
        hrBuffer = nullptr;
        pServer->startAdvertising();
    }
};

void setup() {
    Serial.begin(115200);
    BLEDevice::init("TCC_WATCH");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService* pService = pServer->createService(SERVICE_UUID);
    pHeartRateCharacteristic = pService->createCharacteristic(
                                  HEART_RATE_UUID,
                                  BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE);
    pHeartRateCharacteristic->addDescriptor(new BLE2902());
    pService->start();

    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    BLEDevice::startAdvertising();
    Serial.println("Aguardando conexão BLE...");

    oled.init();
    oled.clear();
    oled.setScale(2);
    oled.setCursor(18, 2);
    oled.print("TCC");
    oled.setCursor(22, 5);
    oled.print("WATCH");
    oled.update();
    delay(3000);

    initSensors();
    pinMode(buttonPin, INPUT);
}

void loop() {
    if (!deviceConnected) {
        deviceConnected = pServer->getConnectedCount() > 0;
        if (!deviceConnected) {
            displayMessage("Aguardando conexão BLE...");
            Serial.println("Aguardando conexão BLE...");
            delay(500);
        }
        return;
    }

    if (!frequenciaRecebida) {
      displayMessage("CONECTADO");
      Serial.println("CONECTADO");
      delay(500);
        String rxData = pHeartRateCharacteristic->getValue();

        if (rxData.length() > 0) {
            int freq = atoi(rxData.c_str());
            if (freq > 0) {
                bufferSize = max(1, freq / 10);
                if (hrBuffer) delete[] hrBuffer;
                hrBuffer = new long[bufferSize];
                bufferIndex = 0;
                bufferFilled = false;
                frequenciaRecebida = true;
                Serial.print("Frequência recebida via BLE: ");
                Serial.println(freq);
                Serial.print("Buffer ajustado para: ");
                Serial.println(bufferSize);
                displayMessage("Freq recebida!");
                delay(500);
                displayMessage("Enviando Dados");
                Serial.println("Enviando Dados");
            }
        }
        delay(100);
        return;
    }

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        long irValue = particleSensor.getIR();
        hrBuffer[bufferIndex] = irValue;
        bufferIndex = (bufferIndex + 1) % bufferSize;
        if (bufferIndex == 0) bufferFilled = true;

        if (bufferFilled) {
            char data[150] = "";
            for (int i = 0; i < bufferSize; i++) {
                char temp[12];
                itoa(hrBuffer[(bufferIndex + i) % bufferSize], temp, 10);
                strcat(data, temp);
                if (i < bufferSize - 1) strcat(data, ",");
            }
            pHeartRateCharacteristic->setValue((uint8_t*)data, strlen(data));
            pHeartRateCharacteristic->notify();
            Serial.print("Enviado: ");
            Serial.println(data);
        }
    }
}

void initSensors() {
    // while (!aht.begin()) {
    //     displayError("AHTX0 Error", "Reconnecting...");
    //     delay(2000);
    // }
    // displayMessage("AHTX0 Connected");

    while (!particleSensor.begin()) {
        displayError("MAX30102 Error!", "Reconnecting...");
        delay(2000);
    }
    displayMessage("MAX30102 Connected");
    particleSensor.setup();
    particleSensor.setPulseAmplitudeRed(0x0A);

    // while (!bmp.begin(0x76)) {
    //     displayError("BMP280 Error", "Reconnecting...");
    //     delay(2000);
    // }
    // displayMessage("BMP280 Connected");
}

void displayMessage(const char* message) {
    oled.clear();
    oled.setScale(1);
    oled.setCursor(10, 1);
    oled.print(message);
    oled.update();
}

void displayError(const char* title, const char* description) {
    oled.clear();
    oled.setScale(1);
    oled.setCursor(10, 1);
    oled.print(title);
    oled.setCursor(10, 3);
    oled.print(description);
    oled.update();
}