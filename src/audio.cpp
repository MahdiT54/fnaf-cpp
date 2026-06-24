#include "audio.hpp"

#define MINIAUDIO_IMPLEMENTATION
#include "../third_party/miniaudio.h"

#include <filesystem>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

static ma_engine engine{};
static bool ready = false;

static std::filesystem::path executableDirectory()
{
#ifdef _WIN32
    wchar_t buf[MAX_PATH];
    const DWORD len = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    if (len > 0 && len < MAX_PATH)
        return std::filesystem::path(buf).parent_path();
#endif
    return std::filesystem::current_path();
}

static std::filesystem::path assetPath(const char *name)
{
    const auto filename = std::filesystem::path(name);
    const auto candidates = {
        executableDirectory() / "assets" / filename,
        std::filesystem::current_path() / "assets" / filename,
    };

    for (const auto &path : candidates)
    {
        if (std::filesystem::exists(path))
            return path;
    }

    return executableDirectory() / "assets" / filename;
}

void initAudio()
{
    if (ma_engine_init(nullptr, &engine) == MA_SUCCESS)
        ready = true;
}

void shutdownAudio()
{
    if (ready)
    {
        ma_engine_uninit(&engine);
        ready = false;
    }
}

void playDoorSound()
{
    if (!ready)
        return;
    ma_engine_play_sound(&engine, assetPath("SFXBible_12478.wav").string().c_str(), nullptr);
}
