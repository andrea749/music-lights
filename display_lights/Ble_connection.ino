#include <ArduinoBLE.h>

int buttonPin = 2;
boolean ledSwitch;

BLEService LEDService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service
// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
// BLEByteCharacteristic LEDCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify | BLEWrite);
BLEStringCharacteristic LEDCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify | BLEWrite);
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
  }
  // set advertised local name and service UUID:
  BLE.setLocalName("ARDUINO");
  BLE.setDeviceName("ARDUINO");
  BLE.setAdvertisedService(LEDService);
  // add the characteristic to the service
  LEDService.addCharacteristic(LEDCharacteristic);
  // add service
  BLE.addService(LEDService);
  // start advertising
  BLE.advertise();
  Serial.println("BLE LED Peripheral, waiting for connections....");
}
void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();
  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    // while the central is still connected to peripheral:
    while (central.connected()) {

      // int buttonState = digitalRead(buttonPin);
      // int ledSwitch = LEDCharacteristic.read();
      // if (ledSwitch) {
      //     Serial.println("ON");
      //     LEDCharacteristic.writeValue((byte)0x01);
      //   }
      //   else {
      //     LEDCharacteristic.writeValue((byte)0x00);
      //     Serial.println("OFF");
      //   }
      // }
      // if (LEDCharacteristic.canRead()) { ledCharacteristic.written()
      if (LEDCharacteristic.written()) {
        Serial.println("Reading...");
        // byte value = LEDCharacteristic.read();
        LEDCharacteristic.read();
        if (LEDCharacteristic.valueLength() > 0) {
          Serial.println("length");
          // std::string str = std::to_string(LEDCharacteristic.valueLength());
          Serial.println(LEDCharacteristic.valueLength());
          // LEDCharacteristic.readValue();
        }
        byte Array[20] value = [];
        LEDCharacteristic.readValue(value);
        if (value > 0) {
          Serial.println("length");
          // std::string str = std::to_string(LEDCharacteristic.valueLength());
          Serial.println(value);
          // LEDCharacteristic.readValue();
        }
        // Serial.println(std::to_string(LEDCharacteristic.readValue(value)));
        if (value == 0x01) {
          Serial.println("ON");
          digitalWrite(LED_BUILTIN, HIGH);
        }
        else if (value == 0x00) {
          digitalWrite(LED_BUILTIN, LOW);
          Serial.println("OFF");
        }
    }
    delay(500);
  }
}
}
