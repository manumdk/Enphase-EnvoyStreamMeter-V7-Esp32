#ifndef CONFIG
#define CONFIG

#define DEBUG true
//#define configMAX_PRIORITIES 1024
#define DEBUG_SERIAL Serial

/**
 * The name of this device (as defined in the AWS IOT console).
 * Also used to set the hostname on the network
 */
#define DEVICE_NAME "Esp32-LectureV7"

#define WIFI_TIMEOUT 30000 // 20 seconds

/**
 * How long should we wait after a failed WiFi connection
 * before trying to set one up again.
 */
#define WIFI_RECOVER_TIME_MS 30000 // 20 seconds


// Check which core Arduino is running on. This is done because updating the 
// display only works from the Arduino core.
#if CONFIG_FREERTOS_UNICORE
    #define ARDUINO_RUNNING_CORE 0
#else
  //  #define ARDUINO_RUNNING_CORE 1
#endif




#endif


///