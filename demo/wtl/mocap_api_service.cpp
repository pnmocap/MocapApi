#include "stdafx.h"
#include "mocap_api_service.h"
#include <cstdlib>
#include <filesystem>
#include <boost/dll.hpp>
#include <spdlog/spdlog.h>
#include "w_a_conv.h"

namespace {
    using MCPGetGenericInterfaceProc = MocapApi::EMCPError(*)(const char*, void**);
    using MCPGetMocapApiVersionProc = void (*)(uint32_t*, uint32_t*,uint32_t*, uint32_t*);
    using MCPGetMocapApiVersionStringProc = const char* (*)(void);

    class WinVerQuery
    {
        struct LanguageAndCodePage {
            WORD language;
            WORD code_page;
        };

    public:
        explicit WinVerQuery(HMODULE hModule)
        {
            HRSRC hRSRC = ::FindResourceW(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
            if (nullptr == hRSRC) {
                throw std::runtime_error("");
            }
            DWORD data_size = ::SizeofResource(hModule, hRSRC);
            HGLOBAL hres = ::LoadResource(hModule, hRSRC);
            if (nullptr == hres) {
                throw std::runtime_error("");
            }
            void* resource = ::LockResource(hres);
            if (!resource) {
                throw std::runtime_error("");
            }
            _resData = std::string_view(reinterpret_cast<char*>(resource), data_size);

            static constexpr wchar_t kTranslation[] = L"\\VarFileInfo\\Translation";
            LPVOID translate = nullptr;
            UINT dummy_size;
            if (!::VerQueryValue(resource, kTranslation, &translate, &dummy_size)) {
                throw std::runtime_error("");
            }
            _languageAndCodePage = static_cast<LanguageAndCodePage*>(translate);
        }

        std::wstring_view GetValue(LPCWSTR key)
        {
            const struct LanguageAndCodePage lang_codepages[] = {
                // Use the language and codepage from the DLL.
                {_languageAndCodePage->language, _languageAndCodePage->code_page},
                // Use the default language and codepage from the DLL.
                {::GetUserDefaultLangID(), _languageAndCodePage->code_page},
                // Use the language from the DLL and Latin codepage (most common).
                {_languageAndCodePage->language, 1252},
                // Use the default language and Latin codepage (most common).
                {::GetUserDefaultLangID(), 1252},
            };

            std::wstring_view val;
            for (const auto& lang_codepage : lang_codepages) {
                wchar_t sub_block[MAX_PATH];
                _snwprintf_s(sub_block, MAX_PATH, MAX_PATH,
                    L"\\StringFileInfo\\%04x%04x\\%ls", lang_codepage.language,
                    lang_codepage.code_page, key);
                LPVOID value_ptr = nullptr;
                uint32_t size;
                BOOL r = ::VerQueryValue(_resData.data(), sub_block, &value_ptr, &size);
                if (r && value_ptr && size) {
                    val = std::wstring_view(reinterpret_cast<LPCWSTR>(value_ptr), size);
                    break;
                }
            }
            return val;
        }

        VS_FIXEDFILEINFO* GetVsFixedFileInfo()
        {
            static constexpr wchar_t kRoot[] = L"\\";
            LPVOID fixed_file_info = nullptr;
            UINT dummy_size;
            if (!::VerQueryValue(_resData.data(), kRoot, &fixed_file_info, &dummy_size)) {
                throw std::runtime_error("");
            }
            return static_cast<VS_FIXEDFILEINFO*>(fixed_file_info);
        }

    private:
        std::string_view _resData;
        const LanguageAndCodePage* _languageAndCodePage;
    };

