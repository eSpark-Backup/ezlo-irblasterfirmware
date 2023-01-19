#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>


BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
bool ir_cmd = false;
uint8_t txValue = 0;
const uint16_t kIrLed = 18;

IRsend irsend(kIrLed);

uint16_t onAC[419] = {9058, 4474,  664, 1644,  664, 566,  638, 568,  638, 1644,  662, 1644,  662, 1644,  662, 568,  638, 568,  636, 568,  638, 568,  636, 568,  636, 1644,  662, 568,  638, 568,  638, 568,  638, 568,  636, 568,  638, 568,  636, 568,  636, 570,  636, 568,  638, 1644,  662, 1646,  662, 568,  636, 570,  636, 570,  636, 570,  636, 570,  636, 1644,  662, 568,  636, 1646,  662, 568,  636, 568,  636, 1670,  638, 568,  638, 19974,  664, 1644,  664, 566,  638, 1644,  664, 568,  638, 1642,  664, 568,  640, 1644,  664, 566,  638, 566,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  636, 1644,  662, 1644,  662, 568,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  636, 568,  638, 568,  636, 568,  636, 570,  636, 568,  636, 570,  638, 1644,  662, 1646,  662, 39960,  9026, 4474,  664, 1644,  664, 568,  638, 568,  638, 1644,  664, 1644,  664, 1644,  662, 568,  638, 568,  636, 568,  638, 568,  638, 568,  638, 1642,  664, 568,  636, 568,  636, 568,  636, 570,  636, 570,  636, 568,  636, 570,  636, 570,  636, 570,  636, 1646,  662, 1646,  660, 570,  636, 570,  636, 570,  634, 570,  636, 570,  636, 1670,  638, 1670,  638, 1670,  636, 570,  636, 568,  636, 1670,  638, 570,  636, 19976,  664, 566,  638, 568,  638, 566,  638, 568,  638, 568,  638, 566,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  636, 568,  638, 568,  636, 568,  638, 568,  636, 568,  636, 1644,  662, 568,  636, 1646,  662, 568,  636, 568,  636, 570,  636, 568,  636, 570,  636, 570,  636, 570,  636, 568,  636, 570,  636, 39984,  9006, 4476,  664, 568,  638, 568,  638, 568,  638, 568,  636, 568,  638, 568,  636, 568,  636, 568,  636, 568,  638, 568,  636, 568,  636, 568,  638, 568,  636, 570,  636, 570,  636, 570,  636, 568,  636, 570,  636, 570,  636, 568,  636, 568,  636, 570,  636, 570,  636, 568,  636, 570,  636, 570,  636, 570,  636, 570,  636, 572,  634, 1668,  638, 570,  636, 1670,  638, 570,  636, 1670,  638, 570,  636, 19974,  664, 566,  638, 566,  638, 568,  638, 566,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  638, 566,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  636, 568,  638, 568,  638, 568,  636, 568,  636, 570,  636, 568,  636, 568,  636, 568,  638, 568,  636, 570,  636, 570,  636, 568,  636, 1646,  662, 568,  636, 1646,  662};

uint16_t offAC[419] = {9028, 4476,  662, 1644,  664, 568,  638, 568,  636, 568,  638, 1644,  662, 1644,  664, 568,  638, 568,  638, 570,  636, 568,  636, 568,  638, 1644,  662, 568,  636, 568,  638, 568,  638, 568,  636, 568,  636, 570,  636, 570,  636, 570,  636, 568,  638, 1646,  662, 570,  636, 570,  636, 570,  636, 570,  636, 570,  636, 570,  636, 1646,  662, 568,  636, 1670,  638, 570,  636, 570,  660, 1646,  662, 546,  636, 19976,  666, 1642,  664, 566,  640, 1644,  664, 568,  638, 1642,  664, 566,  638, 1644,  664, 566,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  636, 568,  638, 1644,  662, 1644,  662, 568,  638, 568,  636, 570,  636, 568,  638, 568,  636, 568,  636, 568,  638, 568,  636, 570,  636, 568,  636, 568,  636, 570,  636, 570,  636, 570,  636, 1646,  662, 568,  636, 39986,  9006, 4476,  664, 1644,  664, 568,  638, 568,  638, 568,  638, 1644,  662, 1644,  662, 568,  638, 568,  636, 568,  636, 570,  636, 568,  638, 1644,  662, 568,  636, 570,  636, 568,  636, 570,  636, 570,  636, 570,  636, 570,  636, 570,  636, 570,  636, 1670,  638, 570,  636, 570,  636, 570,  636, 570,  636, 570,  634, 570,  636, 1670,  638, 1670,  638, 1668,  638, 570,  634, 570,  634, 1670,  636, 570,  634, 19976,  664, 568,  638, 568,  638, 566,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  636, 568,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  636, 568,  638, 568,  636, 568,  636, 568,  638, 568,  636, 1670,  638, 568,  636, 1670,  638, 570,  636, 570,  636, 570,  636, 570,  636, 570,  636, 570,  636, 570,  636, 570,  636, 1670,  636, 39986,  9006, 4476,  664, 566,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  636, 568,  638, 568,  638, 568,  638, 568,  638, 568,  638, 568,  636, 568,  638, 568,  636, 570,  636, 568,  636, 570,  636, 570,  636, 570,  636, 570,  636, 568,  636, 570,  636, 568,  636, 570,  636, 570,  636, 570,  636, 568,  636, 570,  636, 570,  636, 1672,  636, 572,  634, 1672,  636, 570,  634, 1672,  636, 594,  612, 19974,  664, 568,  638, 568,  638, 568,  638, 570,  636, 568,  638, 566,  638, 568,  638, 568,  638, 568,  636, 568,  636, 570,  636, 568,  638, 568,  636, 568,  638, 568,  638, 568,  638, 568,  636, 570,  638, 568,  636, 568,  636, 570,  636, 570,  636, 570,  636, 568,  636, 570,  636, 570,  636, 570,  636, 570,  636, 570,  636, 1672,  636, 570,  636, 1670,  636};

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
        }
        
        switch (rxValue[0]) {
          case '0':
          irsend.sendRaw(offAC, 419, 38);
          Serial.println("OFF");
          break;
          
          case '1':
          irsend.sendRaw(onAC, 419, 38);
          Serial.println("ON");
          break;
        }
      }
    }
};


void setup() {
  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("UART Service");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                    CHARACTERISTIC_UUID_TX,
                    BLECharacteristic::PROPERTY_NOTIFY
                  );
                      
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
                       CHARACTERISTIC_UUID_RX,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
  
  irsend.begin();
}

void loop() {
  
    if (deviceConnected) {
        //pTxCharacteristic->setValue(&txValue, 1);
        //pTxCharacteristic->notify();
        //txValue++;
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent
  }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
