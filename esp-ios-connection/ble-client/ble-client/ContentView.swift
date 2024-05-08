import SwiftUI
import CoreBluetooth

struct ContentView: View {
    @StateObject private var bleManager = BLEManager()
    @State private var toggleSwitches: [UUID: [String: Bool]] = [:]
    @State private var isPlayButtonEnabled: Bool = true

    let characteristicNames: [String: String] = [
        "87654321-4321-4321-4321-210987654321": "LED",
        "43214321-1234-4321-1234-432112345678": "Move Feather",
        "21098765-8765-4321-4321-876543211098": "Play Sound"
    ]

    var body: some View {
        VStack {
            if bleManager.isConnected {
                Text("Devices Connected: \(bleManager.connectedPeripherals.count)")
            } else {
                Text("No Devices Connected")
            }

            List(bleManager.connectedPeripherals, id: \.identifier) { peripheral in
                Section(header: Text("Peripheral: \(peripheral.name ?? "Unknown")")) {
                    if let characteristicList = bleManager.characteristicsByPeripheralId[peripheral.identifier], !characteristicList.isEmpty {
                        ForEach(characteristicList, id: \.uuid) { characteristic in
                            HStack {
                                let characteristicName = characteristicNames[characteristic.uuid.uuidString] ?? characteristic.uuid.uuidString
                                Text("\(characteristicName)")
                                Spacer()

                                if characteristicName == "LED" {
                                    Toggle(isOn: Binding(
                                        get: { self.toggleSwitches[peripheral.identifier]?["ledOn"] ?? false },
                                        set: { newValue in
                                            self.toggleSwitches[peripheral.identifier, default: [:]]["ledOn"] = newValue
                                            bleManager.writeValue(peripheral: peripheral, characteristic: characteristic, value: newValue ? "ON" : "OFF")
                                            print("App sent LED \(newValue) command to \(peripheral.name ?? "Unknown")")
                                        }
                                    )) {
                                        EmptyView()
                                    }
                                    .labelsHidden()
                                    .toggleStyle(SwitchToggleStyle(tint: .blue))
                                } else if characteristicName == "Play Sound" {
                                    Toggle(isOn: Binding(
                                        get: { self.toggleSwitches[peripheral.identifier]?["playSound"] ?? false },
                                        set: { newValue in
                                            self.toggleSwitches[peripheral.identifier, default: [:]]["playSound"] = newValue
                                            let value = newValue ? "PLAY" : "STOP"
                                            bleManager.writeValue(peripheral: peripheral, characteristic: characteristic, value: value)
                                            print("App sent \(value) command to \(peripheral.name ?? "Unknown")")
                                        }
                                    )) {
                                        EmptyView()
                                    }
                                    .labelsHidden()
                                    .toggleStyle(SwitchToggleStyle(tint: .green))
                                } else {
                                    HStack(spacing: 20) {
                                        Spacer()
                                        Button("ON") {
                                            bleManager.writeValue(peripheral: peripheral, characteristic: characteristic, value: "ON")
                                            print("App sent ON command for motor to \(peripheral.name ?? "Unknown")")
                                        }
                                        .padding(10)
                                        .background(Color.green)
                                        .foregroundColor(Color.white)
                                        .cornerRadius(8)
                                        .buttonStyle(PlainButtonStyle())

                                        Button("OFF") {
                                            bleManager.writeValue(peripheral: peripheral, characteristic: characteristic, value: "OFF")
                                            print("App sent OFF command for motor to \(peripheral.name ?? "Unknown")")
                                        }
                                        .padding(10)
                                        .background(Color.red)
                                        .foregroundColor(Color.white)
                                        .cornerRadius(8)
                                        .buttonStyle(PlainButtonStyle())
                                    }
                                }
                            }
                        }
                    } else {
                        Text("No characteristics found")
                    }
                }
            }
        }
        .padding()
    }
}


class BLEManager: NSObject, ObservableObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    
    private var centralManager: CBCentralManager!
    @Published var connectedPeripherals: [CBPeripheral] = []
    @Published var characteristicsByPeripheralId: [UUID: [CBCharacteristic]] = [:]
    @Published var isConnected = false
    
    override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }

    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
        case .poweredOn:
            if connectedPeripherals.isEmpty {
                // Start scanning if no devices are connected
                central.scanForPeripherals(withServices: nil, options: nil)
            } else {
                // Try to reconnect to known peripherals
                for peripheral in connectedPeripherals {
                    if peripheral.state != .connected {
                        central.connect(peripheral, options: nil)
                    }
                }
            }
        default:
            print("Bluetooth is not powered on. Current state: \(central.state.rawValue)")
        }
    }

    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String: Any], rssi RSSI: NSNumber) {
        print("Discovered \(peripheral.name ?? "Unknown") at \(RSSI)")

        if let peripheralName = peripheral.name, peripheralName.hasPrefix("Meow") {
            print("Attempting to connect to \(peripheralName)")
            // Check if the peripheral is already connected before adding it
            if !connectedPeripherals.contains(where: { $0.identifier == peripheral.identifier }) {
                connectedPeripherals.append(peripheral)
                peripheral.delegate = self
                central.connect(peripheral, options: nil)
                isConnected = true
            }
        } else {
            print("Ignored \(peripheral.name ?? "Unknown")")
        }
    }


    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        peripheral.discoverServices(nil) // Ensure connection is established
    }

    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        if let error = error {
            print("Error discovering services: \(error.localizedDescription)")
            return
        }

        guard let services = peripheral.services else {
            print("No services found")
            return
        }

        for service in services {
            peripheral.discoverCharacteristics(nil, for: service)
        }
    }

    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        if let error = error {
            print("Error discovering characteristics: \(error.localizedDescription)")
            return
        }

        guard let characteristics = service.characteristics else {
            print("No characteristics found")
            return
        }

        characteristicsByPeripheralId[peripheral.identifier] = characteristics
    }

    func writeValue(peripheral: CBPeripheral, characteristic: CBCharacteristic, value: String) {
        let data = Data(value.utf8)
        peripheral.writeValue(data, for: characteristic, type: .withResponse) // Ensure correct type
        print("Data to be written to \(characteristic.uuid): \(value)"); // Debug write values
    }
}
