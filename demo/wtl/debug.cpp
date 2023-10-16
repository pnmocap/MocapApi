#include "stdafx.h"
#include "debug.h"
#include <spdlog/sinks/msvc_sink.h>

namespace debug {
    std::shared_ptr<spdlog::logger> log() 
    {
        static std::shared_ptr<spdlog::logger> _logger;
        if (nullptr != _logger) {
            return _logger;
        }
        else {
            spdlog::sinks_init_list sinks = {
                std::make_shared<spdlog::sinks::msvc_sink_mt>()
            };
            _logger = std::make_shared<spdlog::logger>("MocapConsole", sinks);
            return _logger;
        }
    }

    bool verify(bool condition, const char * msg, uint32_t line, const char* file) 
    {
        if (condition) {
        }
        else {
            log()->critical("Verify failed: \"{}\" {} in {}.", msg, line, file);
        }
        return condition;
    }

    bool mcp_verify(MocapApi::EMCPError err, uint32_t line, const char* file) 
    {
        if (err == MocapApi::Error_None) {
            return true;
        }
        else {
            auto msg = fmt::format("MocapApi Error {}", static_cast<uint32_t>(err));
            return verify(false, msg.data(), line, file);
        }
    }

}