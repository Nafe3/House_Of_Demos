# CAN Demo overview
## 1. Network Functionality Concept
### Distributed Sensor Network with Functional Safety Monitoring:
    • Microcontroller 1 (MCU1): Master Controller / Safety Manager
        ○ Collects data from other microcontrollers and monitors the system's overall health.
        ○ Implements a functional safety protocol (e.g., watchdog timer, error handling) and ensures the entire network is operating safely.
        ○ Handles communication with an external interface (e.g., serial terminal, display) to report system status.
    • Microcontroller 2 (MCU2): Sensor Node 1
        ○ Simulates a critical sensor (e.g., temperature sensor) that sends periodic data to the Master Controller.
        ○ Implements self-diagnostics and error reporting to the Master Controller.
    • Microcontroller 3 (MCU3): Sensor Node 2
        ○ Simulates another critical sensor (e.g., pressure sensor) and behaves similarly to MCU2, sending data to the Master Controller.
        ○ Implements a fault injection mechanism (e.g., deliberately send faulty data to test the Master Controller's error-handling capability).
    • Microcontroller 4 (MCU4): Actuator Node
        ○ Simulates an actuator (e.g., a fan or motor) that responds to commands from the Master Controller based on sensor inputs.
        ○ Also, report its status back to the Master Controller to ensure proper functioning.
### Functional Flow:
    • The Master Controller (MCU1) requests and collects data from Sensor Nodes (MCU2 and MCU3) over the CAN network.
    • Based on the data received, the Master Controller sends commands to the Actuator Node (MCU4).
    • The Master Controller monitors the integrity of the data and the responsiveness of the nodes, triggering safety mechanisms if an anomaly is detected (e.g., a sensor goes out of range or an actuator fails to respond).
## 2. Zephyr OS Integration
    • Zephyr OS can be used to manage tasks, handle inter-process communication, and manage CAN communication on each microcontroller.
    • Implement CAN communication using Zephyr's built-in CAN drivers.
    • Implement watchdog timers, safety checks, and error handling using Zephyr’s real-time capabilities.
## 3. Wiring Diagram
Here’s a simplified wiring diagram:
    1. CAN Bus: All 4 microcontrollers connect to a single CAN bus.
        • CAN High (CANH) and CAN Low (CANL) lines run in parallel, connecting all the transceivers.
        • Termination resistors (typically 120 ohms) at both ends of the CAN bus.
    2. Microcontroller to Transceiver Connections:
        • MCU CAN_TX -> CAN Transceiver TXD
        • MCU CAN_RX -> CAN Transceiver RXD
        • CAN Transceiver CANH -> Common CANH Bus
        • CAN Transceiver CANL -> Common CANL Bus
        • CAN Transceiver VCC -> +3.3V or +5V (depending on the transceiver)
        • CAN Transceiver GND -> GND
    3. Power Supply:
        • Ensure all microcontrollers and transceivers share a common ground.
        • Provide a stable power supply to each microcontroller and CAN transceiver.
## 4. Practical Considerations
    • Error Handling: Implement error frames, acknowledgment handling, and bus-off recovery mechanisms.
    • Diagnostics: Implement diagnostic messages or an OBD-like protocol to simulate real-world automotive scenarios.
    • Visualization: Consider connecting a display or logging data to a terminal for visualization of sensor data, actuator commands, and system health status.

# CONNECTIONS
CAN bus network with 4 microcontrollers and 4 CAN transceivers:

## 1. Components Overview
    • MCU1: Master Controller / Safety Manager
    • MCU2: Sensor Node 1
    • MCU3: Sensor Node 2
    • MCU4: Actuator Node
    • CAN Transceivers: 4 transceivers, each connected to one microcontroller
## 2. CAN Bus Network
### All microcontrollers and their respective transceivers are connected to a single CAN bus consisting of two lines:
    • CAN High (CANH)
    • CAN Low (CANL)
## 3. Wiring Connections
### Microcontroller 1 (MCU1):
    • TX (Transmit Pin) -> CAN Transceiver 1 TXD (Transmit Data)
    • RX (Receive Pin) -> CAN Transceiver 1 RXD (Receive Data)
    • VCC (+3.3V or +5V) -> CAN Transceiver 1 VCC
    • GND -> CAN Transceiver 1 GND
    • CAN Transceiver 1 CANH -> Common CANH Bus
    • CAN Transceiver 1 CANL -> Common CANL Bus
### Microcontroller 2 (MCU2):
    • TX (Transmit Pin) -> CAN Transceiver 2 TXD
    • RX (Receive Pin) -> CAN Transceiver 2 RXD
    • VCC (+3.3V or +5V) -> CAN Transceiver 2 VCC
    • GND -> CAN Transceiver 2 GND
    • CAN Transceiver 2 CANH -> Common CANH Bus
    • CAN Transceiver 2 CANL -> Common CANL Bus
### Microcontroller 3 (MCU3):
    • TX (Transmit Pin) -> CAN Transceiver 3 TXD
    • RX (Receive Pin) -> CAN Transceiver 3 RXD
    • VCC (+3.3V or +5V) -> CAN Transceiver 3 VCC
    • GND -> CAN Transceiver 3 GND
    • CAN Transceiver 3 CANH -> Common CANH Bus
    • CAN Transceiver 3 CANL -> Common CANL Bus
### Microcontroller 4 (MCU4):
    • TX (Transmit Pin) -> CAN Transceiver 4 TXD
    • RX (Receive Pin) -> CAN Transceiver 4 RXD
    • VCC (+3.3V or +5V) -> CAN Transceiver 4 VCC
    • GND -> CAN Transceiver 4 GND
    • CAN Transceiver 4 CANH -> Common CANH Bus
    • CAN Transceiver 4 CANL -> Common CANL Bus
## 4. CAN Bus Termination
    • Place a 120-ohm termination resistor between CANH and CANL at both ends of the CAN bus. This is critical for proper signal integrity on the CAN network.
## 5. Power Supply
    • Ensure that all microcontrollers and CAN transceivers share the same power supply and ground connection to maintain stable communication.
## 6. Functional Overview
    • MCU1 (Master Controller): Manages the overall communication, collects sensor data, processes it, and sends commands to the actuator.
    • MCU2 (Sensor Node 1): Sends simulated sensor data (e.g., temperature) to the Master Controller.
    • MCU3 (Sensor Node 2): Sends different simulated sensor data (e.g., pressure) and can introduce faults to test the network.
    • MCU4 (Actuator Node): Receives commands from the Master Controller to simulate controlling an actuator (e.g., turning on a motor).

