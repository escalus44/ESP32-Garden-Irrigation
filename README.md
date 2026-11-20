# ESP32-Garden-Irrigation
A DIY Wi-Fi-enabled garden irrigation controller built using an ESP32, relay, and motorized ball valve.

## Hardware

- ESP32-WROOM-32 development board
- ESP32 38-pin expansion board
- AEDIKO 5V single-channel relay module
- U.S. Solid 9-36V motorized ball valve
- LM2596 adjustable DC-DC buck converter
- DC HOUSE 12.8V 6Ah LiFePO4 battery
- 20W SOLPERK solar panel (future integration)
- Inline fuses
- WAGO connectors
- Weather-resistant enclosure (planned)

## Current Features — v1.0

- Wi-Fi connectivity
- Browser-based control panel
- Manual OPEN and CLOSE commands
- Valve status reporting
- Automatic five-minute shutoff
- Valve OFF at startup

## Wiring

### ESP32 to Relay

| ESP32 | Relay |
|-------|-------|
| 5V | DC+ |
| GND | DC- |
| GPIO 23 | IN |

Relay configured for HIGH triggering.

### Battery and Power

Battery positive -> Fuse -> LM2596 IN+

Battery negative -> LM2596 IN-

LM2596 OUT+ -> ESP32 5V

LM2596 OUT- -> ESP32 GND

### Motorized Valve

Battery positive -> Separate Fuse -> Relay COM

Relay NO -> Valve positive (red)

Valve negative (black) -> Battery negative

## Software

- Arduino IDE
- ESP32 Arduino Core
- WiFi.h
- WebServer.h

## Installation

1. Select ESP32 Dev Module in Arduino IDE.
2. Enter your local Wi-Fi credentials.
3. Upload the sketch.
4. Open Serial Monitor at 115200 baud.
5. Note the assigned IP address.
6. Open the address in a web browser.

## API Endpoints

GET /       - Web interface

GET /open   - Open valve

GET /close  - Close valve

GET /status - Return valve status as JSON

Example response:

{"open":false}

## Safety

- Valve powered from fused battery supply.
- ESP32 powered through regulated 5V supply.
- Do not connect battery voltage directly to ESP32 GPIO.
- Maximum continuous valve activation: 5 minutes.
- Controller is intended for a trusted local network.
- HTTP control endpoints currently have no authentication.
- Status represents the commanded relay state,
  not physical valve position.
- Solar charging requires a LiFePO4-compatible
  charge controller.

## Planned Features — v2.0

- NTP time synchronization
- Daily watering schedules
- Day-of-week selection
- Configurable watering durations
- Saved schedules
- Manual schedule override
- Improved mobile interface

## Future Development

- Home Assistant integration
- iPhone Home Screen app
- Soil moisture sensors
- Battery voltage monitoring
- Solar charging integration
- Irrigation activity logging
