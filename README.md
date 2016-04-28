# solar-tracker
Implementation of a Solar Tracker

## Resources
* To implement the httpd server that will expose the control API : [Libmicrohttpd](https://www.gnu.org/software/libmicrohttpd/)
* To pilote Pololu maestro [libmaestro](https://github.com/pdehn/libmaestro)

### Installation libmicrohttpd
1. Install require package
..* sudo yum install libgcrypt-devel
..* sudo yum install gnutls-devel
..* sudo yum install libcurl-devel
2. ./configure
3. make
4. sudo make install

### Installation libphidget
1. Install require package
..* sudo yum install libusb-devel
2. ./configure
3. make
4. sudo make install
