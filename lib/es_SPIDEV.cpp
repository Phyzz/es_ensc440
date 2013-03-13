#include "es_SPIDEV.hpp"
#include "es_SPI_EX.hpp"
#include "es_EX.hpp"

es_SPIDEV::es_SPIDEV(string device_path, int mode, int clock_speed, int bits_per_word) {
    this->spidev_fh = open(device_path.c_str(), O_RDWR);
    if( this->spidev_fh < 0 ) {
        throw es_SPI_EX_INIT();
    }
    this->setMode(mode);
    this->setClockSpeed(clock_speed);
    this->setBitsPerWord(bits_per_word);
}

void es_SPIDEV::setMode(int mode) {
    if (mode < 0 || mode > 3) {
        throw es_INVALID_ARGS_EX();
    }
    int rc = ioctl(this->spidev_fh, SPI_IOC_WR_MODE, &mode);
    if (rc < 0) {
        throw es_SPI_EX_MODIFY();
    }
    rc = ioctl(this->spidev_fh, SPI_IOC_RD_MODE, &mode);
    if (rc < 0) {
        throw es_SPI_EX_MODIFY();
    }
}

void es_SPIDEV::setClockSpeed(int clock_speed) {
    int rc = ioctl(this->spidev_fh, SPI_IOC_WR_MAX_SPEED_HZ, &clock_speed);
    if (rc < 0) {
        throw es_SPI_EX_MODIFY();
    }
    rc = ioctl(this->spidev_fh, SPI_IOC_RD_MAX_SPEED_HZ, &clock_speed);
    if (rc < 0) {
        throw es_SPI_EX_MODIFY();
    }
}

void es_SPIDEV::setBitsPerWord(int bits_per_word) {
    int rc = ioctl(this->spidev_fh, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word);
    if (rc < 0) {
        throw es_SPI_EX_MODIFY();
    }
    rc = ioctl(this->spidev_fh, SPI_IOC_RD_BITS_PER_WORD, &bits_per_word);
    if (rc < 0) {
        throw es_SPI_EX_MODIFY();
    }
}

void es_SPIDEV::transmit(unsigned char *tx_buffer, int tx_len, bool deslect_after) {
    struct spi_ioc_transfer transfer;
    
    transfer.tx_buf = (unsigned long) tx_buffer;
    transfer.rx_buf = 0;
    transfer.len = tx_len;
    transfer.cs_change = deslect_after;
    
    transfer.speed_hz = 0;
    transfer.bits_per_word = 0;
    transfer.delay_usecs = 0;
    
    int rc = ioctl(this->spidev_fh,  SPI_IOC_MESSAGE(1), &transfer);
    if (rc < 0) {
        throw es_SPI_EX_TRANSFER();
    }
}

void es_SPIDEV::recieve(unsigned char *rx_buffer, int rx_len, bool deslect_after) {
    struct spi_ioc_transfer transfer;
    
    transfer.rx_buf = (unsigned long) rx_buffer;
    transfer.tx_buf = 0;
    transfer.len = rx_len;
    transfer.cs_change = deslect_after;
    
    transfer.speed_hz = 0;
    transfer.bits_per_word = 0;
    transfer.delay_usecs = 0;
    
    int rc = ioctl(this->spidev_fh,  SPI_IOC_MESSAGE(1), &transfer);
    if (rc < 0) {
        throw es_SPI_EX_TRANSFER();
    } 
}

void es_SPIDEV::half_duplex(unsigned char *tx_buffer, int tx_len, unsigned char *rx_buffer, int rx_len, bool deselect_between, bool deselect_after){
    struct spi_ioc_transfer transfers[2];
    
    transfers[0].tx_buf = (unsigned long) tx_buffer;
    transfers[0].rx_buf = 0;
    transfers[0].len = tx_len;
    transfers[0].cs_change = deselect_between;
    
    transfers[1].tx_buf = 0;
    transfers[1].rx_buf = (unsigned long) rx_buffer;
    transfers[1].len = rx_len;
    transfers[1].cs_change = deselect_after;
    
    for (int i = 0; i < 2; ++i) {
        transfers[i].speed_hz = 0;
        transfers[i].bits_per_word = 0;
        transfers[i].delay_usecs = 0;
    }
    
    int rc = ioctl(this->spidev_fh,  SPI_IOC_MESSAGE(2), transfers);
    if (rc < 0) {
        throw es_SPI_EX_TRANSFER();
    } 
}

void es_SPIDEV::full_duplex(unsigned char *tx_buffer, unsigned char *rx_buffer, int len, bool deslect_after) {
    struct spi_ioc_transfer transfer;
    
    transfer.rx_buf = (unsigned long) rx_buffer;
    transfer.tx_buf = (unsigned long) tx_buffer;
    transfer.len = len;
    transfer.cs_change = deslect_after;
    
    transfer.speed_hz = 0;
    transfer.bits_per_word = 0;
    transfer.delay_usecs = 0;
    
    int rc = ioctl(this->spidev_fh,  SPI_IOC_MESSAGE(1), &transfer);
    if (rc < 0) {
        throw es_SPI_EX_TRANSFER();
    } 
}

es_SPIDEV::~es_SPIDEV() {
    close(this->spidev_fh);
}

