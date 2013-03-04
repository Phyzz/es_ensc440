#ifndef es_SPIDEV_H
#define es_SPIDEV_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include <string>


using std::string;

/****************************
*   Thin class wrapper around spidev and ioctl functionality.
****************************/
class es_SPIDEV {

    public:
        /****************************
        *   Initialize an SPI device interface.
        *       device_path: The path to SPI intefrace's file. eg. /dev/spidev0.0
        *       mode: The mode to use in SPI communication, valid choices are 0, 1, 2, 3. Default: 0
        *       clock_speed: SPI clock speed in Hz. Default: 500000
        *       bits_per_word: Bits per SPI communication word. Default: 8
        ****************************/
        es_SPIDEV(string device_path, int mode = 0, int clock_speed = 500000, int bits_per_word = 8);

        /****************************
        *   Change the mode for SPI communication.
        *       mode: The mode to use in SPI communication, valid choices are 0, 1, 2, 3. Default: 0
        ****************************/
        void setMode(int mode);

        /****************************
        *   Change the SPI clock speed.
        *       clock_speed: SPI clock speed in Hz. Default: 500000
        ****************************/
        void setClockSpeed(int clock_speed);


        /****************************
        *   Change the mode SPI bits per word.
        *       bits_per_word: Bits per SPI communication word. Default: 8
        ****************************/
        void setBitsPerWord(int bits_per_word);

        /****************************
        *   Initiate an SPI transmission.
        *       *tx_buffer: A pointer to a buffer containing the data to be transmitted.
        *       deselect_after: Determines wheter the SPI device is deselected after the transmission is complete.
        ****************************/
        void transmit(unsigned char *tx_buffer, bool deslect_after);
        
        /****************************
        *   Attempt to read data from an SPI device.
        *       *tx_buffer: A pointer to a buffer where the recieved data will be stored.
        *       deselect_after: Determines wheter the SPI device is deselected after the transmission is complete.
        ****************************/
        void recieve(unsigned char *rx_buf, bool deslect_after);
        
        void half_duplex(unsigned char *tx_buf, unsigned char *rx_buf, bool deselect_between, bool deselect_after);

        ~es_SPIDEV();

    private:
        int spidev_fh;
};

#endif
