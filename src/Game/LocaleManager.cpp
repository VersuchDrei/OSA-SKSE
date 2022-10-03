#include "Game/LocaleManager.h"

#include <codecvt>

#define WIN32_LEAN_AND_MEAN

#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOSYSMETRICS

#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI
#define NOKERNEL
#define NOUSER
//#define NONLS
#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX

#include <Windows.h>

#undef GetEnvironmentVariable
#undef GetFileVersionInfo
#undef GetFileVersionInfoSize
#undef GetModuleFileName
#undef GetModuleHandle
#undef MessageBox
#undef OutputDebugString
#undef VerQueryValue

LocaleManager::LocaleManager() : _localizations_ENG(), _localizations_LOC() { LoadLocalizationStrings(); }

LocaleManager* LocaleManager::GetSingleton() {
    static LocaleManager singleton;
    return &singleton;
}

std::wstring LocaleManager::ConvertStringToWString(const std::string& a_str) {
    if (a_str.empty()) return std::wstring{};

    auto size =
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, a_str.c_str(), static_cast<int>(a_str.length()), nullptr, 0);
    bool err = size == 0;
    if (!err) {
        std::wstring strTo;
        strTo.resize(size);
        err = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, a_str.c_str(), static_cast<int>(a_str.length()),
                                  strTo.data(), size) == 0;
        if (!err) return strTo;
    }

    if (err) logger::error("MultiByteToWideChar failed with error code ({})", GetLastError());

    return std::wstring{};
}

std::string LocaleManager::ConvertWStringToString(const std::wstring& a_str) {
    if (a_str.empty()) return std::string{};

    auto size =
        WideCharToMultiByte(CP_UTF8, 0, a_str.c_str(), static_cast<int>(a_str.length()), nullptr, 0, nullptr, nullptr);
    bool err = size == 0;
    if (!err) {
        std::string strTo;
        strTo.resize(size);
        err = WideCharToMultiByte(CP_UTF8, 0, a_str.c_str(), static_cast<int>(a_str.length()), strTo.data(), size,
                                  nullptr, nullptr) == 0;
        if (!err) return strTo;
    }

    if (err) logger::error("WideCharToMultiByte failed with error code ({})", GetLastError());

    return std::string{};
}

std::wstring LocaleManager::GetLocalization(std::wstring a_key) { return GetLocalizationInternal(a_key); }

std::string LocaleManager::GetLocalization(std::string a_key) {
    auto str = ConvertStringToWString(a_key);
    str = GetLocalization(str);
    return ConvertWStringToString(str);
}

std::string LocaleManager::Translate(std::string a_key) {
    if (!a_key.empty() && a_key[0] == '$') {
        auto translation = GetLocalization(a_key);
        return translation;
    }

    return a_key;
}

void LocaleManager::SetOverride(std::string a_locale) {
    if (!a_locale.empty()) {
        auto locale = ConvertStringToWString(a_locale);
        _localeOverride = locale;
        if (LoadLocalizationStrings(true)) return;
    }

    _localeOverride = L"";
    LoadLocalizationStrings();
}

bool LocaleManager::FindFiles(const std::filesystem::path& a_path, const std::wregex& a_pattern, bool a_english) {
    std::error_code err;
    if (!std::filesystem::exists(a_path, err)) return false;

    std::filesystem::path fileName;
    for (auto& dirEntry : std::filesystem::directory_iterator(a_path)) {
        fileName = dirEntry.path().filename();
        if (std::regex_match(fileName.native(), a_pattern)) {
            ReadFromFile(dirEntry.path(), a_english);
        }
    }

    return true;
}

void LocaleManager::ReadFromFile(const std::filesystem::path& a_path, bool a_english) {
    constexpr auto CVT_MODE = std::codecvt_mode(std::little_endian | std::consume_header);
    constexpr auto NPOS = std::wstring::npos;

    auto& localizations = a_english ? _localizations_ENG : _localizations_LOC;
    std::wifstream inFile(a_path);
    inFile.imbue(std::locale(inFile.getloc(), new std::codecvt_utf16<wchar_t, 0x10FFFF, CVT_MODE>));  // UCS-2 LE w/ BOM
    std::wstring line;
    std::wstring key;
    std::wstring value;
    if (!inFile.is_open()) {
        logger::error("Failed to open file \"{}\"!\n", a_path.string().c_str());
        return;
    }

    while (std::getline(inFile, line)) {
        if (!line.empty() && line.back() == L'\r') {
            line.pop_back();  // discard carriage return
        }
        if (line.empty() || line.front() != L'$') {
            continue;
        }
        key.clear();
        value.clear();

        auto pos = line.find_first_of(L'\t');
        if (pos != NPOS) {
            key = line.substr(0, pos);
            value = line.substr(pos + 1);
        }

        if (!key.empty() && !value.empty()) {
            auto sanitizedKey = SanitizeKey(key);
            if (sanitizedKey) {
                key = std::move(*sanitizedKey);
            }
            localizations.insert(std::make_pair(std::move(key), std::move(value)));
        }
    }
}

