#ifndef ENCODERWORKER_H
#define ENCODERWORKER_H
#include <iostream>

class EncoderWorker
{
    struct streamInfo
    {
    public:
        streamInfo(std::string br, std::string rr, std::string on)
            : m_bitRate(br)
            , m_resolutionRatio(rr)
            , m_outputName(on)
        {
        }
        std::string m_bitRate;
        std::string m_resolutionRatio;
        std::string m_outputName;
    };
public:
    EncoderWorker(void);
    ~EncoderWorker(void);

    void start(void);
    void run(void);
private:
};

#endif

