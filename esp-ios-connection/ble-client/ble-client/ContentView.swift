import SwiftUI
import CoreBluetooth

struct ContentView: View {
    @StateObject private var bleManager = BLEManager()
    @State private var toggleSwitches: [String: Bool] = ["ledOn": false]

    let characteristicNames: [String: String] = [
        "87654321-4321-4321-4321-210987654321": "LED Control",
        "43214321-1234-4321-1234-432112345678": "Motor Control",
        "21098765-8765-4321-4321-876543211098": "Buzzer Control"
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

                                if characteristicName == "LED Control" {
                                    Toggle(isOn: Binding(
                                        get: { self.toggleSwitches["ledOn", default: false] },
                                        set: { newValue in
                                            self.toggleSwitches["ledOn"] = newValue
                                            bleManager.writeValue(peripheral: peripheral, characteristic: characteristic, value: newValue ? "ON" : "OFF")
                                            print("App sent LED \(newValue) command")
                                        }
                                    )) {
                                        EmptyView()
                                    }
                                    .labelsHidden()
                                    .toggleStyle(SwitchToggleStyle(tint: .blue))
                                } else if characteristicName == "Buzzer Control" {
                                    HStack(spacing: 20) {
                                        Spacer()
                                        Button("PLAY") {
                                            bleManager.writeValue(peripheral: peripheral, characteristic: characteristic,
                                                value: "PLAY")
                                            print("App sent PLAY command") // Debugging
                                        }
                                        .padding(10)
                                        .background(Color.cyan)
                                        .foregroundColor(Color.white)
                                        .cornerRadius(8)
                                        .buttonStyle(PlainButtonStyle())
                                        
                                        Button("STOP") {
                                            bleManager.writeValue(peripheral: peripheral, characteristic: characteristic,
                                                value: "STOP")
                                            print("App sent STOP command") // Debugging
                                        }
                                        .padding(10)
                                        .background(Color.gray)
                                        .foregroundColor(Color.white)
                                        .cornerRadius(8)
                                        .buttonStyle(PlainButtonStyle())
                                    }
                                } else {
                                    HStack(spacing: 20) {
                                        Spacer()
                                        Button("ON") {
                                            bleManager.writeValue(peripheral: peripheral, characteristic: characteristic, value: "ON")
                                            print("App sent ON command for motor")
                                        }
                                        .padding(10)
                                        .background(Color.green)
                                        .foregroundColor(Color.white)
                                        .cornerRadius(8)
                                        .buttonStyle(PlainButtonStyle())

                                        Button("OFF") {
                                            bleManager.writeValue(peripheral: peripheral, characteristic: characteristic, value: "OFF")
                                            print("App sent OFF command for motor")
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
        if central.state == .poweredOn {
            central.scanForPeripherals(withServices: nil, options: nil) // Continue scanning
        } else {
            print("Bluetooth is not powered on. Current state: \(central.state.rawValue)")
        }
    }

    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String: Any], rssi RSSI: NSNumber) {
        if let peripheralName = peripheral.name, peripheralName.hasPrefix("Meow") {
            connectedPeripherals.append(peripheral)
            peripheral.delegate = self
            central.connect(peripheral, options: nil)
            isConnected = true
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
