#include "AuthClient.h"

#if !defined(__WINNT__)
#include <sys/utsname.h>
#endif

using namespace ThorsAnvil::DB::Mongo::Auth;

/*
 * This class is a description of the driver library.
 * i.e. This library.
 * So set the name and version/
 */
THORS_MONGO_HEADER_ONLY_INCLUDE
Driver::Driver()
    : name("ThorsAnvil::Mongo::Driver")
    , version("v2.0")
{}

/*
 * This class is a description of the OS that we are running on.
 * **** Its a mess *
 *
 * Get the type/name/architecture/version of the OS.
 * The type is required (but it is just a string. No meaning is placed on it).
 *
 * All other values are optional.
 * Try and use meaningful values (probably used in logging on the Mongo end).
 */
THORS_MONGO_HEADER_ONLY_INCLUDE
OS::OS()
{
#ifdef __WINNT__
    // TODO This is wrong figure out correct values.
    // Fix: test/HandShakeTest.cpp: TEST(HandShakeTest, CreateOS) Currently  Line 83
    type         = "MSYS_NT";       // uname -s | awk -F- '{print $1}'
    name         = "Msys";          // unmae -o
    architecture = "x86_64";        // uname -m
    version      = "3.4.7.x86_64";  // uname -r
#else
    using UtsName = struct utsname;
    UtsName  buffer;
    int result = uname(&buffer);
    if (result != 0)
    {
        type = "unknown";
        return;
    }
    type         = buffer.sysname;
    architecture = buffer.machine;
    version      = buffer.release;
#if defined(__APPLE__)
    FILE* fp = popen("sw_vers -productName", "r");
#elif defined(__linux__)
    FILE* fp = popen("lsb_release -a", "r");
#endif
    if (fp == nullptr)
    {
        ThorsLogAndThrowFatal("ThorsAnvil::DB::Mongo::OS", "OS", "Can not get OS Information");
    }
    char pBuffer[100];
    std::string nameBuilder;
    while (fgets(pBuffer, sizeof(pBuffer), fp) != NULL)
    {
#if defined(__APPLE__)

        nameBuilder += pBuffer;
#elif defined(__linux__)
        char pValue[100];
        if (sscanf(pBuffer, " Distributor ID: %s", pValue) == 1)
        {
            nameBuilder = pValue;
        }
#endif
    }
    if (nameBuilder[nameBuilder.size() -1] == '\n')
    {
        nameBuilder.resize(nameBuilder.size() -1);
    }
    name = nameBuilder;
#endif

    if (name && name.value() == "") {
        name.reset();
    }
    if (architecture && architecture.value() == "") {
        architecture.reset();
    }
    if (version && version.value() == "") {
        version.reset();
    }
}

/*
 * The name of the application.
 * Can be anything just a string.
 * Passed in via the Client object.
 */
THORS_MONGO_HEADER_ONLY_INCLUDE
Application::Application(std::string const& application)
    : name(application)
{}

/*
 * All client information is created via this constructor (see above).
 * This info is simply to help Mongo in logging.
 * It has not real meaning.
 */
THORS_MONGO_HEADER_ONLY_INCLUDE
Client::Client(std::string const& application, std::optional<std::string> const& platformInfo)
    : application(application)
    , platform(platformInfo)
{
    if (platform && platform.value() == "") {
        platform.reset();
    }
}
