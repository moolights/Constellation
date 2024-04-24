//
//  ContentView.swift
//  BLEManager
//
//  Created by Wyatt Boublil on 4/20/24.
//

import SwiftUI
import CoreBluetooth

struct ContentView: View {
    @StateObject private var bleManager = BLEManager()
    
    var body: some View {
        VStack {
            Button("Turn on LED") {
                bleManager.writeValue("ON")
            }
            .padding(.bottom, 150)
            Button("Turn off LED") {
                bleManager.writeValue("OFF")
            }
        }
    }
}

class BLEManager: NSObject, ObservableObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    
    private var centralManager: CBCentralManager!
    private var esp32Peripheral: CBPeripheral?
    private let serviceUUID = CBUUID(string: "12345678-1234-1234-1234-123456789012")
    private let characteristicUUID = CBUUID(string: "87654321-4321-4321-4321-210987654321")
    
    override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == .poweredOn {
            central.scanForPeripherals(withServices: [serviceUUID], options: nil)
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String: Any], rssi RSSI: NSNumber) {
        
        if peripheral.name == "Master" {
            esp32Peripheral = peripheral
            esp32Peripheral?.delegate = self
            central.stopScan()
            central.connect(peripheral, options: nil)
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        peripheral.discoverServices([serviceUUID])
    }

    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        guard let service = peripheral.services?.first else { return }
        peripheral.discoverCharacteristics([characteristicUUID], for: service)
    }

    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        guard let characteristic = service.characteristics?.first else { return }
        // Save the characteristic for writing later
        self.esp32PeripheralCharacteristic = characteristic
    }

    func writeValue(_ value: String) {
        guard let characteristic = esp32PeripheralCharacteristic else { return }
        let data = Data(value.utf8)
        esp32Peripheral?.writeValue(data, for: characteristic, type: .withResponse)
    }
        
    private var esp32PeripheralCharacteristic: CBCharacteristic?
}

#Preview {
    ContentView()
}

