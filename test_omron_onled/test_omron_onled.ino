#include <BLEDevice.h> //Header file for BLE 


static BLEUUID serviceUUID("0c4c3000-7700-46f4-aa96-d5e974e32a54"); 
static BLEUUID    charUUID("0c4c3003-7700-46f4-aa96-d5e974e32a54");  
String My_BLE_Address = "d2:25:6b:86:62:5c"; 




static BLERemoteCharacteristic* pRemoteCharacteristic;

BLEScan* pBLEScan; //Name the scanning device as pBLEScan
BLEScanResults foundDevices;

static BLEAddress *Server_BLE_Address;
String Scaned_BLE_Address;

boolean paired = false; //boolean variable to togge light




bool connectToserver (BLEAddress pAddress){
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");
    Serial.println("---------------------------------");


    pClient->connect(pAddress);
    Serial.println(" - Connected to fitnessband");



    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService != nullptr)
    { 
      std::string value = pRemoteCharacteristic->readValue();
      Serial.println(" - Found our service");
      Serial.println(value.c_str());
      return true;
    }
    else
    return false;


    if (pRemoteCharacteristic != nullptr)
      Serial.println(" - Found our characteristic");

      return true;
}





class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks 
{
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.printf("Scan Result: %s \n", advertisedDevice.toString().c_str());
      Server_BLE_Address = new BLEAddress(advertisedDevice.getAddress());
      Scaned_BLE_Address = Server_BLE_Address->toString().c_str();
    }
};





void setup() {
    Serial.begin(115200); //Start serial monitor 
    Serial.println("ESP32 BLE Server program"); //Intro message 

    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); //Call the class that is defined above 
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster

    pinMode (LED_BUILTIN, OUTPUT); //Declare the in-built LED pin as output 

}

void loop() {

  foundDevices = pBLEScan->start(5); //Scan for 3 seconds to find the Fitness band 

  while (foundDevices.getCount() >= 1)
  {
    if (Scaned_BLE_Address == My_BLE_Address or paired == false)
    {
      Serial.println("Found Device :-)... connecting to Server as client");
      ///std::string value = pRemoteCharacteristic->readValue();
      Serial.println("toooooo dis   looop ");
      //Serial.println(value.c_str());
       
       
       
       if (connectToserver(*Server_BLE_Address))
      {
      paired = true;
      Serial.println("********************LED turned ON************************");
      digitalWrite (LED_BUILTIN,HIGH);
      break;
      }
      else
      {
      Serial.println("Pairing failed");
      break;
      }
    }
    
    if (Scaned_BLE_Address == My_BLE_Address or paired == true)
    {
      Serial.println("Our device went out of range");
      paired = false;
      Serial.println("********************LED OOOFFFFF************************");
      digitalWrite (LED_BUILTIN,LOW);
      ESP.restart();
      break;
    }
    else
    {
    Serial.println("We have some other BLe device in range");
    break;
    }
  } 
}
