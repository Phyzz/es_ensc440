#ifndef es_EX_H
#define es_EX_H

#include <exception>

/****************************
*   Invalid Arguments
****************************/
class es_INVALID_ARGS_EX: public std::exception
{
    virtual const char* what() const throw() {
        return "Invalid argument.";
    }
};

#endif
