# solar-tracker
Implementation of a Solar Tracker

## Resources
* To implement the httpd server that will expose the control API : [Libmicrohttpd](https://www.gnu.org/software/libmicrohttpd/)
wget http://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.49.tar.gz
wget http://www.phidgets.com/downloads/libraries/libphidget.tar.gz
* To pilote Pololu maestro [libmaestro](https://github.com/pdehn/libmaestro)

### Installation libmicrohttpd
1. Install require package

Centos
* sudo yum install libgcrypt-devel
* sudo yum install gnutls-devel
* sudo yum install libcurl-devel

Raspbian
* sudo aptitude install libgcrypt-dev libgnutls-dev 
* sudo aptitude install libusb-1.0-0-dev libcurl4-gnutls-dev

2. ./configure
3. make
4. sudo make install

### Installation libphidget
1. Install require package
..* Install libusb-1.0
2. ./configure
3. make
4. sudo make install

### Installation libmaestro
1. Install requirements
..* sudo yum install cmake
2. git clone https://github.com/pdehn/libmaestro.git
3. cd libmaestro/build
4. cmake ..
5. make
6. sudo make install
