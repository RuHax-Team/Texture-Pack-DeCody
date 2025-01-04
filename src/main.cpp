#include <Geode/Geode.hpp>
#include <chrono>

using namespace geode::prelude;

//randomly
#if 1
#include  <random>
#include  <iterator>
template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
};
template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}
bool rndb(int rarity = 1) {
    auto varsVec = std::vector<bool>();
    varsVec.push_back(true);
    while (rarity > 0) {
        rarity = rarity - 1;
        varsVec.push_back(false);
    }
    auto rtn = *select_randomly(varsVec.begin(), varsVec.end());
    //log::debug("{}({}) = {} of {}", __func__, variants, rtn, varsVec);
    return rtn;
}
#endif

#include <regex>
#include "glob.hpp"
static auto fserr = std::error_code();

bool isSnowdays() {
    time_t now = time(0);
    tm* gmt = gmtime(&now);
    if (gmt == nullptr) {
        return false;
    }

    int month = gmt->tm_mon;

    std::vector<int> snowMonths = { 11, 0, 1 };
    return std::find(snowMonths.begin(), snowMonths.end(), month) != snowMonths.end();
}

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
    CCFileUtils::sharedFileUtils()->addPriorityPath(
        getMod()->getResourcesDir().string().c_str()
    );
    if (isSnowdays()) CCFileUtils::sharedFileUtils()->addPriorityPath(
        (getMod()->getResourcesDir() / "snow_var").string().c_str()
    );
}
$on_mod(Loaded) { onLoaded(); }

#include <Geode/modify/MenuLayer.hpp>
class $modify(MenuLayerExt, MenuLayer) {
    $override bool init() {
        if (!MenuLayer::init()) return false;

        //remove unused links
        if (auto node = this->getChildByIDRecursive("facebook-button"))
            node->setVisible(0);
        if (auto node = this->getChildByIDRecursive("twitter-button"))
            node->setVisible(0);
        if (auto node = this->getChildByIDRecursive("youtube-button"))
            node->setVisible(0);
        if (auto node = this->getChildByIDRecursive("twitch-button"))
            node->setVisible(0);
        if (auto node = this->getChildByIDRecursive("more-games-button")) {
            node->setContentSize(CCPointZero);
            node->setVisible(0);
        }

        if (isSnowdays()) if (auto node = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("main-title"))) {
            auto frames = Ref(CCArray::create());
            if (auto fr = CCSpriteFrameCache::get()->spriteFrameByName("logodpt_01.png")) frames->addObject(fr);
            if (auto fr = CCSpriteFrameCache::get()->spriteFrameByName("logodpt_02.png")) frames->addObject(fr);
            if (auto fr = CCSpriteFrameCache::get()->spriteFrameByName("logodpt_03.png")) frames->addObject(fr);
            if (auto fr = CCSpriteFrameCache::get()->spriteFrameByName("logodpt_04.png")) frames->addObject(fr);
            if (auto fr = CCSpriteFrameCache::get()->spriteFrameByName("logodpt_05.png")) frames->addObject(fr);
            auto anim = CCAnimation::createWithSpriteFrames(frames, 0.5f);
            CCAnimate* theAnim = CCAnimate::create(anim);
            if (isSnowdays()) node->runAction(CCRepeatForever::create(theAnim));
        }

        if (isSnowdays()) {
            auto snow = CCParticleSnow::create();
            snow->setID("snow"_spr);
            snow->setBlendAdditive(1);
            this->addChild(snow, 999, 6539);
        }

        return true;
    }
};