bool LocaleManager::LoadLocalizationStrings(bool a_override) {
    _localizations_ENG.clear();
    _localizations_LOC.clear();

    constexpr wchar_t REGEX_PREFIX[] = L".*_";
    constexpr wchar_t ENGLISH[] = L"ENGLISH";
    constexpr wchar_t REGEX_POSTFIX[] = L"\\.txt$";
    constexpr auto REGEX_FLAGS = std::regex_constants::grep | std::regex_constants::icase;

    std::filesystem::path path("data/interface/translations");

    std::wstring pattern(REGEX_PREFIX);
    std::wstring wLanguage(ENGLISH);
    if (a_override) {
        wLanguage = _localeOverride;
    } else {
        auto setting = RE::GetINISetting("sLanguage:General");
        if (setting) {
            auto u8Language = setting->GetString();
            wLanguage = ConvertStringToWString(u8Language);
        }
    }
    pattern += wLanguage;
    pattern += REGEX_POSTFIX;
    std::wregex regex(pattern, REGEX_FLAGS);

    bool english = _wcsicmp(ENGLISH, wLanguage.c_str()) == 0;

    bool found = FindFiles(path, regex, english);
    if (!english) {
        pattern = REGEX_PREFIX;
        pattern += ENGLISH;
        pattern += REGEX_POSTFIX;
        regex.assign(pattern, REGEX_FLAGS);
        FindFiles(path, regex, true);
    }

    return found;
}

LocaleManager::LocalizationMap& LocaleManager::GetLocalizationMap() {
    return _localizations_LOC.empty() ? _localizations_ENG : _localizations_LOC;
}

std::wstring LocaleManager::GetLocalizationInternal(const std::wstring& a_key) {
    if (a_key.empty() || a_key[0] != L'$') return a_key;

    auto sanitizedKey = SanitizeKey(a_key);
    if (!sanitizedKey) return a_key;

    auto localization = FindLocalization(*sanitizedKey);
    if (!localization) return a_key;

    std::stack<size_type> stack;
    std::queue<std::wstring> queue;
    if (!GetNestedLocalizations(a_key, stack, queue)) return *localization;

    while (!stack.empty()) {
        stack.pop();
    }

    InsertLocalizations(*localization, stack, queue);
    return *localization;
}

std::optional<std::wstring> LocaleManager::SanitizeKey(std::wstring a_key) {
    std::stack<size_type> stack;
    for (size_type pos = 0; pos < a_key.size(); ++pos) {
        switch (a_key[pos]) {
            case L'{':
                stack.push(pos);
                break;
            case L'}': {
                switch (stack.size()) {
                    case 0:
                        return std::nullopt;
                    case 1: {
                        size_type last = stack.top();
                        stack.pop();
                        auto off = last + 1;
                        auto count = pos - last - 1;
                        if (count > 0) {
                            a_key.replace(off, count, L"");
                        }
                        pos = off;
                    } break;
                    default:
                        stack.pop();
                }
                break;
            }
        }
    }

    if (!a_key.empty() && a_key.back() == L'\r') a_key.pop_back();

    return std::make_optional(a_key);
}

std::optional<std::wstring> LocaleManager::FindLocalization(const std::wstring& a_key) {
    auto& localizations = GetLocalizationMap();
    auto it = localizations.find(a_key);
    if (it == localizations.end()) {
        if (&localizations != &_localizations_ENG) {
            it = _localizations_ENG.find(a_key);
            if (it == _localizations_ENG.end()) {
                return std::nullopt;
            }
        } else {
            return std::nullopt;
        }
    }

    return std::make_optional(it->second);
}

bool LocaleManager::GetNestedLocalizations(const std::wstring& a_key, std::stack<size_type>& a_stack,
                                           std::queue<std::wstring>& a_queue) {
    for (size_type pos = 0; pos < a_key.size(); ++pos) {
        switch (a_key[pos]) {
            case L'{':
                a_stack.push(pos);
                break;
            case L'}': {
                switch (a_stack.size()) {
                    case 0:
                        return false;
                    case 1: {
                        size_type last = a_stack.top();
                        a_stack.pop();
                        auto off = last + 1;
                        auto count = pos - last - 1;
                        if (count == 0) {
                            return false;  // nothing to replace {} with
                        }
                        auto subStr = a_key.substr(off, count);
                        a_queue.push(GetLocalizationInternal(subStr));
                    } break;
                    default:
                        a_stack.pop();
                }
                break;
            }
        }
    }

    return true;
}

bool LocaleManager::InsertLocalizations(std::wstring& a_localization, std::stack<size_type>& a_stack,
                                        std::queue<std::wstring>& a_queue) {
    for (size_type pos = 0; pos < a_localization.size(); ++pos) {
        switch (a_localization[pos]) {
            case L'{':
                a_stack.push(pos);
                break;
            case L'}': {
                if (a_stack.empty() || a_queue.empty()) return false;

                size_type beg = a_stack.top();
                a_stack.pop();
                auto subStr = a_queue.front();
                a_queue.pop();

                a_localization.replace(beg, pos - beg + 1, subStr);
                pos = beg;
            } break;
        }
    }

    return true;
}