#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <regex>
#include "glob.hpp"
static auto fserr = std::error_code();

void onLoaded() {
    // Match on a single pattern
    for (auto& p : glob::glob(getMod()->getResourcesDir().string() + "/*.*.*")) {
        auto name = p.filename().string();
        std::reverse(name.begin(), name.end());
        auto should_replace = false;
        for (auto& ch : name) {
            ch = (ch == '.' and should_replace) ? '/' : ch;
            should_replace = (ch == '.') ? true : should_replace;
        }
        std::reverse(name.begin(), name.end());
        auto todvde = std::filesystem::path(name);
        auto newp = p.parent_path() / todvde.parent_path();
        std::filesystem::create_directories(newp, fserr);
        std::filesystem::rename(p, newp / todvde.filename(), fserr);
    }
    CCFileUtils::sharedFileUtils()->addPriorityPath(getMod()->getResourcesDir().string().c_str());
}
$on_mod(Loaded) { onLoaded(); }