#include <Geode/modify/LoadingLayer.hpp>
class $modify(LoadingLayerExt, LoadingLayer) {
    $override bool init(bool p0) {
        if (!LoadingLayer::init(p0)) return false;

        if (auto nodeToSetup = this->getChildByIDRecursive("progress-slider")) {
            nodeToSetup->setPosition(CCPointMake(105.f, 12.f));
            nodeToSetup->setScale(0.9f);
        }

        if (auto nodeToSetup = this->getChildByIDRecursive("gd-logo")) {
            nodeToSetup->setPosition(CCPointMake(0.f, this->getContentHeight()));
            nodeToSetup->setAnchorPoint(CCPointMake(0.f, 1.f));
            nodeToSetup->setScale(0.7f);
        }

        if (auto nodeToSetup = this->getChildByIDRecursive("fmod-logo")) nodeToSetup->setVisible(0);

        if (auto nodeToSetup = this->getChildByIDRecursive("robtop-logo")) {
            nodeToSetup->setPosition(CCPointMake(this->getContentWidth(), 0.f));
            nodeToSetup->setAnchorPoint(CCPointMake(1.05f, -0.1f));
            nodeToSetup->setScale(0.825f);
        }

        if (auto nodeToSetup = this->getChildByIDRecursive("bg-texture")) nodeToSetup->setZOrder(-10);

        CCSpriteFrameCache::get()->addSpriteFramesWithFile("GJ_GameSheet02.plist", "GJ_GameSheet02.png");
        CCSpriteFrameCache::get()->addSpriteFramesWithFile("GJ_GameSheet03.plist", "GJ_GameSheet03.png");

        auto gr = GJGroundLayer::create(0, 0);
        gr->setPosition(CCPointMake(0.f, 88.000f));
        this->addChild(gr, -1);

        auto icons = std::vector<SimplePlayer*>();
        {
            auto DeCody = SimplePlayer::create(123);
            DeCody->setColors({ 90,90,90 }, { 255,255,255 });
            DeCody->setID("DeCody");
            icons.push_back(DeCody);

            auto MaFFaka = SimplePlayer::create(1);
            MaFFaka->setColors(
                GameManager::get()->colorForIdx(10),
                GameManager::get()->colorForIdx(25)
            );
            MaFFaka->setGlowOutline(
                GameManager::get()->colorForIdx(9)
            );
            MaFFaka->setID("MaFFaka");
            icons.push_back(MaFFaka);

            auto retromanGD = SimplePlayer::create(142);
            retromanGD->setColors(
                GameManager::get()->colorForIdx(18),
                GameManager::get()->colorForIdx(12)
            );
            retromanGD->setID("retromanGD");
            icons.push_back(retromanGD);

            auto Felix = SimplePlayer::create(19);
            Felix->setColors(
                { 255, 75, 0 },
                { 255, 255, 255 }
            );
            Felix->setGlowOutline(
                { 255, 255, 255 }
            );
            Felix->setID("Felix");
            icons.push_back(Felix);

            auto Guschin = SimplePlayer::create(193);
            Guschin->setColors(
                { 125, 125, 255 },
                { 255, 255, 255 }
            );
            Guschin->setID("Guschin");
            icons.push_back(Guschin);

            auto Sirena_rakitic = SimplePlayer::create(123);
            Sirena_rakitic->setColors(
                { 0, 200, 255 },
                { 255, 125, 125 }
            );
            Sirena_rakitic->setGlowOutline(
                { 255, 125, 125 }
            );
            Sirena_rakitic->setID("Sirena_rakitic");
            icons.push_back(Sirena_rakitic);
        };

        auto icon = *select_randomly(icons.begin(), icons.end());
        if (icon->m_iconRequestID = 123) {

        }
        icon->setPosition(this->getContentSize() / 2);
        icon->setPositionY(104.000f);
        this->addChild(icon);
        
        auto asd = { "sosal", "1803", "2203", "1337", "618", "ruhax", "iconpatch spizdili :D" };
        auto icontitle = CCLabelBMFont::create(
            rndb() ? icon->getID().c_str() : *select_randomly(asd.begin(), asd.end()),
            "chatFont.fnt"
        );
        icontitle->setPosition(this->getContentSize() / 2);
        icontitle->setPositionY(130.000f);
        this->addChild(icontitle);

        return true;
    }
};

#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
class $modify(CCMenuItemSpriteExtraExt, CCMenuItemSpriteExtra) {
    $override void selected() {
        if (auto spr = typeinfo_cast<CCSprite*>(this->getNormalImage())) {
            spr->setCascadeColorEnabled(1);
            spr->setCascadeOpacityEnabled(1);
            if (spr->getColor() == ccWHITE) {
                this->m_animationEnabled = this->m_animationType == MenuAnimationType::Move;
                this->m_colorEnabled = this->m_animationType == MenuAnimationType::Scale;
            };
        }
        FMODAudioEngine::get()->playEffect(".btn_click");
        return CCMenuItemSpriteExtra::selected();
    }
};

#include <Geode/modify/CCApplication.hpp>
class $modify(CCApplicationLinksReplace, CCApplication) {
    $override void openURL(const char* url) {
        url = string::replace(url, "https://www.robtopgames.com", "https://t.me/theguschin").c_str();
        url = string::replace(url, "https://discord.com/invite/geometrydash", "https://discord.com/invite/B39mTe48tY").c_str();
        return CCApplication::openURL(url);
    }
};

//#include <Geode/modify/CCSprite.hpp>
//class $modify(SpecialSprites, CCSprite) {
//    static void onModify(auto & self) {
//        auto names = {
//            "cocos2d::CCSprite::create",
//            "cocos2d::CCSprite::createWithSpriteFrameName",
//        };
//        for (auto name : names) if (!self.setHookPriorityPost(name, Priority::Last)) {
//            log::error("Failed to set hook priority for {}.", name);
//        }
//    }
//    $override static CCSprite* create(const char* pszFileName) {
//        return CCSprite::create("diffIcon_10_btn_001.png");
//    }
//    $override static CCSprite* createWithSpriteFrameName(const char* pszSpriteFrameName) {
//        return CCSprite::create("diffIcon_10_btn_001.png");
//    }
//};