#ifndef LOGGING_H__
#define LOGGING_H__


#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/filesystem/path.hpp>


namespace loglevel
{
    enum SeverityLevel
    {
        debug,
        info,
        warning,
        error,
    };
}


#endif//LOGGING_H__
