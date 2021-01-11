
#ifndef STDOUT_CONSUMER_H
#define STDOUT_CONSUMER_H

#include "Log.h"


namespace FLog {

class StdoutConsumer: public LogConsumer {
public:
    virtual ~StdoutConsumer() {};
     virtual void Consume(const Log::Entry&);

private:
    void PrintHeader(const Log::Entry&) const;
    void PrintContext(const Log::Entry&) const;
};

} // namespace FLog

#endif
