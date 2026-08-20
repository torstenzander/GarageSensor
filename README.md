# Garage Sensor

This project is a small ESP32-S3 based garage door sensor. It monitors a reed switch and sends the current door state over LoRa using the ThinkNode M2 module with an SX1262 radio.

## What it does

- Reads the reed contact connected to GPIO2
- Detects whether the garage is open or closed
- Sends a LoRa message when the state changes
- Sends a periodic heartbeat every 60 seconds
- Prints status information to the serial monitor at 115200 baud

The project transmits these messages:

- `GARAGE_CLOSED`
- `GARAGE_OPEN`

## Hardware

- Board: ESP32-S3 DevKitC-1
- Radio: SX1262 via ThinkNode M2
- Reed switch: connected between GPIO2 and GND
- Logic level: 3.3V

## Pin mapping

The code configures the following pins:

- Reed contact: GPIO2
- LoRa CS: GPIO10
- LoRa SCK: GPIO12
- LoRa MOSI: GPIO11
- LoRa MISO: GPIO13
- LoRa RST: GPIO21
- LoRa BUSY: GPIO14
- LoRa DIO1: GPIO3
- LoRa power enable: GPIO48

## Wiring notes

The reed contact is configured as an input with pull-up enabled:

- One side of the reed switch connects to GPIO2
- The other side connects to GND
- When the contact is closed, the input reads LOW
- When the contact is open, it reads HIGH

The radio is powered through GPIO48, which is driven HIGH in setup.

## PlatformIO configuration

This project uses PlatformIO with the ESP32-S3 platform and the Arduino framework.

Relevant configuration from `platformio.ini`:

- Platform: `https://github.com/pioarduino/platform-espressif32/releases/download/stable/platform-espressif32.zip`
- Board: `esp32-s3-devkitc-1`
- Framework: `arduino`
- Flash size: `4MB`
- Monitor speed: `115200`
- Upload port: `/dev/cu.wchusbserial210`
- Monitor port: `/dev/cu.wchusbserial210`
- Library: `jgromes/RadioLib @ 7.7.1`

## Setup

### 1. Install PlatformIO

If you are using VS Code, install the PlatformIO extension.

### 2. Open the project

Open the project folder in VS Code so PlatformIO detects the `platformio.ini` and source files.

### 3. Connect the board

Plug the ESP32-S3 into your USB port. The board is configured to use a serial device like `/dev/cu.wchusbserial210` on macOS.

If your board appears under a different path, update these lines in `platformio.ini`:

```ini
upload_port = /dev/cu.wchusbserial210
monitor_port = /dev/cu.wchusbserial210
```

### 4. Build and upload

From the PlatformIO toolbar or terminal, run:

```bash
pio run -t upload
```

To watch the serial output:

```bash
pio device monitor
```

## Behavior in operation

After startup:

1. Serial output is initialized at 115200 baud.
2. The reed pin is configured with `INPUT_PULLUP`.
3. The SX1262 radio is powered on and initialized.
4. The current garage state is read immediately and sent once.
5. Any change in the reed state triggers a new LoRa transmission.
6. A message is sent every 60 seconds as a heartbeat if nothing else changed.

## Important notes

- The LoRa frequency is set to `868.0 MHz`.
- The bandwidth is `125.0 kHz`.
- The spreading factor is `9`.
- The coding rate is `7`.
- The private sync word is used for the radio link.
- The project assumes the sensor is connected to a LoRa receiver or gateway that understands the transmitted strings.

## Troubleshooting

### Board not detected

- Check the USB cable and serial port
- Verify the port path in `platformio.ini`
- On macOS, the port is often something like `/dev/cu.usbserial-*` or `/dev/cu.wchusbserial*`

### LoRa init fails

- Verify the SPI wiring
- Check that GPIO48 is actually powering the radio module
- Verify the SX1262 is connected correctly and receiving 3.3V power

### No messages sent

- Confirm the reed contact is wired between GPIO2 and GND
- Open the serial monitor and check for startup logs
- Make sure the radio is initialized successfully before transmission

## Project files

- `platformio.ini` – PlatformIO project configuration and board settings
- `src/main.cpp` – Main firmware logic for reed contact and LoRa transmission
