#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Cached so the hook below doesn't do a settings lookup on every collision.
static bool g_noclip = false;

$execute {
    g_noclip = Mod::get()->getSettingValue<bool>("enabled");

    // Keep the cache in sync when the setting is changed from the mod settings UI.
    listenForSettingChanges<bool>("enabled", [](bool value) {
        g_noclip = value;
    });

    // Toggle keybind (default: 6).
    listenForKeybindSettingPresses("toggle-keybind", [](Keybind const&, bool down, bool repeat, double) {
        if (!down || repeat) return false;

        bool now = !Mod::get()->getSettingValue<bool>("enabled");
        Mod::get()->setSettingValue<bool>("enabled", now);
        g_noclip = now;

        Notification::create(
            now ? "Noclip: ON" : "Noclip: OFF",
            now ? NotificationIcon::Success : NotificationIcon::Info,
            0.6f
        )->show();
        return true;
    });
}

class $modify(NoclipPlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        // Let the game's own anti-cheat spike through, block every other death.
        if (g_noclip && object != m_anticheatSpike) return;
        PlayLayer::destroyPlayer(player, object);
    }
};
