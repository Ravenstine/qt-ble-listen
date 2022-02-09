Qt BLE Listen
=============

This is a barebones example of using Qt to connect to a Bluetooth Low Energy device and listening for changes to characteristic data.  I wrote this while trying to learn how to use Qt to connect to an HM-10 Bluetooth 4.0 module, and tried to keep it as minimal as possible by making it a command line application and using C++11 lambdas to make the code as sensible as possible.  This program does not deal with PIN-based pairing or writing data to the remote characteristic.  If you need to do those things, hopefully this code is easy enough to understand that you can then figure out the rest.

## Prerequisites

I've only compiled this on macOS.  In theory, it should also work for Linux or Windows.

You will need `qt@6`.  It might work for `qt@5`, but I've not tried.

```sh
brew install qt
```

## Compile

Run `qmake` to generate the Makefile.  The qmake program comes with Qt.

```sh
qmake
```

Then run make:

```sh
make
```

This will generate an executable called `qt-ble-listen`.

## Run

```sh
./qt-ble-listen
```

You may need to give this program permission to use Bluetooth.  On macOS you might get a permission dialog requesting Bluetooth access.  If you don't, go to **Security & Privacy** in your system preferences and add the program under **Bluetooth**.
