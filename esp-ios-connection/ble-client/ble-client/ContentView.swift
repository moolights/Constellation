import SwiftUI
import CoreBluetooth

struct ContentView: View {
    @StateObject private var bleManager = BLEManager()
    @State private var toggleSwitches: [UUID: [String: Bool]] = [:]
    @State private var isPlayButtonEnabled: Bool = true

    let characteristicNames: [String: String] = [
        "87654321-4321-4321-4321-210987654321": "LED",
        "43214321-1234-4321-1234-432112345678": "Move Feather",
        "21098765-8765-4321-4321-876543211098": "Play Sound",
        "56781234-4321-4321-4321-876543210987": "Dispense Treat"
    ]

    var body: some View {
        VStack {
            VStack {
                HStack {
                    Text("My Home")
                        .font(.largeTitle)
                        .fontWeight(.bold)
                    Spacer()
                    Image(systemName: "plus")
                        .font(.title)
                }
                .padding()
                
                ScrollView(.horizontal, showsIndicators: false) {
                    HStack {
                        QuickView(ImageName:"battery.100",info:"Battery",status: "Good")
                        QuickView(ImageName:"cellularbars",info:"Signal",status: "Poor")
                        QuickView(ImageName:"airport.express",info:"Devices",status: "\(bleManager.connectedPeripherals.count)")
                        Spacer()
                    }.padding()
                }
                
                HStack {
                    Text("Quick Play")
                        .font(.headline)
                        .padding()
                    Spacer()
                }
                
                ScrollView(.horizontal, showsIndicators: false) {
                    HStack {
                        QuickButton(ImageName: "house.fill", PlayName: "Trick or Treat")
                        QuickButton(ImageName: "house.fill", PlayName: "Where are you")
                        QuickButton(ImageName: "house.fill", PlayName: "Catch it!")
                    }.padding()
                }
            }
            .background(LinearGradient(gradient: Gradient(colors: [Color("DarkPurple"), Color("LightPurple")]), startPoint: .top, endPoint: .bottom))
            .shadow(color: Color.black.opacity(0.25), radius: 4, x: 0, y: 4)
            
            HStack {
                Text("Devices")
                    .font(.headline)
                    .padding()
                Spacer()
            }
            
            ScrollView {
                VStack {
                    ForEach(bleManager.connectedPeripherals, id: \.identifier) { peripheral in
                        DeviceView(name: peripheral.name ?? "Unknown", count: bleManager.characteristicsByPeripheralId[peripheral.identifier]?.count ?? 0, bleManager: bleManager, peripheral: peripheral, toggleSwitches: $toggleSwitches)
                    }
                }
                .padding()
            }
            
            TabView {
                Text("")
                    .tabItem {
                        Image(systemName: "house")
                        Text("Home")
                    }
                
                Text("")
                    .tabItem {
                        Image(systemName: "magnifyingglass")
                        Text("Discover")
                    }
                
                Text("")
                    .tabItem {
                        Image(systemName: "person")
                        Text("My")
                    }
            }
            .frame(height: 50)
        }
    }
}

struct QuickView: View {
    let ImageName: String
    let info: String
    let status: String
    
    var body: some View {
        HStack {
            Image(systemName: ImageName)
                .font(.system(size: 12))
            VStack(alignment: .leading){
                Text(info)
                    .font(.system(size: 12))
                Text(status)
                    .font(.system(size: 12))
            }
        }
        .fixedSize(horizontal: true, vertical: false)
        .frame(height: 10)
        .padding()
        .background(Color("LightPurple"))
        .cornerRadius(20)
        .shadow(color: Color.black.opacity(0.25), radius: 4, x: 4, y: 4)
        .shadow(color: Color.white.opacity(0.25), radius: 4, x: -1, y: -1)
    }
}

struct QuickButton: View {
    let ImageName: String
    let PlayName: String
    
    var body: some View {
        Button(action: {
            // Handle button action
        }) {
            HStack {
                Image(systemName: ImageName)
                    .font(.title)
                    .font(.subheadline)
                Text(PlayName)
                    .font(.system(size: 16))
                    .frame(alignment: .leading)
            }
            .padding()
            .foregroundStyle(.black)
            .fixedSize(horizontal: true, vertical: false)
            .frame(width: 170)
            .background(Color("LightPurple"))
            .cornerRadius(10)
            .shadow(color: Color.black.opacity(0.25), radius: 4, x: 4, y: 4)
            .shadow(color: Color.white.opacity(0.35), radius: 4, x: -1, y: -1)
        }
    }
}

struct DeviceView: View {
    let name: String
    let count: Int
    let bleManager: BLEManager
    let peripheral: CBPeripheral
    @Binding var toggleSwitches: [UUID: [String: Bool]]
    
