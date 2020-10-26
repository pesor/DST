#include <NTPClient.h>
#include <WiFi.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
String formattedDate;
String dayStamp;
String timeStamp1;

// define your SSID's, and remember to fill out variable ssidArrNo with the number of your SSID's
String ssidArr[] = {"Enterprise-pro", "Enterprise_EXT", "Enterprise_EXTN", "Enterprise" };
int ssidArrNo = 4;

const char* ssid = ""; // no need to fill in
const char* password = "gp44pnh6";
const char* ntpServer = "pool.ntp.org";

const long  gmtOffset_sec = 3600;

void setup() {

  Serial.begin(115200);
  Serial.println("Void Setup");

  // Start WiFi and update time
  connectToNetwork();
  Serial.println(" ");
  Serial.println("Connected to network");
  timeClient.setTimeOffset(gmtOffset_sec);
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }

  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  dayStamp = dayStamp.substring(5);
  String dateMonth = dayStamp.substring(0, 2);
  String dateDay = dayStamp.substring(3, 5);
  dayStamp = dateDay + "-" + dateMonth;
  timeStamp1 = formattedDate.substring(splitT + 1, formattedDate.length() - 1);
  //  config.time = timeStamp1.substring(0, 5);
  // variables needed for DST test
  int thisHour = timeClient.getHours();
  int thisDay = dateDay.toInt();
  int thisMonth = dateMonth.toInt();
  int thisWeekday = timeClient.getDay();
  bool dst = false;

  // Test for DST active
  if (thisMonth == 10 && thisDay < 25 && thisWeekday < 7 )  {
    dst = true;
  }

  if (thisMonth == 10 && thisDay > 24 && thisWeekday == 7 && thisHour < 2)  {
    dst = true;
  }

  if (thisMonth < 10 && thisMonth > 3) {
    dst = true;
  }

  if (thisMonth == 3) {
    dst = true;
    if (thisDay < 25) {
      dst = false;
    }
    else
      // thisDay > 25
    {
      if (thisWeekday == 7 && thisHour < 2)      {
        dst = false;
      }
      else {
        if (thisWeekday == 7) {
          dst = true;
        }
        else {
          if (thisWeekday < 7) {
            int checkSum = thisDay - thisWeekday + 7;
            if (checkSum > 31) {
              dst = true;
            }
            else {
              dst = false;
            }
          }
        }
      }
    }
  }

  if (dst) {
    Serial.println("IN SOMMERTIME");
    timeClient.setTimeOffset(gmtOffset_sec + 3600);
    while (!timeClient.update()) {
      timeClient.forceUpdate();
    }
    // The formattedDate comes with the following format:
    // 2018-05-28T16:00:13Z
    // We need to extract date and time
    formattedDate = timeClient.getFormattedDate();
    // Extract date
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    dayStamp = dayStamp.substring(5);
    String dateMonth = dayStamp.substring(0, 2);
    String dateDay = dayStamp.substring(3, 5);
    dayStamp = dateDay + "-" + dateMonth;
    timeStamp1 = formattedDate.substring(splitT + 1, formattedDate.length() - 1);
  } else {
    Serial.println("IN VINTERTIME");
  }
}

void connectToNetwork() {
  Serial.print("Size of SSID array ");
  Serial.println(ssidArrNo);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  bool breakLoop = false;
  for (int i = 0; i <= ssidArrNo; i++) {
    ssid = ssidArr[i].c_str();
    Serial.print("SSID name: ");
    Serial.print(ssidArr[i]);

    while ( WiFi.status() !=  WL_CONNECTED )
    {
      // wifi down, reconnect here
      WiFi.begin(ssid, password);
      int WLcount = 0;
      int UpCount = 0;
      while (WiFi.status() != WL_CONNECTED )
      {
        delay( 100 );
        Serial.printf(".");
        if (UpCount >= 60)  // just keep terminal from scrolling sideways
        {
          UpCount = 0;
          Serial.printf("\n");
        }
        ++UpCount;
        ++WLcount;
        if (WLcount > 200) {
          Serial.println("we should break");
          breakLoop = true;
          break;
        }
      }
      if (breakLoop) {
        breakLoop = false;
        break;
      }
    }
  }
  if (WiFi.status() !=  WL_CONNECTED) {
    return;
  }
}


void loop() {

}
