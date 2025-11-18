# HEXTECH_WATCH – Firmware ESP32 para Monitoramento PPG via BLE

Firmware para um dispositivo vestível baseado em ESP32 que:

- Usa o sensor óptico **MAX30102/MAX30105** para adquirir o sinal PPG (canal IR);
- Exibe mensagens de status em um display **OLED 128x64 (SSH1106)** via `GyverOLED`;
- Inicializa sensores **AHTX0** (temperatura/umidade) e **BMP280** (pressão atmosférica);
- Envia, via **Bluetooth Low Energy (BLE)**, **janelas de dados brutos IR** em formato CSV para um aplicativo móvel ou outro cliente BLE;
- Permite que o **cliente configure a “frequência”** (parâmetro que ajusta o tamanho do buffer de amostras enviados).

---

## ✨ Funcionalidades

- Inicialização e verificação dos sensores:
  - `Adafruit_AHTX0` – temperatura e umidade;
  - `MAX30105` – utilizado aqui como leitor PPG (IR);
  - `Adafruit_BMP280` – pressão atmosférica.
- Display OLED:
  - Tela de boot com texto **HEXTECH WATCH**;
  - Mensagens de status: conexão, erros de sensor, aguardo de frequência, envio de dados.
- BLE:
  - Serviço BLE com UUID: `4fafc201-1fb5-459e-8fcc-c5c9c331914b`;
  - Característica para dados de frequência cardíaca/sinal PPG:
    - UUID: `e5a1d466-344c-4be3-ab3f-189f80dd7518`;
    - Propriedades: `READ`, `WRITE`, `NOTIFY`.
  - Reconexão automática após desconexão.

---

## 🧱 Hardware Necessário

- **ESP32 DevKit** (ou equivalente com BLE embarcado);
- **Sensor PPG**: módulo **MAX30102/MAX30105** (I²C);
- **Display OLED 128x64 SSH1106** (I²C, usado com `GyverOLED`);
- Botão físico (opcional, ligado ao pino `0` no código, ainda não usado na lógica principal);
- Protoboard, jumpers, fonte de alimentação adequada (USB ou bateria).

### Ligações (sugestão genérica)

> Ajuste conforme seu módulo/placa. Abaixo é o padrão para muitos ESP32 DevKit.

- **I²C (sensores e OLED)**
  - SDA → GPIO 21 (ESP32 padrão)
  - SCL → GPIO 22 (ESP32 padrão)
  - VCC → 3V3
  - GND → GND

- **Botão**
  - Um lado → GPIO 0 (`buttonPin = 0`)
  - Outro lado → GND (use `pinMode(buttonPin, INPUT_PULLUP)` se for alterar o firmware para ler o botão com pull-up interno).

---

## 📚 Bibliotecas Utilizadas

Certifique-se de instalar estas bibliotecas na Arduino IDE ou no seu `platformio.ini`:

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
