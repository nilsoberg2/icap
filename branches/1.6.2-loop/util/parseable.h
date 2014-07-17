#ifndef PARSEABLE_H__
#define PARSEABLE_H__

#include <string>


class Parseable
{
private:
    std::string errorMsg;

protected:
    virtual void setErrorMessage(const std::string& msg) { errorMsg = msg; }
    virtual void appendErrorMessage(const std::string& msg) { errorMsg += msg; }
public:
    std::string getErrorMessage() { return errorMsg; }
};


#endif//PARSEABLE_H__
