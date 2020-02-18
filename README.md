# Hmkit Python Workspace

The HMKit Python Workspace makes it easy to work with car data using the HIGH MOBILITY API platform. The SDK implements a strong security layer between your Python app and the platform while giving you straightforward native interface to read and write to connected cars.

The library is designed to give IOT developers simple access to High Mobility's systems, by handling all the communication protocols, authentication flows and other security related components.

This repository is a development workspace for generating Python bluetooth installable sdk [Hmkit Python SDK](https://github.com/highmobility/hmkit-python/) from all the dependencies.

It has been tested on Raspbian Pi Zero W and Pi 3 Model B+.

## Getting started

* Get started by reading the [Python SDK guide](https://high-mobility.com/learn/tutorials/sdk/python/) in high-mobility.com.

* Check out the [code references](https://high-mobility.com/learn/documentation/iot-sdk/python/hmkit/) for more details that is present in code documentation.

* Checkout supporting documentations in [Developer Center](https://developers.high-mobility.com/)

* Hmkit Python source files are located in [Hmkit Python SDK](https://github.com/highmobility/hmkit-python/)

## Architecture

**General**: Hmkit Python Workspace contains following submodules, hal files and python c extension files. Hal files act as interface  between hmkit-core and other implementation specific submodules. Python c extension files act as interface between python layer and whole c layers.

**hmkit-core**: HMKit Core implements the transportation protocol of HMKit in C.

**hmkit-python**: Hmkit Python bluetooth SDK. Contains Python implementations of Hmkit functions and AutoApi. Also contains dependencies as libraries.

**btstack**: Bluetooth protocol stack implementation from BlueKitchen GmbH.

**hmkit-crypto-c**: HMKIT Crypto C is the HMKit cryptographic layer implementation based on OpenSSL.

**hmkit-persistence-sqlite-c**: HMKit Persistence C layer implementation based on Sqlite3.

## Requirements

Present HMKit SDK requires Python 3.7. Hmkit-python-workspace can be used to build dependencies for various Python versions and upload the built binary to Hmkit Python SDK. Python version requirement is imposed by python implementations in HMKit SDK.

Latest Raspbian image for Pi zero comes with Python 3.7 installed. however Python 2.7 may be enabled as default.
Set Python 3.7 as default in alternatives.

```
If Python 3.7 is not default:
sudo update-alternatives --install /usr/bin/python python /usr/bin/python3.7 2

#sudo apt-get install python3.7-dev
#sudo apt-get install libglib2.0-dev
#sudo apt-get install libsqlite3-dev
#sudo apt-get install libssl-dev
#sudo apt install python3-pip
#pip3 install pytz
```

```
install the dependent libc6. if not available for direct install(apt-get ), download the debian file from raspian repository
https://www.raspberrypi.org/downloads/raspbian/ and install manually 

Example(version changes):
sudo dpkg -i libc6-dev_2.28-8_armhf.deb
#sudo dpkg -i libssl-dev_1.1.1b-1_armhf.dieb
if needed install corresponsing locales, libc6-dev, 
```

## Integration

```
Clone this repo hmkit-python-workspace.
make changes in the corresponding files if needed.

Build the sdk
// builds the source files and copies the build output libraries inside the sdk folder hmkit-python (submodule)
hmkit-python-workspace/
make sdk

for fresh clean build (takes time)
make clean
make sdk
```

### How do I test? ###

Build outputs can be tested from the submodule hmkit-python.
Refer README in [Hmkit Python SDK](https://github.com/highmobility/hmkit-python/) for more details.


## Contributing

We would love to accept your patches and contributions to this project. Before getting to work, please first discuss the changes that you wish to make with us via [GitHub Issues](https://github.com/highmobility/hmkit-python-workspace/issues), [Spectrum](https://spectrum.chat/high-mobility/) or [Slack](https://slack.high-mobility.com/).

See more in [CONTRIBUTING.md](https://github.com/highmobility/hmkit-python-workspace/blob/master/CONTRIBUTING.md)

## License ##

This repository is using MIT license. See more in [LICENSE](https://github.com/highmobility/hmkit-python-workspace/blob/master/LICENSE). The included BTstack submodule is free for non-commercial use only (BSD 3-Clause License).

