/**
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

// https://github.com/joric/qmk/blob/master/docs/reference_keymap_extras.md
#include "keymap_french.h"

enum charybdis_keymap_layers {
	                 LAYER_BASE = 0,
			 LAYER_LF,
	LAYER_CP_BASE_1, LAYER_RF,
	LAYER_CP_LF_1,   LAYER_LF_RF,
	LAYER_CP_BASE_2, LAYER_MOUSE,
};

// https://github.com/qmk/qmk_firmware/blob/master/docs/feature_layers.md
// https://github.com/qmk/qmk_firmware/blob/master/docs/ref_functions.md#update_tri_layer_statestate-x-y-z
layer_state_t layer_state_set_user(layer_state_t state) {
	// if both LAYER_LF and LAYER_RF are "pressed"
	// move to LAYER_LF_RF
	state = update_tri_layer_state(state, LAYER_LF, LAYER_RF, LAYER_LF_RF);
	return state;
}

enum custom_keycodes {
	PERSO_RGB_TOG = SAFE_RANGE,
	PERSO_MOUSE_SCROLL,
	PERSO_MOUSE_SNIP,
	// utf8 ↓
	PERSO_THREEDOTS,
	PERSO_OE,
	PERSO_AR_N,
	PERSO_AR_NE,
	PERSO_AR_E,
	PERSO_AR_SE,
	PERSO_AR_S,
	PERSO_AR_SO,
	PERSO_AR_O,
	PERSO_AR_NO,
	PERSO_AR_OE,
};

static bool     perso_mouse_scrolling        = false;
static uint16_t perso_mouse_last_scroll      = 0;
static int32_t  perso_mouse_scroll_waiting_h = 0;
static int32_t  perso_mouse_scroll_waiting_v = 0;
static uint8_t  perso_mouse_buttons          = 0;

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
	// disable the trackball when not in LAYER_MOUSE
	if (!layer_state_is(LAYER_MOUSE)) {
		mouse_report.x = 0;
		mouse_report.y = 0;
		return mouse_report;
	}

	// from here LAYER_MOUSE is active

	// when one KC_MS_BTN* is pressed/release
	// I back up mouse_report.buttons (in post_process_record_user)
	// and re-set it here (else the buttons gets reset and it is impossible to
	// hold a mouse key)
	mouse_report.buttons = perso_mouse_buttons;

	// when PERSO_MOUSE_SCROLL is held
	if (perso_mouse_scrolling) {
		perso_mouse_scroll_waiting_h += mouse_report.x;
		perso_mouse_scroll_waiting_v += mouse_report.y;
		int32_t max_scroll_waiting = abs(perso_mouse_scroll_waiting_h) > abs(perso_mouse_scroll_waiting_v) ? abs(perso_mouse_scroll_waiting_h) : abs(perso_mouse_scroll_waiting_v);

		if (timer_elapsed(perso_mouse_last_scroll) < 20 /* ms */ || max_scroll_waiting < 100) {
			// → don't move
			mouse_report.x = 0;
			mouse_report.y = 0;
			return mouse_report;
		}

		// from here perso_mouse_scroll_waiting_h != 0
		// or        perso_mouse_scroll_waiting_h != 0

		// make 1 scroll movement
		if (abs(perso_mouse_scroll_waiting_h) > abs(perso_mouse_scroll_waiting_v)) {
			mouse_report.h = perso_mouse_scroll_waiting_h > 0 ? -1 : 1;
		} else {
			mouse_report.v = perso_mouse_scroll_waiting_v > 0 ? -1 : 1;
		}

		// reset counters
		perso_mouse_last_scroll = timer_read();
		perso_mouse_scroll_waiting_h = 0;
		perso_mouse_scroll_waiting_v = 0;

		// don't move the pointer (as we did a "scroll")
		mouse_report.x = 0;
		mouse_report.y = 0;
	}

	return mouse_report;
}


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	switch (keycode) {
		case PERSO_MOUSE_SCROLL:
			perso_mouse_scrolling = record->event.pressed;
			break;
		case PERSO_MOUSE_SNIP:
			// charybdis sniping mode doesn't work well so I implemented my own
			uint16_t cpi_target;
			if (record->event.pressed) {
				cpi_target = 200;
			} else {
				cpi_target = CHARYBDIS_MINIMUM_DEFAULT_DPI;
			}

			while (pointing_device_get_cpi() != cpi_target) {
				pointing_device_set_cpi(cpi_target);
			}
			break;
		case PERSO_RGB_TOG:
			// RGB_TOG without storing the state to EEPROM
			if (record->event.pressed) {
				// https://github.com/qmk/qmk_firmware/blob/master/docs/feature_rgb_matrix.md#disableenable-effects-iddisable-enable-effects
				rgb_matrix_toggle_noeeprom();
			}
			break;
		case PERSO_THREEDOTS: // …
			if (record->event.pressed) {
				// https://www.fileformat.info/info/unicode/char/2026/index.htm
				register_code(KC_LCTL);
				register_code(KC_LSFT);
				tap_code(FR_U);
				unregister_code(KC_LSFT);
				unregister_code(KC_LCTL);
				tap_code(KC_2);
				tap_code(KC_0);
				tap_code(KC_2);
				tap_code(KC_6);
				tap_code(KC_SPC);
			}
			break;
		case PERSO_OE: // œ
			if (record->event.pressed) {
				// https://www.fileformat.info/info/unicode/char/0153/index.htm
				register_code(KC_LCTL);
				register_code(KC_LSFT);
				tap_code(FR_U);
				unregister_code(KC_LSFT);
				unregister_code(KC_LCTL);
				tap_code(KC_1);
				tap_code(KC_5);
				tap_code(KC_3);
				tap_code(KC_SPC);
			}
			break;
		case PERSO_AR_N: // ↑
			if (record->event.pressed) {
				// https://www.fileformat.info/info/unicode/char/2191/index.htm
				register_code(KC_LCTL);
				register_code(KC_LSFT);
				tap_code(FR_U);
				unregister_code(KC_LSFT);
				unregister_code(KC_LCTL);
				tap_code(KC_2);
				tap_code(KC_1);
				tap_code(KC_9);
				tap_code(KC_1);
				tap_code(KC_SPC);
			}
			break;
		case PERSO_AR_NE: // ↗
			if (record->event.pressed) {
				// https://www.fileformat.info/info/unicode/char/2197/index.htm
				register_code(KC_LCTL);
				register_code(KC_LSFT);
				tap_code(FR_U);
				unregister_code(KC_LSFT);
				unregister_code(KC_LCTL);
				tap_code(KC_2);
				tap_code(KC_1);
				tap_code(KC_9);
				tap_code(KC_7);
				tap_code(KC_SPC);
			}
			break;
		case PERSO_AR_E: // →
			if (record->event.pressed) {
				// https://www.fileformat.info/info/unicode/char/2192/index.htm
				register_code(KC_LCTL);
				register_code(KC_LSFT);
				tap_code(FR_U);
				unregister_code(KC_LSFT);
				unregister_code(KC_LCTL);
				tap_code(KC_2);
				tap_code(KC_1);
				tap_code(KC_9);
				tap_code(KC_2);
				tap_code(KC_SPC);
			}
			break;
		case PERSO_AR_SE: // ↘
			if (record->event.pressed) {
				// https://www.fileformat.info/info/unicode/char/2198/index.htm
				register_code(KC_LCTL);
				register_code(KC_LSFT);
				tap_code(FR_U);
				unregister_code(KC_LSFT);
				unregister_code(KC_LCTL);
				tap_code(KC_2);
				tap_code(KC_1);
				tap_code(KC_9);
				tap_code(KC_8);
				tap_code(KC_SPC);
			}
			break;
		case PERSO_AR_S: // ↓
			if (record->event.pressed) {
				// https://www.fileformat.info/info/unicode/char/2193/index.htm
				register_code(KC_LCTL);
				register_code(KC_LSFT);
				tap_code(FR_U);
				unregister_code(KC_LSFT);
				unregister_code(KC_LCTL);
				tap_code(KC_2);
				tap_code(KC_1);
				tap_code(KC_9);
				tap_code(KC_3);
				tap_code(KC_SPC);
			}
			break;
		case PERSO_AR_SO: // ↙
			if (record->event.pressed) {
				// https://www.fileformat.info/info/unicode/char/2199/index.htm
				register_code(KC_LCTL);
				register_code(KC_LSFT);
				tap_code(FR_U);
				unregister_code(KC_LSFT);
				unregister_code(KC_LCTL);
				tap_code(KC_2);
				tap_code(KC_1);
				tap_code(KC_9);
				tap_code(KC_9);
				tap_code(KC_SPC);
			}
			break;
		case PERSO_AR_O: // ←
			if (record->event.pressed) {
				// https://www.fileformat.info/info/unicode/char/2190/index.htm
				register_code(KC_LCTL);
				register_code(KC_LSFT);
				tap_code(FR_U);
				unregister_code(KC_LSFT);
				unregister_code(KC_LCTL);
				tap_code(KC_2);
				tap_code(KC_1);
				tap_code(KC_9);
				tap_code(KC_0);
				tap_code(KC_SPC);
			}
			break;
		case PERSO_AR_NO: // ↖
			if (record->event.pressed) {
				// https://www.fileformat.info/info/unicode/char/2196/index.htm
				register_code(KC_LCTL);
				register_code(KC_LSFT);
				tap_code(FR_U);
				unregister_code(KC_LSFT);
				unregister_code(KC_LCTL);
				tap_code(KC_2);
				tap_code(KC_1);
				tap_code(KC_9);
				tap_code(KC_6);
				tap_code(KC_SPC);
			}
			break;
		case PERSO_AR_OE: // ↔
			if (record->event.pressed) {
				// https://www.fileformat.info/info/unicode/char/2194/index.htm
				register_code(KC_LCTL);
				register_code(KC_LSFT);
				tap_code(FR_U);
				unregister_code(KC_LSFT);
				unregister_code(KC_LCTL);
				tap_code(KC_2);
				tap_code(KC_1);
				tap_code(KC_9);
				tap_code(KC_4);
				tap_code(KC_SPC);
			}
			break;
	}
	return true;
}
void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
	if IS_MOUSEKEY_BUTTON (keycode) {
		perso_mouse_buttons = pointing_device_get_report().buttons;
	}
}

