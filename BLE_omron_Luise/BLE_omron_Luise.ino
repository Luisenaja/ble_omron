/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

#include "BLEDevice.h"
#include "BLEScan.h"
#include <BLEUtils.h>

// The remote service we wish to connect to.
static BLEUUID serviceUUID("0c4c3000-7700-46f4-aa96-d5e974e32a54");
                         
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("0c4c3001-7700-46f4-aa96-d5e974e32a54");


static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
   
      BLEClient*  pClient  = BLEDevice::createClient();
      pClient->setClientCallbacks(new MyClientCallback());
      pClient->connect(myDevice);  
      Serial.println(" - Connected to server");

    
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
//    if (pRemoteService == nullptr) {
//      Serial.print("Failed to find our service UUID: ");
//      Serial.println(serviceUUID.toString().c_str());
//      pClient->disconnect();
//      return false;
//    }
    Serial.println(" - Found our service");


    
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
//    if (pRemoteCharacteristic == nullptr) {
//      Serial.print("Failed to find our characteristic UUID: ");
//      Serial.println(charUUID.toString().c_str());
//      pClient->disconnect();
//      return false;
//    }
    Serial.println(" - Found our characteristic");
    if(pRemoteCharacteristic->canRead()) {
      //std::string value = pRemoteCharacteristic->readValue();
      std::string value = pRemoteCharacteristic->readValue();
//      //String Target = advertisedDevice.getAddress().toString().c_str();
//      Serial.println(value.c_str());
      
      if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);     
}
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());


    BLEAddress t = BLEAddress("d2:25:6b:86:62:5c");
    Serial.print("set address: ");
    Serial.println(t.toString().c_str());

    if (advertisedDevice.getAddress().equals(t)){
      myDevice = new BLEAdvertisedDevice(advertisedDevice); 
      BLEDevice::getScan()->stop();

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);

        Serial.println();
        Serial.println("*********");
      }
    }
};




void init_scan(){
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_RX,
                      BLECharacteristic::PROPERTY_WRITE
                    );
  
  pRxCharacteristic->setCallbacks(new MyCallbacks());
  pBLEScan->setInterval(1000);
  pBLEScan->setWindow(2000);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(20, false);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  init_scan();
} 


void loop() {
   connectToServer();  
} 
