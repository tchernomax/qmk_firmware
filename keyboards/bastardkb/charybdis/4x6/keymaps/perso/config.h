#pragma once

// https://github.com/qmk/qmk_firmware/blob/master/docs/feature_rgb_matrix.md#additional-configh-options-idadditional-configh-options
#undef  RGB_MATRIX_DEFAULT_SPD
#define RGB_MATRIX_DEFAULT_SPD 4

// https://github.com/joric/qmk/blob/master/docs/feature_split_keyboard.md#handedness-by-define
#undef MASTER_RIGHT
#define MASTER_LEFT

// used only at keyboard initialization
#define CHARYBDIS_MINIMUM_DEFAULT_DPI 800
#define CHARYBDIS_DEFAULT_DPI_CONFIG_STEP 0 // so the DPI is exactly CHARYBDIS_MINIMUM_DEFAULT_DPI