#define LAYER_BASE_LAYOUT LAYOUT( \
  /* ╭─────────────────────────────────────────────────────────────╮ ╭───────────────────────────────────────────────────────────╮ */ \
       KC_WWW_FORWARD,  FR_DLR, FR_DQUO, FR_QUOT, FR_COLN, FR_AMPR,    FR_HASH,      FR_PIPE, FR_MINS, FR_SLSH, FR_COMM, KC_F14,      \
  /* ├─────────────────────────────────────────────────────────────┤ ├───────────────────────────────────────────────────────────┤ */ \
          KC_WWW_BACK, FR_EXLM,    FR_B,    FR_P,    FR_O,  KC_ESC,    FR_CIRC,         FR_V,    FR_D,    FR_L,    FR_Z, FR_TILD,     \
  /* ├─────────────────────────────────────────────────────────────┤ ├───────────────────────────────────────────────────────────┤ */ \
              FR_QUES,    FR_A,    FR_U,    FR_I,    FR_E,  KC_TAB,       FR_C,         FR_T,    FR_S,    FR_R,    FR_N, FR_ASTR,     \
  /* ├─────────────────────────────────────────────────────────────┤ ├───────────────────────────────────────────────────────────┤ */ \
              KC_LGUI,    FR_J,    FR_Y,    FR_X,  FR_DOT,    FR_K,       FR_H,         FR_Q,    FR_G,    FR_F,    FR_M,    FR_W,     \
  /* ├─────────────────────────────────────────────────────────────┤ ├───────────────────────────────────────────────────────────┤ */ \
                                    KC_BSPC,       KC_SPC, KC_LCTL,     KC_LALT, MO(LAYER_RF),   \
                                             MO(LAYER_LF), KC_LSFT,      KC_ENT                  \
  /*                                   ╰──────────────────────────╯ ╰───────────────────────╯ */ \
  )

