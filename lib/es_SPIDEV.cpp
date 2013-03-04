#include "es_SPIDEV.hpp"
#include "es_SPI_EX.hpp"

es_SPIDEV::es_SPIDEV(string device_path, int mode, int clock_speed, int bits_per_word) {
    this->spidev_fh = open(device_path.c_str(), O_RDWR);
    if( this->spidev_fh < 0 ) {
        throw SPI_EX_INIT();
    }
    this->setMode(mode);
    this->setClockSpeed(clock_speed);
    this->setBitsPerWord(bits_per_word);
}

void es_SPIDEV::setMode(int mode) {
    int rc = ioctl(this->spidev_fh, SPI_IOC_WR_MODE, &mode);
    if (rc < 0) {
        throw SPI_EX_MODIFY();
    }
    rc = ioctl(this->spidev_fh, SPI_IOC_RD_MODE, &mode);
    if (rc < 0) {
        throw SPI_EX_MODIFY();
    }
}

void es_SPIDEV::setClockSpeed(int clock_speed) {
    int rc = ioctl(this->spidev_fh, SPI_IOC_WR_MAX_SPEED_HZ, &clock_speed);
    if (rc < 0) {
        throw SPI_EX_MODIFY();
    }
    rc = ioctl(this->spidev_fh, SPI_IOC_RD_MAX_SPEED_HZ, &clock_speed);
    if (rc < 0) {
        throw SPI_EX_MODIFY();
    }
}

void es_SPIDEV::setBitsPerWord(int bits_per_word) {
    int rc = ioctl(this->spidev_fh, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word);
    if (rc < 0) {
        throw SPI_EX_MODIFY();
    }
    rc = ioctl(this->spidev_fh, SPI_IOC_RD_BITS_PER_WORD, &bits_per_word);
    if (rc < 0) {
        throw SPI_EX_MODIFY();
    }
}

void es_SPIDEV::transmit(unsigned char *tx_buffer, bool deslect_after) {
    struct spi_ioc_transfer transfer;
    //memset(transfer, 0, sizeof transfer);
    
    transfer.tx_buf = (unsigned long) tx_buffer;
    transfer.rx_buf = 0;
    transfer.len = sizeof tx_buffer;
    transfer.cs_change = deslect_after;
    
    transfer.speed_hz = 0;
    transfer.bits_per_word = 0;
    transfer.delay_usecs = 0;
    
    int rc = ioctl(this->spidev_fh,  SPI_IOC_MESSAGE(1), &transfer);
    if (rc < 0) {
        throw SPI_EX_TRANSFER();
    }
}

void es_SPIDEV::recieve(unsigned char *rx_buffer, bool deslect_after) {
    struct spi_ioc_transfer transfer;
    //memset(transfer, 0, sizeof transfer);
    
    transfer.rx_buf = (unsigned long) rx_buffer;
    transfer.tx_buf = 0;
    transfer.len = sizeof rx_buffer;
    transfer.cs_change = deslect_after;
    
    int rc = ioctl(this->spidev_fh,  SPI_IOC_MESSAGE(1), &transfer);
    if (rc < 0) {
        throw SPI_EX_TRANSFER();
    }
    
    transfer.speed_hz = 0;
    transfer.bits_per_word = 0;
    transfer.delay_usecs = 0;
}

es_SPIDEV::~es_SPIDEV() {
    close(this->spidev_fh);
}

