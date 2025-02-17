#pragma once

#include "JsonHelpers.h"

#include <common/SettingsAPI/settings_helpers.h>
#include <common/utils/json.h>
#include <mutex>

#include <string>
#include <unordered_map>
#include <optional>
#include <vector>
#include <winnt.h>
#include <FancyZonesLib/JsonHelpers.h>

namespace FancyZonesDataTypes
{
    struct ZoneSetData;
    struct DeviceIdData;
    struct DeviceInfoData;
    struct CustomLayoutData;
    struct AppZoneHistoryData;
}

#if defined(UNIT_TESTS)
namespace FancyZonesUnitTests
{
    class FancyZonesDataUnitTests;
    class FancyZonesIFancyZonesCallbackUnitTests;
    class ZoneSetCalculateZonesUnitTests;
    class WorkAreaUnitTests;
    class WorkAreaCreationUnitTests;
    class LayoutHotkeysUnitTests;
    class LayoutTemplatesUnitTests;
    class CustomLayoutsUnitTests;
}
#endif

class FancyZonesData
{
public:
    FancyZonesData();

    void ReplaceZoneSettingsFileFromOlderVersions();

    void SetVirtualDesktopCheckCallback(std::function<bool(GUID)> callback);

    std::optional<FancyZonesDataTypes::DeviceInfoData> FindDeviceInfo(const FancyZonesDataTypes::DeviceIdData& id) const;

    const JSONHelpers::TDeviceInfoMap& GetDeviceInfoMap() const;

    inline const std::wstring& GetZonesSettingsFileName() const 
    {
        return zonesSettingsFileName;
    }

    inline const std::wstring& GetSettingsFileName() const
    {
        return settingsFileName;
    }

    bool AddDevice(const FancyZonesDataTypes::DeviceIdData& deviceId);
    void CloneDeviceInfo(const FancyZonesDataTypes::DeviceIdData& source, const FancyZonesDataTypes::DeviceIdData& destination);
    void SyncVirtualDesktops(GUID desktopId);
    void RemoveDeletedDesktops(const std::vector<GUID>& activeDesktops);

    void SetActiveZoneSet(const FancyZonesDataTypes::DeviceIdData& deviceId, const FancyZonesDataTypes::ZoneSetData& zoneSet);

    json::JsonObject GetPersistFancyZonesJSON();

    void LoadFancyZonesData();
    void SaveZoneSettings() const;

    void SaveFancyZonesEditorParameters(bool spanZonesAcrossMonitors, const std::wstring& virtualDesktopId, const HMONITOR& targetMonitor, const std::vector<std::pair<HMONITOR, MONITORINFOEX>>& allMonitors) const;

private:
#if defined(UNIT_TESTS)
    friend class FancyZonesUnitTests::FancyZonesDataUnitTests;
    friend class FancyZonesUnitTests::FancyZonesIFancyZonesCallbackUnitTests;
    friend class FancyZonesUnitTests::WorkAreaUnitTests;
    friend class FancyZonesUnitTests::WorkAreaCreationUnitTests;
    friend class FancyZonesUnitTests::ZoneSetCalculateZonesUnitTests;
    friend class FancyZonesUnitTests::LayoutHotkeysUnitTests;
    friend class FancyZonesUnitTests::LayoutTemplatesUnitTests;
    friend class FancyZonesUnitTests::CustomLayoutsUnitTests;

    inline void SetDeviceInfo(const FancyZonesDataTypes::DeviceIdData& deviceId, FancyZonesDataTypes::DeviceInfoData data)
    {
        deviceInfoMap[deviceId] = data;
    }

    inline bool ParseDeviceInfos(const json::JsonObject& fancyZonesDataJSON)
    {
        deviceInfoMap = JSONHelpers::ParseDeviceInfos(fancyZonesDataJSON);
        return !deviceInfoMap.empty();
    }

    inline void clear_data()
    {
        appZoneHistoryMap.clear();
        deviceInfoMap.clear();
    }

    inline void SetSettingsModulePath(std::wstring_view moduleName)
    {
        std::wstring result = PTSettingsHelper::get_module_save_folder_location(moduleName);
        zonesSettingsFileName = result + L"\\" + std::wstring(L"zones-settings.json");
        appZoneHistoryFileName = result + L"\\" + std::wstring(L"app-zone-history.json");
    }

    inline std::wstring GetZoneSettingsPath(std::wstring_view moduleName)
    {
        std::wstring result = PTSettingsHelper::get_module_save_folder_location(moduleName);
        return result + L"\\" + std::wstring(L"zones-settings.json");
    }
#endif
    // Maps app path to app's zone history data
    std::unordered_map<std::wstring, std::vector<FancyZonesDataTypes::AppZoneHistoryData>> appZoneHistoryMap{};
    // Maps device unique ID to device data
    JSONHelpers::TDeviceInfoMap deviceInfoMap{};

    std::wstring settingsFileName;
    std::wstring zonesSettingsFileName;
    std::wstring appZoneHistoryFileName;
    std::wstring editorParametersFileName;

    std::function<bool(GUID)> m_virtualDesktopCheckCallback;

    mutable std::recursive_mutex dataLock;
};

FancyZonesData& FancyZonesDataInstance();

namespace DefaultValues
{
    const int ZoneCount = 3;
    const bool ShowSpacing = true;
    const int Spacing = 16;
    const int SensitivityRadius = 20;
}
