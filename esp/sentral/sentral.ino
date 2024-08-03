#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID "E05BEF95-2101-4878-B98A-14689699A844"
#define CHARACTERISTIC_UUID_TX1 "d2f5c8c9-5729-444a-9681-beaa37580f4a"
#define CHARACTERISTIC_UUID_TX2 "2122b241-dc8b-4673-af63-19ba55fc97ec"
#define CHARACTERISTIC_UUID_RX "1d5616fb-de0a-4354-b680-d291333dc25a"

BLECharacteristic *pTxCharacteristic1;
BLECharacteristic *pTxCharacteristic2;
BLECharacteristic *pRxCharacteristic;
BLEServer *pServer;

int numConnections = 0;
int pushUpCount = 0;
int incorrectPushUp = 0;
bool client1Notified = false;
bool client2Notified = false;
unsigned long client1Timestamp = 0;
unsigned long client2Timestamp = 0;
bool bothClientsConnected = false;

void checkPushUpCount() {
  if (numConnections >= 2 && client1Notified == true && client2Notified == true) {
    unsigned long timeDiff = client1Timestamp > client2Timestamp ? client1Timestamp - client2Timestamp : client2Timestamp - client1Timestamp;
    Serial.print("Time difference: ");
    Serial.println(timeDiff);
    if (timeDiff <= 500) {  // Notifications within 1 second
      pushUpCount++;
      Serial.print("Both clients notified with 'true' within 1 second. Push up count: ");
      Serial.println(pushUpCount);

      // Send notification '1' to RX characteristic
      pRxCharacteristic->setValue("1");
      pRxCharacteristic->notify();
    } else {
      incorrectPushUp++;
      Serial.print("Clients notified with 'true' but not within 1 second. Incorrect movement count: ");
      Serial.println(incorrectPushUp);

      // Send notification '1' to RX characteristic
      pRxCharacteristic->setValue("2");
      pRxCharacteristic->notify();

      // Send notifications to both clients
      pTxCharacteristic1->setValue("2");
      pTxCharacteristic1->notify();
      pTxCharacteristic2->setValue("2");
      pTxCharacteristic2->notify();
    }

    // Reset notification flags
    client1Notified = false;
    client2Notified = false;
  }
}

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    numConnections++;
    Serial.print("Client connected. Total connections: ");
    Serial.println(numConnections);
    if (numConnections >= 2) {
      bothClientsConnected = true;
      Serial.println("Both clients connected. Ready to start counting push-ups.");
    }
    if (numConnections < 3) {
      pServer->startAdvertising();
      Serial.println("Advertising restarted.");
    }
  }

  void onDisconnect(BLEServer *pServer) {
    numConnections--;
    Serial.print("Client disconnected. Total connections: ");
    Serial.println(numConnections);
    bothClientsConnected = false;
  }
};

class TxCallbacks1 : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
      Serial.print("Client 1 value received: ");
      Serial.println(value.c_str()); // Menampilkan isi nilai value
      if (value[0] == '1') {
        client1Notified = true;
        client1Timestamp = millis();
        Serial.println("Client 1 notified with 'true'");
      } else {
        client1Notified = false;
        Serial.println("Client 1 notified with 'false'");
      }
      checkPushUpCount();
    }
  }
};

class TxCallbacks2 : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
      Serial.print("Client 2 value received: ");
      Serial.println(value.c_str()); // Menampilkan isi nilai value
      if (value[0] == '1') {
        client2Notified = true;
        client2Timestamp = millis();
        Serial.println("Client 2 notified with 'true'");
      } else {
        client2Notified = false;
        Serial.println("Client 2 notified with 'false'");
      }
      checkPushUpCount();
    }
  }
};

class RxCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
      if (value == "1") {
        Serial.println("Received '1' from Django. Sending notify '0'.");

        // Send notify '0' to RX characteristic
        pRxCharacteristic->setValue("0");
        pRxCharacteristic->notify();
      } else if (value == "reset") {
        Serial.println("Received 'reset' from Django. Resetting TX1 and TX2.");

        // Reset TX1 and TX2 characteristics
        pTxCharacteristic1->setValue("reset");
        pTxCharacteristic1->notify();
        pTxCharacteristic2->setValue("reset");
        pTxCharacteristic2->notify();
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("Server");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pTxCharacteristic1 = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX1,
    BLECharacteristic::PROPERTY_READ | 
    BLECharacteristic::PROPERTY_WRITE | 
    BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic1->setCallbacks(new TxCallbacks1());

  pTxCharacteristic2 = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX2,
    BLECharacteristic::PROPERTY_READ | 
    BLECharacteristic::PROPERTY_WRITE | 
    BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic2->setCallbacks(new TxCallbacks2());

  pRxCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    BLECharacteristic::PROPERTY_WRITE | 
    BLECharacteristic::PROPERTY_NOTIFY);
  pRxCharacteristic->setCallbacks(new RxCallbacks());

  pService->start();
  pServer->getAdvertising()->addServiceUUID(SERVICE_UUID);
  pServer->startAdvertising();
  Serial.println("Server started advertising");
}

void loop() {
  delay(100);
}