#define LAYER_LF_LAYOUT LAYOUT( \
  /* ╭─────────────────────────────────────────────────────────────╮ ╭───────────────────────────────────────────────────────────╮ */ \
       PERSO_RGB_TOG, _______, _______, _______,  _______, _______,      KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12, \
  /* ├─────────────────────────────────────────────────────────────┤ ├───────────────────────────────────────────────────────────┤ */ \
             _______, _______, FR_LBRC, FR_RBRC,  _______, _______,    _______,    FR_7,    FR_8,    FR_9, _______, _______, \
  /* ├─────────────────────────────────────────────────────────────┤ ├───────────────────────────────────────────────────────────┤ */ \
             _______, FR_LABK, FR_RABK, FR_LPRN, FR_RPRN,  _______,    FR_CCED,    FR_4,    FR_5,    FR_6, FR_PLUS, _______, \
  /* ├─────────────────────────────────────────────────────────────┤ ├───────────────────────────────────────────────────────────┤ */ \
             _______,  KC_APP, KC_LCTL, KC_LSFT, FR_LCBR,  FR_RCBR,       FR_0,    FR_1,    FR_2,    FR_3,  FR_EQL, FR_MINS, \
  /* ├─────────────────────────────────────────────────────────────┤ ├───────────────────────────────────────────────────────────┤ */ \
                                 _______, MO(LAYER_MOUSE), _______,    KC_RALT, _______, \
                                                  XXXXXXX, _______,    _______           \
  /*                           ╰───────────────────────────────────╯ ╰───────────────────────╯ */ \
  )

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYER_BASE_LAYOUT,
  [LAYER_LF] = LAYER_LF_LAYOUT,

  [LAYER_CP_BASE_1] = LAYER_BASE_LAYOUT,
  [LAYER_RF] = LAYOUT(
  // ╭──────────────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       _______,   KC_F1,   KC_F2,   KC_F3,    KC_F4,          KC_F5,   _______, _______, _______,  FR_BSLS,  FR_GRV, _______,
  // ├──────────────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       _______, _______, _______, _______, PERSO_OE,        _______,   FR_PERC, _______,   KC_UP,  _______, _______, KC_MUTE,
  // ├──────────────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       _______, FR_AGRV, FR_UGRV, FR_EGRV,  FR_EACU,  LGUI(KC_PGUP),   KC_PGUP, KC_LEFT, KC_DOWN, KC_RIGHT, FR_PLUS, KC_MPLY,
  // ├──────────────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       _______,  KC_APP, _______, _______,  FR_SCLN,  LGUI(KC_PGDN),   KC_PGDN, FR_UNDS, KC_VOLD,  KC_VOLU,    FR_0, FR_MINS,
  // ╰──────────────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                           KC_DEL, _______, _______,    _______, _______,
                                                   _______, _______,    XXXXXXX
  //                                     ╰──────────────────────────╯ ╰───────────────────╯
  ),

  [LAYER_CP_LF_1] = LAYER_LF_LAYOUT,
  [LAYER_LF_RF] = LAYOUT(
  // ╭──────────────────────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
         KC_F6, FR_EURO, PERSO_AR_NO,  PERSO_AR_N,     PERSO_AR_NE, KC_PSCR,    KC_CAPS, _______, _______, _______, _______, _______,
  // ├──────────────────────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       _______, _______,  PERSO_AR_O, PERSO_AR_OE,      PERSO_AR_E, _______,    _______, _______, _______, _______, _______, _______,
  // ├──────────────────────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       _______,   FR_AT, PERSO_AR_SO,  PERSO_AR_S,     PERSO_AR_SE, _______,    _______, KC_HOME, _______,  KC_END, _______, _______,
  // ├──────────────────────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       _______, _______,     _______,     _______, PERSO_THREEDOTS, _______,    _______, _______, KC_MNXT, KC_MPRV, _______, _______,
  // ╰──────────────────────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                                  _______, _______, _______,    _______, _______,
                                                           XXXXXXX, _______,    XXXXXXX
  //                                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  // https://github.com/qmk/qmk_firmware/blob/master/docs/keycodes.md#mouse-keys-idmouse-keys
  [LAYER_CP_BASE_2] = LAYER_BASE_LAYOUT,
  [LAYER_MOUSE] = LAYOUT(
  // ╭────────────────────────────────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       _______,    _______,          _______,            _______,    _______, _______,    _______, _______, _______, _______, _______, _______,
  // ├────────────────────────────────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       _______,    _______,          _______,            _______,    _______, _______,    _______, _______, _______, _______, _______, _______,
  // ├────────────────────────────────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       _______, KC_MS_BTN2, PERSO_MOUSE_SNIP, PERSO_MOUSE_SCROLL, KC_MS_BTN1, _______,    _______, _______, _______, _______, _______, _______,
  // ├────────────────────────────────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       _______, KC_MS_BTN3,          KC_LCTL,            KC_LSFT,    _______, _______,    _______, _______, _______, _______, _______, _______,
  // ╰────────────────────────────────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                                            _______, XXXXXXX, _______,    _______, _______,
                                                                     XXXXXXX, _______,    XXXXXXX
  //                                                      ╰───────────────────────────╯ ╰──────────────────╯
  ),

};
// clang-format on
