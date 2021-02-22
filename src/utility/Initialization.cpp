#include "utility/Initialization.hpp"

// project
#include "utility/Resources.hpp"

// libraries
#if SPDLOG_VERSION >= 10600 // v1.6.0
#include "spdlog/cfg/env.h"
#include "spdlog/cfg/helpers.h"
#else
#include "depthai-shared/log/LogLevel.hpp"
#endif
#include "spdlog/details/os.h"
#include "spdlog/spdlog.h"

// For easier access to dai namespaced symbols
namespace dai {

// Anonymous namespace to hide 'Preloader' symbol and variable as its not needed to be visible to other compilation units
namespace {

// Doing early static initialization hits this stage faster than some libraries initialize their global static members

// Preloader uses static global object constructor (works only for shared libraries)
// to execute some code upon final executable launch  or library import
// Preloader
// struct Preloader {
//     Preloader(){
//         initialize();
//     }
// } preloader;

}  // namespace

bool initialize() {
    // atomic bool for checking whether depthai was already initialized
    static std::atomic<bool> initialized{false};

    if(initialized.exchange(true)) return true;

#if SPD_VERSION >= 10601 //v1.6.1
    // Set global logging level from ENV variable 'DEPTHAI_LEVEL'
    // Taken from spdlog, to replace with DEPTHAI_LEVEL instead of SPDLOG_LEVEL
    // spdlog::cfg::load_env_levels();
    auto env_val = spdlog::details::os::getenv("DEPTHAI_LEVEL");
    if(!env_val.empty()) {
        spdlog::cfg::helpers::load_levels(env_val);
    } else {
        // Otherwise set default level to WARN
        spdlog::set_level(spdlog::level::warn);
    }
#else
    if(const char* env_val = std::getenv("DEPTHAI_LEVEL")) {
        spdlog::set_level(spdlog::level::from_str(std::string(env_val)));
        dai::CurrentLogLevel = spdlog::level::from_str(std::string(env_val));
    } else {
        // Otherwise set default level to WARN
        spdlog::set_level(spdlog::level::warn);
    }
#endif

    // auto debugger_val = spdlog::details::os::getenv("DEPTHAI_DEBUGGER");
    // if(!debugger_val.empty()){
    //    // TODO(themarpe) - instruct Device class that first available device is also a booted device
    // }

    // Executed at library load time

    // Preload Resources (getting instance causes some internal lazy loading to start)
    Resources::getInstance();

    spdlog::debug("Initialize - finished");

    return true;
}

}  // namespace dai
