

#ifndef _LOG_LOG_H_
#define _LOG_LOG_H_

#include "DBQueue.h"
#include <thread>
#include <sstream>
#include <atomic>
#include <regex>


// Logging API:

//! Logs an info message. Disable it through Log::SetVerbosity, #define LOG_NO_INFO, or being in a release branch
#define logInfo(cat, msg) logInfo_(cat, msg)
//! Logs a warning. Disable reporting through Log::SetVerbosity or #define LOG_NO_WARNING
#define logWarning(cat, msg) logWarning_(cat, msg)
//! Logs an error. Disable reporting through #define LOG_NO_ERROR
#define logError(cat, msg) logError_(cat, msg)


namespace FLog {

class LogConsumer;

/**
 * Logging utilities.
 * Logging is accessed through the three macros above, and configuration on the log output
 * can be achieved through static methods on the class. Logging at various levels can be
 * disabled dynamically (through the Verbosity level) or statically (through the LOG_NO_[VERB]
 * macros) for maximum performance.
 * @ingroup COMMON_MODULE
 */
class Log
{
    public:
        /**
        * Types of log entry.
        * * Error: Maximum priority. Can only be disabled statically through LOG_NO_ERROR.
        * * Warning: Medium priority.  Can be disabled statically and dynamically.
        * * Info: Low priority. Useful for debugging. Disabled by default on release branches.
        */
        enum Kind
        {
            Error,
            Warning,
            Info,
        };

        /**
        * Registers an user defined consumer to route log output.
        * There is a default stdout consumer active as default.
        * @param consumer r-value to a consumer unique_ptr. It will be invalidated after the call.
        */
         static void RegisterConsumer(std::unique_ptr<LogConsumer>&& consumer);

        //! Removes all registered consumers, including the default stdout.
         static void ClearConsumers();

        //! Enables the reporting of filenames in log entries. Disabled by default.
         static void ReportFilenames(bool);

        //! Enables the reporting of function names in log entries. Enabled by default when supported.
         static void ReportFunctions(bool);

        //! Sets the verbosity level, allowing for messages equal or under that priority to be logged.
         static void SetVerbosity(Log::Kind);

        //! Returns the current verbosity level.
         static Log::Kind GetVerbosity();

        //! Sets a filter that will pattern-match against log categories, dropping any unmatched categories.
         static void SetCategoryFilter(const std::regex&);

        //! Sets a filter that will pattern-match against filenames, dropping any unmatched categories.
         static void SetFilenameFilter(const std::regex&);

        //! Sets a filter that will pattern-match against the provided error string, dropping any unmatched categories.
         static void SetErrorStringFilter(const std::regex&);

        //! Returns the logging engine to configuration defaults.
         static void Reset();

        //! Waits until no more log info is availabel
         static void Flush();

        //! Stops the logging thread. It will re-launch on the next call to a successful log macro.
         static void KillThread();

        // Note: In VS2013, if you're linking this class statically, you will have to call KillThread before leaving
        // main, due to an unsolved MSVC bug.

        struct Context
        {
            const char* filename;
            int line;
            const char* function;
            const char* category;
        };

        struct Entry
        {
            std::string message;
            Log::Context context;
            Log::Kind kind;
            std::string timestamp;
        };

        /**
        * Not recommended to call this method directly! Use the following macros:
        *  * logInfo(cat, msg);
        *  * logWarning(cat, msg);
        *  * logError(cat, msg);
        */
         static void QueueLog(
                const std::string& message,
                const Log::Context&,
                Log::Kind);

    private:
        struct Resources
        {
            DBQueue<Entry> mLogs;
            std::vector<std::unique_ptr<LogConsumer>> mConsumers;
            std::unique_ptr<std::thread> mLoggingThread;

            // Condition variable segment.
            std::condition_variable mCv;
            std::mutex mCvMutex;
            bool mLogging;
            bool mWork;

            // Context configuration.
            std::mutex mConfigMutex;
            bool mFilenames;
            bool mFunctions;
            std::unique_ptr<std::regex> mCategoryFilter;
            std::unique_ptr<std::regex> mFilenameFilter;
            std::unique_ptr<std::regex> mErrorStringFilter;

            std::atomic<Log::Kind> mVerbosity;

            Resources();

            ~Resources();
        };

        static struct Resources mResources;

        // Applies transformations to the entries compliant with the options selected (such as
        // erasure of certain context information, or filtering by category. Returns false
        // if the log entry is blacklisted.
        static bool Preprocess(Entry&);

        static void LaunchThread();

        static void Run();

        static void GetTimestamp(std::string&);
};

/**
 * Consumes a log entry to output it somewhere.
 */
class LogConsumer
{
    public:
        virtual ~LogConsumer(){};

        virtual void Consume(const Log::Entry&) = 0;

    protected:
        void PrintTimestamp(
                std::ostream& stream,
                const Log::Entry&,
                bool color) const;

        void PrintHeader(
                std::ostream& stream,
                const Log::Entry&,
                bool color) const;

        void PrintContext(
                std::ostream& stream,
                const Log::Entry&,
                bool color) const;

        void PrintMessage(
                std::ostream& stream,
                const Log::Entry&,
                bool color) const;

        void PrintNewLine(
                std::ostream& stream,
                bool color) const;
};

#if defined(WIN32)
#define __func__ __FUNCTION__
#endif

#ifndef LOG_NO_ERROR
#define logError_(cat, msg)                                                                          \
    {                                                                                                \
        std::stringstream ss;                                                                        \
        ss << msg;                                                                                   \
        Log::QueueLog(ss.str(), Log::Context{__FILE__, __LINE__, __func__, #cat}, Log::Kind::Error); \
    }
#else
#define logError_(cat, msg)
#endif

#ifndef LOG_NO_WARNING
#define logWarning_(cat, msg)                                                                              \
    {                                                                                                      \
        if (Log::GetVerbosity() >= Log::Kind::Warning)                                                     \
        {                                                                                                  \
            std::stringstream ss;                                                                          \
            ss << msg;                                                                                     \
            Log::QueueLog(ss.str(), Log::Context{__FILE__, __LINE__, __func__, #cat}, Log::Kind::Warning); \
        }                                                                                                  \
    }
#else
#define logWarning_(cat, msg)
#endif

#ifndef LOG_NO_INFO
#define logInfo_(cat, msg)                                                                              \
    {                                                                                                   \
        if (Log::GetVerbosity() >= Log::Kind::Info)                                                     \
        {                                                                                               \
            std::stringstream ss;                                                                       \
            ss << msg;                                                                                  \
            Log::QueueLog(ss.str(), Log::Context{__FILE__, __LINE__, __func__, #cat}, Log::Kind::Info); \
        }                                                                                               \
    }
#else
#define logInfo_(cat, msg)
#endif

} // namespace log

#endif