    var body: some View {
        HStack {
            Rectangle()
                .fill(Color.gray.opacity(0.2))
                .frame(width: 150, height: 150)
            
            Spacer()
            
            VStack(alignment: .trailing) {
                Text(name)
                    .font(.subheadline)
                Text("\(count) Times")
                    .font(.caption)
                Divider()
                    .frame(height: 30)
                HStack {
                    Button(action: {
                        // Handle LED toggle action
                        if let characteristic = bleManager.characteristicsByPeripheralId[peripheral.identifier]?.first(where: { $0.uuid.uuidString == "87654321-4321-4321-4321-210987654321" }) {
                            let newValue = !(self.toggleSwitches[peripheral.identifier]?["ledOn"] ?? false)
                            self.toggleSwitches[peripheral.identifier, default: [:]]["ledOn"] = newValue
                            bleManager.writeValue(peripheral: peripheral, characteristic: characteristic, value: newValue ? "ON" : "OFF")
                            print("App sent LED \(newValue ? "ON" : "OFF") command to \(peripheral.name ?? "Unknown")")
                        }
                    }) {
                        Image(systemName: "lightbulb.fill")
                            .font(.system(size: 20))
                            .padding(8)
                            .background(Color.black.opacity(0.25))
                            .foregroundColor(.white)
                            .clipShape(Circle())
                    }
                    .shadow(color: Color.black.opacity(0.25), radius: 4, x: 4, y: 4)
                    .shadow(color: Color.white.opacity(0.35), radius: 4, x: -1, y: -1)
                    
                    Button(action: {
                        // Handle play sound toggle action
                        if let characteristic = bleManager.characteristicsByPeripheralId[peripheral.identifier]?.first(where: { $0.uuid.uuidString == "21098765-8765-4321-4321-876543211098" }) {
                            let newValue = !(self.toggleSwitches[peripheral.identifier]?["playSound"] ?? false)
                            self.toggleSwitches[peripheral.identifier, default: [:]]["playSound"] = newValue
                            let value = newValue ? "PLAY" : "STOP"
                            bleManager.writeValue(peripheral: peripheral, characteristic: characteristic, value: value)
                            print("App sent \(value) command to \(peripheral.name ?? "Unknown")")
                        }
                    }) {
                        Image(systemName: "music.quarternote.3")
                            .font(.system(size: 18))
                            .padding(8)
                            .background(Color.black.opacity(0.25))
                            .foregroundColor(.white)
                            .clipShape(Circle())
                    }
                    .shadow(color: Color.black.opacity(0.25), radius: 4, x: 4, y: 4)
                    .shadow(color: Color.white.opacity(0.35), radius: 4, x: -1, y: -1)
                    
                    Button(action: {
                        // Handle dispense treat action
                        if let characteristic = bleManager.characteristicsByPeripheralId[peripheral.identifier]?.first(where: { $0.uuid.uuidString == "56781234-4321-4321-4321-876543210987" }) {
                            bleManager.writeValue(peripheral: peripheral, characteristic: characteristic, value: "DISPENSE")
                            print("App sent DISPENSE command to \(peripheral.name ?? "Unknown")")
                        }
                    }) {
                        Image(systemName: "waveform.path")
                            .font(.system(size: 15))
                            .padding(8)
                            .background(Color.black.opacity(0.25))
                            .foregroundColor(.white)
                            .clipShape(Circle())
                    }
                    .shadow(color: Color.black.opacity(0.25), radius: 4, x: 4, y: 4)
                    .shadow(color: Color.white.opacity(0.35), radius: 4, x: -1, y: -1)

                    // Move Feather ON/OFF Buttons
                    VStack {
                        Button(action: {
                            // Handle move feather ON action
                            if let characteristic = bleManager.characteristicsByPeripheralId[peripheral.identifier]?.first(where: { $0.uuid.uuidString == "43214321-1234-4321-1234-432112345678" }) {
                                bleManager.writeValue(peripheral: peripheral, characteristic: characteristic, value: "ON")
                                print("App sent ON command for Move Feather to \(peripheral.name ?? "Unknown")")
                            }
                        }) {
                            Image(systemName: "arrowtriangle.up.fill")
                                .font(.system(size: 20))
                                .padding(8)
                                .background(Color.green.opacity(0.25))
                                .foregroundColor(.white)
                                .clipShape(Circle())
                        }
                        .shadow(color: Color.black.opacity(0.25), radius: 4, x: 4, y: 4)
                        .shadow(color: Color.white.opacity(0.35), radius: 4, x: -1, y: -1)

                        Button(action: {
                            // Handle move feather OFF action
                            if let characteristic = bleManager.characteristicsByPeripheralId[peripheral.identifier]?.first(where: { $0.uuid.uuidString == "43214321-1234-4321-1234-432112345678" }) {
                                bleManager.writeValue(peripheral: peripheral, characteristic: characteristic, value: "OFF")
                                print("App sent OFF command for Move Feather to \(peripheral.name ?? "Unknown")")
                            }
                        }) {
                            Image(systemName: "arrowtriangle.down.fill")
                                .font(.system(size: 20))
                                .padding(8)
                                .background(Color.red.opacity(0.25))
                                .foregroundColor(.white)
                                .clipShape(Circle())
                        }
                        .shadow(color: Color.black.opacity(0.25), radius: 4, x: 4, y: 4)
                        .shadow(color: Color.white.opacity(0.35), radius: 4, x: -1, y: -1)
                    }
                }
            }
            .alignmentGuide(.trailing) { d in d[.trailing] }
        }
        .padding(.horizontal)
        .background(Color.gray.opacity(0.2))
        .cornerRadius(10)
        .shadow(color: Color.gray.opacity(0.5), radius: 4, x: 4, y: 4)
        .shadow(color: Color.white.opacity(0.5), radius: 4, x: -1, y: -1)
    }
}

#Preview {
    ContentView()
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
