# Smart-dustbin
# Smart Dustbin (ESP8266 + Blynk)

A dual-compartment smart dustbin built on an ESP8266 (NodeMCU). Each compartment has an IR sensor that detects a hand or object and automatically opens the lid with a servo, and an ultrasonic sensor that measures how full the bin is. Fill levels are sent live to the Blynk IoT app.

##   Features

- **Touchless lids:** IR sensors trigger the servos to open each lid for 5 seconds, then close it.
- **Fill-level monitoring:** Ultrasonic sensors measure the distance to the waste and convert it to a 0-100% fill level.
- **Remote dashboard:** Fill levels are pushed to Blynk virtual pins `V0` and `V1`.
- **Serial debugging:** Distances, fill levels and IR states are printed at 115200 baud.

## Hardware Required

| Component | Qty |
|---|---|
| ESP8266 NodeMCU (or compatible) | 1 |
| HC-SR04 ultrasonic sensor | 2 |
| IR obstacle sensor module | 2 |
| SG90 / MG90 servo motor | 2 |
| External 5V supply for servos (recommended) | 1 |
| Jumper wires, breadboard | - |

## Pin Connections

| Function | NodeMCU Pin |
|---|---|
| Ultrasonic 1 - TRIG | D5 |
| Ultrasonic 1 - ECHO | D6 |
| Ultrasonic 2 - TRIG | D7 |
| Ultrasonic 2 - ECHO | D8 |
| IR Sensor 1 (output) | D1 |
| IR Sensor 2 (output) | D2 |
| Servo 1 (signal) | D0 |
| Servo 2 (signal) | D4 |

Power the sensors from 5V/3V3 as per their specs and connect all grounds together (ESP8266, servos, sensors, external supply).

## Software Requirements

- [Arduino IDE](https://www.arduino.cc/en/software)
- ESP8266 board package (Boards Manager URL: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`)
- Libraries (install via Library Manager):
  - **Blynk** (provides `BlynkSimpleEsp8266.h`)
  - **Servo** (ESP8266 core includes it)

## Blynk Setup

1. Create a template in [Blynk Console](https://blynk.cloud) and a device from it.
2. Add two datastreams:
   - `V0` - Integer, 0-100 (Bin 1 fill level)
   - `V1` - Integer, 0-100 (Bin 2 fill level)
3. Add widgets (e.g., Gauge or Level) to display `V0` and `V1`.
4. Copy your **Template ID**, **Template Name** and **Auth Token**.

## Configuration

Edit the top of the sketch with your own values:

```cpp
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"

char auth[] = "YOUR_AUTH_TOKEN";
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";
```

> **Security note:** Never commit your real Auth Token or WiFi password to a public repository. Keep them in a separate `secrets.h` file listed in `.gitignore`. If you have already published them, regenerate the token in Blynk and change your WiFi password.

## Upload and Run

1. Select **NodeMCU 1.0 (ESP-12E Module)** under Tools > Board.
2. Select the correct COM port.
3. Upload the sketch.
4. Open Serial Monitor at **115200** baud to see readings.

## How It Works

**Lid control**

- Servo 1 rests at `0°`. When IR Sensor 1 detects an object, it moves to `100°`, waits 5 seconds, then returns to `0°`.
- Servo 2 rests at `100°`. When IR Sensor 2 detects an object, it moves to `0°`, waits 5 seconds, then returns to `100°`.
- A flag (`sensor1Active` / `sensor2Active`) prevents retriggering until the object is removed.

**Fill level**

Distance is calculated from the echo pulse:

```
distance (cm) = duration * 0.034 / 2
```

It is then mapped to a percentage:

```cpp
fillLevel = map(distance, 2, 10, 100, 0);
```

- 2 cm from the sensor = 100% full
- 10 cm from the sensor = 0% (empty)

Adjust the `2` and `10` values to match the depth of your bin.

## Troubleshooting

- **Fill level below 0 or above 100:** `map()` does not clamp values. Wrap it with `constrain(..., 0, 100)`.
- **Distance always 0:** Check wiring and sensor power. HC-SR04 ECHO outputs 5V, so use a voltage divider to protect the 3.3V ESP8266 pins.
- **Boot problems:** `D4` (GPIO2) must be HIGH and `D8` (GPIO15) must be LOW during boot. A servo or echo line pulling these the wrong way can stop the board from starting; disconnect them while uploading if needed.
- **Servo on D0 not moving or jittering:** `D0` (GPIO16) is not well supported by the Servo library on ESP8266. Try moving the servo to `D3` or another PWM-capable pin.
- **Servo resets the board:** Power servos from a separate 5V supply with a common ground.
- **Slow Blynk updates:** The `delay(5000)` calls block the loop while a lid is open, so Blynk updates pause during that time.

## Possible Improvements

- Replace blocking `delay()` with non-blocking `millis()` timing.
- Add a push notification in Blynk when a bin is over 90% full.
- Average multiple ultrasonic readings to reduce noise.

## License

Released under the MIT License. Feel free to use and modify.
