#include "BLEDevice.h"
#include <MPU6050_tockn.h>
#include <Wire.h>
#define SDA 8
#define SCL 9

MPU6050 mpu6050(Wire);
const int threshold = 40;
const int minDelay = 500;
unsigned long lastPushUpTime = 0;
bool isPushUpInProgress = false;

static BLEUUID serviceUUID("E05BEF95-2101-4878-B98A-14689699A844");
static BLEUUID charUUID("d2f5c8c9-5729-444a-9681-beaa37580f4a");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

static unsigned long sessionStartTime = 0; // Track the start time of the push-up session
static unsigned long sessionDuration = 60000; // 1 minute session duration

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
    Serial.write(pData, length);
    Serial.println();

    // Check if the received data is the reset command
    if (length == 5 && memcmp(pData, "reset", 5) == 0) {
        resetMPU6050();
    }
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {}

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    pClient->connect(myDevice);
    Serial.println(" - Connected to server");
    pClient->setMTU(517);

    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");

    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    return true;
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    }
  }
};

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA, SCL);
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop() {
  mpu6050.update();
  float z = mpu6050.getGyroX();

  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothing more we will do.");
    }
    doConnect = false;
  }

  if (connected) {
    if (!isPushUpInProgress && z > threshold) {
      isPushUpInProgress = true;
      Serial.println("Push Up Dimulai");
      Serial.print("z : ");
      Serial.println(z);
      lastPushUpTime = millis();

      if (connected && pRemoteCharacteristic != nullptr) {
        pRemoteCharacteristic->writeValue((uint8_t*) "0", 1);
      }
      // if (connected && pRemoteCharacteristic != nullptr) {
      //   pRemoteCharacteristic->writeValue((uint8_t*) "1", 1);
      // }
    } else if (isPushUpInProgress && z < -threshold && millis() - lastPushUpTime > minDelay) {
      isPushUpInProgress = false;
      Serial.println("Push Up Selesai");
      // hitungPushUp();
      if (connected && pRemoteCharacteristic != nullptr) {
        pRemoteCharacteristic->writeValue((uint8_t*) "1", 1);
      }
      // if (connected && pRemoteCharacteristic != nullptr) {
      //   pRemoteCharacteristic->writeValue((uint8_t*) "0", 1);
      // }
    }
  } else if (doScan) {
    BLEDevice::getScan()->start(0);
  }
  Serial.println(z);
  delay(100);
}

void hitungPushUp() {
    static int pushUpCount = 0;
    pushUpCount++;
    Serial.print("Jumlah Push Up: ");
    Serial.println(pushUpCount);

    // if (connected && pRemoteCharacteristic != nullptr) {
    //     String pushUpStr = String(pushUpCount);
    //     pRemoteCharacteristic->writeValue((uint8_t*)pushUpStr.c_str(), pushUpStr.length());
    // }
}

void resetMPU6050() {
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x80);
  Wire.endTransmission(false);
  delay(100);

  mpu6050.begin();
  Serial.println("MPU6050 has been reset.");
}
