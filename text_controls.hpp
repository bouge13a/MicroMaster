/*
 * text_controls.h
 *
 *  Created on: Aug 3, 2020
 *      Author: steph
 */

#ifndef TEXT_CONTROLS_HPP_
#define TEXT_CONTROLS_HPP_

#include <stdint.h>

namespace TextCtl {

    enum text_modes {
        mode_reset = 0,
        mode_bold_on,
        mode_faint,
        mode_italic,
        mode_underline,
        mode_slow_blink,
        mode_rapid_blink,
        mode_reverse,
        mode_concealed,
        mode_underline_off = 24,
        mode_reverse_off = 27,

    };

    enum text_colors {
        black_text = 30,
        red_text,
        green_text,
        yellow_text,
        blue_text,
        magenta_text,
        cyan_text,
        white_text,
        bright_red_text=91,
    };

    enum background_colors {
        black_bgd = 40,
        red_bgd,
        green_bgd,
        yellow_bgd,
        blue_bgd,
        magenta_bgd,
        cyan_bgd,
        white_bgd,
    };


    void set_text_mode(uint32_t mode);
    void text_color(uint32_t color);
    void bold_text_color(uint32_t color);
    void bgd_color(uint32_t color);
    void cursor_pos(uint32_t row, uint32_t column);
    void save_cursor_pos(void);
    void restore_cursor_pos(void);
    void clear_terminal(void);
    void clear_line(void);
    void clear_in_line(void);
    void clear_below_line(void);

}

#endif /* TEXT_CONTROLS_HPP_ */
