#include <QDebug>
#include <QString>
#include <QObject>
#include <QCoreApplication>
#include <QLowEnergyController>
#include <QBluetoothDeviceInfo>

int main(int argc, char* argv[]) {
  QCoreApplication bleApp(argc, argv);

  // You will need to find these values using a BLE scanner
  // app on your device.  Although QBluetoothDeviceDiscoveryAgent
  // has a method to discover devices, that action is no longer
  // allowed on macOS in an app that isn't signed by the
  // developer.  In that case, use a BLE scanner app to
  // fill in these UUIDs.
  QBluetoothAddress deviceAddress(QString("30:45:11:F6:3D:2E"));
  // Third parameter is the device class.  Honestly, I have
  // no clue what this is.  Leaving this as 0 for my HM-10
  // device seems to make no difference, but since I'm on
  // macOS this might be ignored.
  QBluetoothDeviceInfo remoteDeviceInfo(deviceAddress, QString("Remote Device"), 0);
  // This device UUID is needed on macOS because macOS does
  // not expose the device MAC address.  You need to either
  // use a BLE scanner app to find the UUID of your target
  // device or implement your own BLE device discovery which
  // is only allowed with app bundles that have a developer
  // signature.  Because of this, you can omit providing the
  // MAC address on macOS or iOS.
  QBluetoothUuid deviceUuid(QString("C758D77A-5716-723C-1692-B2D9343AFC4E"));

  remoteDeviceInfo.setDeviceUuid(deviceUuid);

  QBluetoothUuid serviceUuid(QString("0000FFE0-0000-1000-8000-00805F9B34FB"));
  QBluetoothUuid characteristicUuid(QString("0000FFE1-0000-1000-8000-00805F9B34FB"));

  QLowEnergyController * bleController = QLowEnergyController::createCentral(remoteDeviceInfo);
  // Even though we actually get the service object in the
  // serviceDiscovered lambda, for some reason a segfault
  // occurs unless the variable is declared in a higher
  // scope.  This may not happen with the standard approach
  // of using QObject for signals.
  QLowEnergyService * service = nullptr;

  QObject::connect(bleController, &QLowEnergyController::connected, [&] () {
    qDebug() << "connected";

    bleController->discoverServices();
  });
  QObject::connect(bleController, &QLowEnergyController::disconnected, [&] () {
    qDebug() << "disconnected";
  });
  QObject::connect(bleController, &QLowEnergyController::errorOccurred, [] (QLowEnergyController::Error error) {
    qDebug() << error;
  });
  QObject::connect(bleController, &QLowEnergyController::serviceDiscovered, [&] (const QBluetoothUuid &discoveredServiceUuid) {
    if (discoveredServiceUuid != serviceUuid) return;

    qDebug() << "service found";

    service = bleController->createServiceObject(discoveredServiceUuid);

    // Wait for service details to be discovered
    QObject::connect(service, &QLowEnergyService::stateChanged, [&] (QLowEnergyService::ServiceState serviceState) {
      if (serviceState != QLowEnergyService::RemoteServiceDiscovered) return;

      QLowEnergyCharacteristic characteristic = service->characteristic(characteristicUuid);
      QLowEnergyDescriptor descriptor = characteristic.descriptor(
        QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration
      );

      // Enable notification for value changes
      service->writeDescriptor(descriptor, QByteArray::fromHex("0100"));
    });
    // Listen for value changes via notification
    QObject::connect(service, &QLowEnergyService::characteristicChanged, [&] (const QLowEnergyCharacteristic &characteristic, const QByteArray &value) {
      qDebug() << value;
    });

    service->discoverDetails();
  });

  bleController->connectToDevice();

  bleApp.exec();

  return 0;
}
