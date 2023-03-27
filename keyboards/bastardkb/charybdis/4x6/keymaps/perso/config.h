#pragma once

// https://github.com/qmk/qmk_firmware/blob/master/docs/feature_rgb_matrix.md#additional-configh-options-idadditional-configh-options
#undef  RGB_MATRIX_DEFAULT_SPD
#define RGB_MATRIX_DEFAULT_SPD 4

// setting the left side as master trigger problems with trackball DPI (cpi) setting
// https://discord.com/channels/681309835135811648/1089876985213354026/1089977683477266523

// used only at keyboard initialization
#define CHARYBDIS_MINIMUM_DEFAULT_DPI 800
#define CHARYBDIS_DEFAULT_DPI_CONFIG_STEP 0 // so the DPI is exactly CHARYBDIS_MINIMUM_DEFAULT_DPI
