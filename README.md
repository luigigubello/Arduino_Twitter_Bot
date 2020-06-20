# Arduino Twitter Bot
A lyrics Twitter bot using **Arduino Nano 33 IoT** or **Arduino MKR WiFi 1010** ♾️

## Usage
In `arduino_secret.h` set your Twitter API tokens and your Wi-Fi credentials. Create HTTP endpoint and place here your lyrics JSON files.

Edit these lines:
* `line 28`: edit `your.site` entering your domain
* `line 31`: put your song titles using the same JSON filenames
* `line 74`: edit `/lyrics/` entering JSON files path

The JSON filenames have to be lowercase, see `line 71`. You can use HTTPS endpoint just editing lines `27` and `28`.</br>
**JSON formatting**: `{"lyrics":"Your lyrics\nhere"}`
