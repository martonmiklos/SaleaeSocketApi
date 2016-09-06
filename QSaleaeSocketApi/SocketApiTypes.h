#ifndef SALEAE_SOCKETAPI_TYPES_H
#define SALEAE_SOCKETAPI_TYPES_H


#include <QtGlobal>

#include <QException>
#include <QList>
#include <QObject>
#include <QString>


namespace SaleaeSocketApi
{

class SampleRate
{
public:
    SampleRate(int digital, int analog) : AnalogSampleRate(analog), DigitalSampleRate(digital) {}

    int AnalogSampleRate;// { get; set; }
    int DigitalSampleRate;// { get; set; }

    bool operator==( SampleRate b )
    {
        // Return true if the fields match:
        return this->AnalogSampleRate == b.AnalogSampleRate && this->DigitalSampleRate == b.DigitalSampleRate;
    }

    bool operator!=( SampleRate b )
    {
        return !( *this == b );
    }


};

struct Analyzer
{
    QString AnalyzerType;// { get; set; }
    int Index;// { get; set; }
};

class SaleaeSocketApiException : QException
{
public:
    SaleaeSocketApiException()
        : QException()
    {
    }

    SaleaeSocketApiException( QString message )
        : QException(),
          m_message(message)
    {

    }

private:
    QString m_message;
};
}


#endif
