/*

  Based on Tweeter.ino by Sandeep Mistry
  https://github.com/arduino-libraries/Arduino_OAuth/blob/master/examples/Tweeter/Tweeter.ino
  
*/

#include "arduino_secrets.h"
#include <ArduinoECCX08.h>     // ArduinoBearSSL depends on ArduinoECCX08
#include <ArduinoBearSSL.h>    // Arduino_OAuth depends on ArduinoBearSSL
#include <ArduinoHttpClient.h> // Arduino_OAuth depends on ArduinoHttpClient
#include <Arduino_OAuth.h>
#include <ArduinoJson.h>
//#include <string.h>
#include <WiFiNINA.h>

const char ssid[] = SECRET_SSID;    // your network SSID (name)
const char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

const char consumerKey[]       = SECRET_CONSUMER_KEY;
const char consumerKeySecret[] = SECRET_CONSUMER_KEY_SECRET;
const char accessToken[]       = SECRET_ACCESS_TOKEN;
const char accessTokenSecret[] = SECRET_ACCESS_TOKEN_SECRET;

int status = WL_IDLE_STATUS;

WiFiSSLClient wifiSSLClient;
WiFiClient wifiClient;
HttpClient client = HttpClient(wifiClient, "your.site", 80);
OAuthClient oauthClient(wifiSSLClient, "api.twitter.com", 443);

String song[] = {"song_1", "song_2", "song_3"};

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Waiting for the network time to sync ");
  while (getTime() == 0) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.println();

  // assign the OAuth credentials
  oauthClient.setCredentials(consumerKey, consumerKeySecret, accessToken, accessTokenSecret);
  oauthClient.onGetTime(getTime);
}

void loop() {
  String songTweet;

  int randomInt = random(10);
  String titleSong = song[randomInt];
  titleSong.toLowerCase();
  titleSong += ".json";
  Serial.println(titleSong);
  // Path to lyrics json files
  client.get("/lyrics/" + titleSong);

  int songStatusCode = client.responseStatusCode();
  String songResponse = client.responseBody();
  //songResponse.remove(0,10);

  if (songStatusCode != 200) {
    Serial.println(songStatusCode);
    Serial.println(songResponse);
  } else {
    // Parse JSON response
    StaticJsonDocument<2048> jsonSong;
    deserializeJson(jsonSong, songResponse);
    String songLyrics = jsonSong["lyrics"];
    Serial.println(songLyrics);
    int lyricsLength = songLyrics.length() - 180;
    randomInt = random(lyricsLength);
    int i = 0, j = 0, k = 0;
    while (i < 170) {
      while (j == 0 and songLyrics[randomInt] != '\n') {
        Serial.println(songLyrics[randomInt]);
        randomInt += 1;
      }
      while (j == 0 and songLyrics[randomInt] == '\n') {
        Serial.println(songLyrics[randomInt]);
        randomInt += 1;
      }
      j = 1;
      if (i > 120 and songLyrics[randomInt] == '\n') {
        songTweet += songLyrics[randomInt];
        break;
      }
      songTweet += songLyrics[randomInt];
      i += 1;
      randomInt += 1;
    }
    // Removing comma if it is the last char
    int tweetLength = songTweet.length();
    tweetLength -= 1;
    while (songTweet[tweetLength] != '\n') {
      songTweet.remove(tweetLength);
      tweetLength -= 1;
    }
    tweetLength = songTweet.length();
    tweetLength -= 2;
    if (songTweet[tweetLength] == ',') {
      songTweet.remove(tweetLength);
      songTweet += '\n';
    }
  }

  int tweetStatusCode = 0;
  if (songStatusCode == 200) {
    tweetStatusCode = tweet(songTweet);
    while (tweetStatusCode != 200) {
      delay(30000);
      tweetStatusCode = tweet(songTweet);
    }
  }
  
  Serial.println("Tweet inviato, forse");

  // wait one hour before tweeting again
  delay(3600 * 1000L);
}

unsigned long getTime() {
  return WiFi.getTime();
}

int tweet(String text) {
  Serial.println("Sending tweet: ");
  Serial.println(text);

  String requestBody;

  requestBody += "status=";
  requestBody += URLEncoder.encode(text);

  // HTTP POST it via the OAuth client, which sets the Authorization header for us
  oauthClient.post("/1.1/statuses/update.json", "application/x-www-form-urlencoded", requestBody);

  int statusCode = oauthClient.responseStatusCode();

  Serial.print("statusCode = ");
  Serial.println(statusCode);
  Serial.println();
  return statusCode;
}