    class PMocapApiService
        : public MocapApiService
    {
    public:
        PMocapApiService()
            : _dll()
            , _pfGetGenericInterface(nullptr)
            , _pfGetMocapApiVersionProc(nullptr)
            , _pfGetMocapApiVersionStringProc(nullptr)
            , _winVerQuery()
            , _verString()
            , _mocapApiVersion()
        {
            auto path_ = std::getenv("MOCAP_API_PATH");
            if (nullptr != path_) {
                _dll.load(path_);
            }
            else {
                std::wstring exeFilePath;
                exeFilePath.resize(MAX_PATH);
                exeFilePath.resize(::GetModuleFileNameW(_Module.GetModuleInstance(),
                    exeFilePath.data(), exeFilePath.size()));
                std::filesystem::path path_(exeFilePath);
                path_.replace_filename(L"MocapApi.dll");
                _dll.load(path_);
            }
            _pfGetGenericInterface = _dll.get<MocapApi::EMCPError(const char*, void**)>("MCPGetGenericInterface");
        }

        ~PMocapApiService()
        {}

        void* GetInterfaceInternal(const char* pchInterfaceVersion) override final
        {
            void* pInterface = nullptr;
            auto err = _pfGetGenericInterface(pchInterfaceVersion, &pInterface);
            return pInterface;
        }

        const MocapApi::MCPMocapApiVersion* mocapApiVersion() override final
        {
            if (nullptr != _mocapApiVersion) {
                return _mocapApiVersion.get();
            }
            else {
                return mocapApiVersion0();
            }
        }

        const wchar_t* mocapApiVersionString() override final
        {
            if (nullptr != _verString) {
                return _verString->data();
            }
            else {
                QueryVersionString();
                return _verString->data();
            }
        }
        boost::dll::shared_library _dll;
        MCPGetGenericInterfaceProc _pfGetGenericInterface;
        MCPGetMocapApiVersionProc _pfGetMocapApiVersionProc;
        MCPGetMocapApiVersionStringProc _pfGetMocapApiVersionStringProc;
        std::unique_ptr<std::wstring> _verString;
        std::unique_ptr<MocapApi::MCPMocapApiVersion> _mocapApiVersion;
        std::unique_ptr<WinVerQuery> _winVerQuery;

        void QueryVersion()
        {
            if (nullptr != _winVerQuery) {
                ;
            }
            else {
                _winVerQuery = std::make_unique<WinVerQuery>(_dll.native());
            }
            _mocapApiVersion = std::make_unique<MocapApi::MCPMocapApiVersion>();
            auto ffi = _winVerQuery->GetVsFixedFileInfo();
            _mocapApiVersion->_major = HIWORD(ffi->dwFileVersionMS);
            _mocapApiVersion->_minor = LOWORD(ffi->dwFileVersionMS);
            _mocapApiVersion->_build = HIWORD(ffi->dwFileVersionLS);
            auto val = _winVerQuery->GetValue(L"FileVersion");
            std::wstring hash;
            auto iterBeg = val.rbegin();
            if (*iterBeg == L'\0') {
                ++iterBeg;
            }
            auto iter = std::find(iterBeg, val.rend(), L'.');
            std::copy(iter.base(), iterBeg.base(),
                std::back_inserter(hash));
            wchar_t* endPtr = nullptr;
            _mocapApiVersion->_revision = std::wcstoull(hash.data(), &endPtr, 16);
        }

        void QueryVersionString()
        {
            if (_pfGetMocapApiVersionStringProc) {
                _verString = std::make_unique<std::wstring>(
                    AToW(_pfGetMocapApiVersionStringProc()));
            }
            else if (_dll.has("MCPGetMocapApiVersionString")) {
                _pfGetMocapApiVersionStringProc = _dll.get<const char* (void)>("MCPGetMocapApiVersionString");
                _verString = std::make_unique<std::wstring>(
                    AToW(_pfGetMocapApiVersionStringProc()));
            }
            else if (nullptr != _winVerQuery) {
                _verString = std::make_unique<std::wstring>(
                    _winVerQuery->GetValue(L"FileVersion"));
            }
            else {
                _winVerQuery = std::make_unique<WinVerQuery>(_dll.native());
                _verString = std::make_unique<std::wstring>(
                    _winVerQuery->GetValue(L"FileVersion"));
            }
        }

        const MocapApi::MCPMocapApiVersion* mocapApiVersion0()
        {
            if (nullptr != _mocapApiVersion) {
                return _mocapApiVersion.get();
            }
            else if (_pfGetMocapApiVersionProc) {
                _mocapApiVersion = std::make_unique<MocapApi::MCPMocapApiVersion>();
                _pfGetMocapApiVersionProc(&_mocapApiVersion->_major,
                    &_mocapApiVersion->_minor,
                    &_mocapApiVersion->_build,
                    &_mocapApiVersion->_revision);
                return _mocapApiVersion.get();
            }
            else if (_dll.has("MCPGetMocapApiVersion")) {
                _pfGetMocapApiVersionProc = _dll.get<void(uint32_t*,uint32_t*,uint32_t*,uint32_t*)>("MCPGetMocapApiVersion");;
                _mocapApiVersion = std::make_unique<MocapApi::MCPMocapApiVersion>();
                _pfGetMocapApiVersionProc(&_mocapApiVersion->_major,
                    &_mocapApiVersion->_minor,
                    &_mocapApiVersion->_build,
                    &_mocapApiVersion->_revision);
                return _mocapApiVersion.get();
            }
            else {
                QueryVersion();
                return _mocapApiVersion.get();
            }
        }
    };
}

MocapApiService& MocapApiService::Get(void)
{
    static PMocapApiService theInstance;
    return theInstance;
}