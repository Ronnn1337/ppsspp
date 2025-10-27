#include "Common/Data/Text/I18n.h"
#include "Common/System/OSD.h"

I18NRepo g_i18nrepo;

I18NRepo::I18NRepo() {}

void I18NRepo::Clear() {}

bool I18NRepo::IniExists(const std::string &) const {
	return false;
}

bool I18NRepo::LoadIni(const std::string &, const Path &) {
	return false;
}

std::string I18NRepo::LanguageID() {
	return {};
}

std::shared_ptr<I18NCategory> I18NRepo::GetCategory(I18NCat category) {
	static std::shared_ptr<I18NCategory> dummy = std::make_shared<I18NCategory>();
	return category == I18NCat::NONE ? nullptr : dummy;
}

std::shared_ptr<I18NCategory> GetI18NCategory(I18NCat category) {
	static std::shared_ptr<I18NCategory> dummy = std::make_shared<I18NCategory>();
	return category == I18NCat::NONE ? nullptr : dummy;
}

I18NCategory::I18NCategory(const Section &) {}

void I18NCategory::Clear() {}

std::string_view I18NCategory::T(std::string_view key, std::string_view def) {
	return !def.empty() ? def : key;
}

const char *I18NCategory::T_cstr(const char *key, const char *def) {
	return def ? def : key;
}

void I18NCategory::SetMap(const std::map<std::string, std::string> &) {}

std::map<std::string, std::string> I18NCategory::Missed() const {
	return {};
}

OnScreenDisplay g_OSD;

OnScreenDisplay::~OnScreenDisplay() {}

void OnScreenDisplay::Show(OSDType, std::string_view, std::string_view, std::string_view, float, const char *) {}
void OnScreenDisplay::ShowOnOff(std::string_view, bool, float) {}
void OnScreenDisplay::Update() {}
void OnScreenDisplay::ShowAchievementUnlocked(int) {}
void OnScreenDisplay::ShowAchievementProgress(int, bool) {}
void OnScreenDisplay::ShowChallengeIndicator(int, bool) {}
void OnScreenDisplay::ShowLeaderboardTracker(int, std::string_view, bool) {}
void OnScreenDisplay::ShowLeaderboardStartEnd(std::string_view, std::string_view, bool) {}
void OnScreenDisplay::ShowLeaderboardSubmitted(std::string_view, std::string_view) {}
void OnScreenDisplay::SetProgressBar(std::string_view, std::string_view, float, float, float, float) {}
void OnScreenDisplay::RemoveProgressBar(std::string_view, bool, float) {}
void OnScreenDisplay::NudgeSidebar() {}
float OnScreenDisplay::SidebarAlpha() const { return 0.0f; }
void OnScreenDisplay::ClearAchievementStuff() {}
void OnScreenDisplay::SetClickCallback(const char *, void (*)(bool, void *), void *) {}
void OnScreenDisplay::SetFlags(const char *, OSDMessageFlags) {}
std::vector<OnScreenDisplay::Entry> OnScreenDisplay::Entries() { return {}; }
void OnScreenDisplay::ClickEntry(size_t, double) {}
