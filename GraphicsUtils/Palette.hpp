#pragma once

#include <imgui.h>

namespace AppSpace::GraphicsUtils {

struct Palette final {
    struct AsImU32 final {
        static constexpr ImU32 kBlackColor     = IM_COL32_BLACK;
        static constexpr ImU32 kWhiteColor     = IM_COL32_WHITE;
        static constexpr ImU32 kRedColor       = IM_COL32(255, 0, 0, 255);
        static constexpr ImU32 kGreenColor     = IM_COL32(0, 255, 0, 255);
        static constexpr ImU32 kBlueColor      = IM_COL32(0, 0, 255, 255);
        static constexpr ImU32 kGrayColor      = IM_COL32(50, 50, 50, 255);
        static constexpr ImU32 kBrightRedColor = IM_COL32(250, 60, 50, 255);
        static constexpr ImU32 kYellowColor    = IM_COL32(255, 255, 0, 255);
        static constexpr ImU32 kOrangeColor    = IM_COL32(255, 165, 0, 255);
    };

    struct AsImColor final {
        static constexpr ImColor kBlackColor = ImColor(AsImU32::kBlackColor);
        static constexpr ImColor kWhiteColor = ImColor(AsImU32::kWhiteColor);
        static constexpr ImColor kRedColor   = ImColor(AsImU32::kRedColor);
        static constexpr ImColor kGreenColor = ImColor(AsImU32::kGreenColor);
        static constexpr ImColor kBlueColor  = ImColor(AsImU32::kBlueColor);
        static constexpr ImColor kGrayColor  = ImColor(AsImU32::kGrayColor);
        static constexpr ImColor kBrightRedColor =
            ImColor(AsImU32::kBrightRedColor);
        static constexpr ImColor kYellowColor = ImColor(AsImU32::kYellowColor);
        static constexpr ImColor kOrangeColor = ImColor(AsImU32::kOrangeColor);
    };
};

}  // namespace AppSpace::GraphicsUtils
