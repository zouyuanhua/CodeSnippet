

#ifndef FILE_CONSUMER_H
#define FILE_CONSUMER_H

#include "Log.h"

#include <fstream>


namespace FLog {

/**
 * Log consumer that writes the log events to a file.
 *
 * @file FileConsumer.h
 */
class FileConsumer : public LogConsumer {
public:
    //! Default constructor: filename = "output.log", append = false.
     FileConsumer();

    /** Constructor with parameters.
     * @param filename path of the output file where the log will be wrote.
     * @param append indicates if the consumer must append the content in the filename.
     */
     FileConsumer(const std::string &filename, bool append = false);

    /** \internal
     * Called by Log to ask us to consume the Entry.
     * @param Log::Entry to consume.
     */
     virtual void Consume(const Log::Entry&);

    virtual ~FileConsumer();

private:
    void PrintHeader(const Log::Entry&);
    void PrintContext(const Log::Entry&);

    std::string mOutputFile;
    std::ofstream mFile;
    bool mAppend;
};

} // namespace log

#endif // FILE_CONSUMER_H
