# HEXTECH_WATCH – Firmware ESP32 para Aquisição PPG via BLE

Este repositório contém o firmware para um dispositivo vestível baseado em **ESP32 DevKit**, projetado para a aquisição de sinais fotopletismográficos (PPG) utilizando o sensor **MAX30102/MAX30105**, com transmissão dos dados brutos via **Bluetooth Low Energy (BLE)** e exibição de status por um **display OLED SSH1106 (128×64)**.

O objetivo principal deste projeto é fornecer uma plataforma aberta e reprodutível para experimentação científica, permitindo acesso direto às amostras brutas do sensor óptico e parametrização completa da aquisição.

---

## 1. Funcionalidades

- Leitura contínua do sensor óptico **MAX30102/MAX30105**.
- Exibição de informações de status em display OLED (SSH1106).
- Leitura de sensores ambientais opcionais:
  - **AHTX0** – temperatura e umidade.
  - **BMP280** – pressão.
- Transmissão dos dados via **Bluetooth Low Energy (BLE)**:
  - Serviço BLE: `4fafc201-1fb5-459e-8fcc-c5c9c331914b`
  - Característica de envio de dados PPG: `e5a1d466-344c-4be3-ab3f-189f80dd7518`
  - Suporte a leitura, escrita e notificação.
- Envio de **janelas de dados brutos IR** em formato CSV.
- Parametrização remota da “frequência”, ajustando o tamanho do buffer enviado.

---

## 2. Hardware Necessário

- ESP32 DevKit (WROOM/DOIT ou equivalente)
- Sensor óptico **MAX30102** ou **MAX30105**
- Display **OLED SSH1106 128×64** (I²C)
- Bateria Li-Po 3.7 V (recomendado 800–1000 mAh)
- Protoboard ou placa de circuito
- Jumpers / fios de conexão

---

## 3. Esquemático do Sistema

O circuito utilizado no protótipo está ilustrado abaixo:

![Esquemático do Circuito](fbabb5fa-898a-45c0-8f32-a7931c454cc2.png)

---

## 4. Conexões do Hardware

A comunicação entre o ESP32, o MAX30102 e o display OLED ocorre via barramento I²C.

### 4.1. Ligações do MAX30102

| MAX30102 | ESP32 |
|----------|--------|
| VIN      | 3V3    |
| GND      | GND    |
| SDA      | GPIO 21 |
| SCL      | GPIO 22 |

Observações:
- O MAX30102 integra LEDs, fotodiodo, TIA, cancelamento de luz ambiente e ADC interno.
- O sensor deve ficar em contato direto com a pele; vibrações prejudicam o sinal.
- Cabos I²C devem ser mantidos curtos para reduzir interferências.

### 4.2. Ligações do Display OLED

| OLED | ESP32 |
|------|--------|
| VDD  | 3V3    |
| GND  | GND    |
| SDA  | GPIO 21 |
| SCL  | GPIO 22 |

O display e o sensor compartilham o mesmo barramento I²C.

### 4.3. Ligações da Bateria

| Bateria | ESP32 |
|---------|--------|
| VCC     | VIN (regulador) |
| GND     | GND |

---

## 5. Considerações de Montagem

- O sensor MAX30102 deve ser instalado na parte inferior do dispositivo, voltado para a pele.
- O ESP32 deve ficar distante do sensor para reduzir ruído eletromagnético.
- O display OLED deve estar na parte frontal para facilitar a visualização.
- A bateria deve ser fixada de forma segura, evitando contato com o regulador de tensão do ESP32, que aquece durante a operação.
- Recomenda-se a montagem final com solda, evitando a instabilidade de protoboard.

---

## 6. Bibliotecas Utilizadas

O firmware utiliza as seguintes bibliotecas:

```cpp
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
