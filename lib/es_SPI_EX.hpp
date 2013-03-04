#ifndef es_SPI_EX_H
#define es_SPI_EX_H

#include <exception>

/****************************
*   Base exception for errors in using the SPI device interface.
****************************/
class es_SPI_EX: public std::exception
{
    virtual const char* what() const throw() {
        return "An error occured while interacting with an SPI device.";
    }
};

/****************************
*   Exception thrown if unable to open an SPI device.
****************************/
class es_SPI_EX_INIT: public es_SPI_EX
{
    virtual const char* what() const throw() {
        return "An error occured while opening the specified SPI device.";
    }
};

/****************************
*   Exception thrown if unable to modify an SPI device.
****************************/
class es_SPI_EX_MODIFY: public es_SPI_EX
{
    virtual const char* what() const throw() {
        return "An error occured while modifying the properties of the specified SPI device.";
    }
};

/****************************
*   Exception thrown if an error occurs while doing an SPI transfer
****************************/
class es_SPI_EX_TRANSFER: public es_SPI_EX
{
    virtual const char* what() const throw() {
        return "An error occured while doing an SPI data transfer.";
    }
};

#endif
