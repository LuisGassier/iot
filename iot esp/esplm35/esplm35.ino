#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>
#include <DHTesp.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

#define ADC_VREF_mV    3300.0 // in millivolt
#define ADC_RESOLUTION 4096.0
#define PIN_LM35       4

DHTesp dht;
int dhtPin = 4;

/* 1. Define the WiFi credentials */
#define WIFI_SSID "prueba"
#define WIFI_PASSWORD "12345678"

/* 2. Define the API Key */
#define API_KEY "AIzaSyCmWELbxYmzjef3-sIql8JMkriAe3NtdyI"

/* 3. Define the project ID */
#define FIREBASE_PROJECT_ID "iot-s-b84e1"

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "prueba@prueba.com"
#define USER_PASSWORD "12345678"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long dataMillis = 0;
int count = 0;
String timeV ="";
String temp = "";
int temperature;

// The Firestore payload upload callback function
void fcsUploadCallback(CFS_UploadStatusInfo info)
{
  if (info.status == fb_esp_cfs_upload_status_init)
  {
    Serial.printf("\nUploading data (%d)...\n", info.size);
  }
  else if (info.status == fb_esp_cfs_upload_status_upload)
  {
    Serial.printf("Uploaded %d%s\n", (int)info.progress, "%");
  }
  else if (info.status == fb_esp_cfs_upload_status_complete)
  {
    Serial.println("Upload completed ");
  }
  else if (info.status == fb_esp_cfs_upload_status_process_response)
  {
    Serial.print("Processing the response... ");
  }
  else if (info.status == fb_esp_cfs_upload_status_error)
  {
    Serial.printf("Upload failed, %s\n", info.errorMsg.c_str());
  }
}

void setup()
{

  Serial.begin(115200);
    dht.setup(dhtPin, DHTesp::DHT11);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

#if defined(ESP8266)
  // In ESP8266 required for BearSSL rx/tx buffer for large data handle, increase Rx size as needed.
  fbdo.setBSSLBufferSize(2048 /* Rx buffer size in bytes from 512 - 16384 */, 2048 /* Tx buffer size in bytes from 512 - 16384 */);
#endif

  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  // For sending payload callback
  // config.cfs.upload_callback = fcsUploadCallback;
}

void loop()
{
   


  // Firebase.ready() should be called repeatedly to handle authentication tasks.

  if (Firebase.ready() && (millis() - dataMillis > 60000 || dataMillis == 0))
  {
    


    dataMillis = millis();

    // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create.ino
    FirebaseJson content;

    // aa is the collection id, bb is the document id.
    String documentPath = "data/6HYpbt4pcawcmP5G1YcL";

    // If the document path contains space e.g. "a b c/d e f"
    // It should encode the space as %20 then the path will be "a%20b%20c/d%20e%20f"
    //obten la hora actual
        time_t now = time(nullptr);
        //convierte la hora actual a una cadena
        String timeV = ctime(&now);
        //elimina el salto de línea de la cadena
        timeV.replace("\n", "");
        //imprime la hora actual
        Serial.println(timeV);
        // read the ADC value from the temperature sensor
       

        // obtenemos el valor del sensor lm35
        temperature = dht.getTemperature();
        Serial.println(temperature);

    content.clear();
    content.set("fields/value/integerValue", temperature);
    content.set("fields/time/stringValue", timeV);

    Serial.print("Create a document... ");

    if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw()))
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
    else
      Serial.println(fbdo.errorReason());


    content.clear();

    content.set("fields/value/integerValue", temperature);
    content.set("fields/time/stringValue", timeV);

    Serial.print("Update a document... ");

    /** if updateMask contains the field name that exists in the remote document and
       this field name does not exist in the document (content), that field will be deleted from remote document
    */

    if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw(), "count,status" /* updateMask */))
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
    else
      Serial.println(fbdo.errorReason());
  }
}
