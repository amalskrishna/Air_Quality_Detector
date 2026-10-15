# Air Quality Monitoring System Using STM32 Blue Pill

An embedded prototype that reads an **MQ-135 gas sensor** with an **STM32F103C8T6 (Blue Pill)**, classifies the response into three levels (**LOW / MODERATE / HIGH**), shows the status on a **16×2 I²C LCD**, and signals it with **three LEDs**. A built-in **demonstration mode** lets you show all three alert states without changing the air around the sensor.

> ⚠️ **This is a learning/demo project, not a certified air-quality instrument.** The MQ-135 is a broad-response sensor, and the displayed value is a raw ADC reading, not a calibrated ppm concentration or an official AQI.

---

## Features

- Analog sensor reading through the STM32 12-bit ADC (0–4095)
- Three software-defined levels with LED indication
- Live status and raw response value on a 16×2 LCD (I²C)
- Blinking red LED in the HIGH state
- Demonstration mode that cycles through simulated readings
- Programmed with the Arduino IDE and the STM32 Arduino core, uploaded via ST-Link (SWD)

## Hardware Required

| Component | Notes |
|---|---|
| STM32F103C8T6 Blue Pill | Main controller (3.3 V logic) |
| MQ-135 gas sensor module | Analog output (AO) used, DO unconnected |
| JHD162A 16×2 LCD with I²C backpack | Status display |
| LEDs ×3 (green, yellow, red) | One series resistor each (220 Ω – 1 kΩ) |
| 20 kΩ and 10 kΩ resistors | Voltage divider for the sensor output |
| ST-Link programmer | Uploads firmware over SWD |
| Breadboard, jumper wires, regulated power supply | |

## Pin Connections

### MQ-135

| MQ-135 | Connects to |
|---|---|
| VCC | Regulated 5 V |
| GND | Common GND |
| AO | 20 kΩ resistor → divider junction |
| DO | Not connected |

Voltage divider: `AO → 20 kΩ → junction → 10 kΩ → GND`, with the **junction wired to PA0**.

### LCD (I²C)

| LCD | STM32 |
|---|---|
| GND | Common GND |
| VCC | Regulated 5 V (typical for the module) |
| SDA | PB7 |
| SCL | PB6 |

> The I²C pull-ups on the backpack must be compatible with the STM32's 3.3 V logic. If SDA/SCL are pulled up to 5 V, use a bidirectional I²C level shifter or a verified 3.3 V pull-up arrangement.

### LEDs

| LED | Pin |
|---|---|
| Green (LOW) | PB0 → resistor → LED → GND |
| Yellow (MODERATE) | PB1 → resistor → LED → GND |
| Red (HIGH) | PB10 → resistor → LED → GND |

### Power notes

- All grounds must be common.
- Never power the MQ-135 heater from a GPIO pin.
- Check the ST-Link's target-power arrangement before connecting any extra supply, so two supplies never drive each other.

## Software Setup

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Install the **STM32 Arduino core** (STM32duino) through the Boards Manager.
3. Install the **hd44780** library (provides `hd44780_I2Cexp`) through the Library Manager.
4. Select the Blue Pill board (STM32F103C8) and **ST-Link** as the upload method.
5. Connect the ST-Link to the Blue Pill's SWD pins, then open the sketch and upload.

## How It Works

1. The MQ-135 produces an analog voltage that changes with the surrounding air.
2. The voltage divider scales it down for the STM32's analog input.
3. The STM32 reads PA0 and converts it to a 12-bit value.
4. The value is compared with two thresholds and classified.
5. The LCD shows the status and raw value; the matching LED is switched on.
6. In the HIGH state the red LED blinks. The loop then repeats.

Example LCD output:

```
Air: LOW
Response:500
```

## Threshold Logic

| Raw ADC value | Level | Indicator |
|---|---|---|
| Below 1365 | LOW | Green LED |
| 1365 – 2729 | MODERATE | Yellow LED |
| 2730 and above | HIGH | Blinking red LED |

These are demonstration thresholds, **not** validated environmental limits.

## Demonstration Mode

In demonstration mode the program ignores the sensor and generates a simulated value that changes about every five seconds:

| Simulated value | Level |
|---|---|
| 500 | LOW |
| 2000 | MODERATE |
| 3500 | HIGH |

Switch to real sensor mode to display the actual raw ADC reading from PA0.

## Testing Checklist

- [ ] Firmware uploads successfully through ST-Link
- [ ] LCD initializes and shows status and response value
- [ ] MQ-135 output reaches PA0 through the divider within the allowed voltage range
- [ ] Green LED on in LOW, yellow in MODERATE, red blinking in HIGH
- [ ] Demonstration mode cycles through all three states

## Limitations

- The MQ-135 responds to multiple gases and is not specific to any one of them.
- Raw ADC values are not ppm concentrations, and no AQI is calculated.
- Readings vary with warm-up time, temperature, humidity, sensor age, and surrounding air.
- With the 20 kΩ / 10 kΩ divider the PA0 voltage is one third of the sensor output (at most about 1.67 V for a 5 V output, roughly ADC 2070), so real-sensor readings may not reach the HIGH threshold of 2730 unless the divider or thresholds are adjusted.
- Reliable quantitative measurement needs calibration against reference equipment.

## Future Improvements

- Calibrate the sensor against reference instruments
- Add data logging
- Add a wireless module for remote monitoring
- Build a mobile or web dashboard
- Add sensors for specific environmental parameters
- Improve alert logic with calibrated, documented thresholds

These are possible extensions, not part of the current implementation.

## Skills Demonstrated

ADC operation, GPIO control, I²C communication, sensor interfacing, and embedded C/C++ programming on STM32.
