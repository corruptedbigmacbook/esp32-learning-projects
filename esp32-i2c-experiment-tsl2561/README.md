# TSL2561 Lux Sensor Experiment with ESP32

This project is a small ESP32 + ESP-IDF experiment where I interface with the TSL2561 light sensor over I2C and calculate lux values from the raw sensor data.

The goal of this project was to practice reading a real sensor datasheet, writing to configuration registers, reading multi-byte data registers, combining low/high bytes, and converting raw sensor readings into a physical value.

---

## What I Practiced

- ESP32 I2C master setup using ESP-IDF
- Adding an I2C device to the bus
- Writing to TSL2561 control and timing registers
- Reading 16-bit sensor data from two 8-bit registers
- Using the TSL2561 command bit and word bit
- Combining low byte and high byte values
- Calculating lux from Channel 0 and Channel 1 readings
- Using FreeRTOS delays for sensor integration timing

---

## Hardware Used

- ESP32 development board
- TSL2561 light sensor module
- Jumper wires
- Breadboard

---

## Wiring

| TSL2561 Pin | ESP32 Pin |
|------------|-----------|
| SDA        | GPIO 18   |
| SCL        | GPIO 19   |
| VCC        | 3.3V      |
| GND        | GND       |

The sensor is used at I2C address:


#define TSL2561_ADDR 0x39
