#pragma once
#include <stdexcept>
#include <cstdint>
#include <spdlog/spdlog.h>
#include <MocapApi.h>

namespace debug{

    std::shared_ptr<spdlog::logger> log();

    bool verify(bool , const char * msg, uint32_t line, const char* file);
    bool mcp_verify(MocapApi::EMCPError err, uint32_t line, const char* file);
}

#define MCP_VERIFY(e)   \
    if(debug::mcp_verify(e, __LINE__, __FILE__)){\
    }\
    else {\
        throw std::runtime_error("");\
    }
#define VERIFY(e)\
if(debug::verify(e, #e, __LINE__, __FILE__)){\
    ;\
}\
else {\
    throw std::runtime_error("");\
}