#pragma once

#include "ui/style.h"

ui::Style defaultStyle(
        
{
{"ubuntu_mono", "res/fonts/UMR.ttf"},
},

{
{"MainFrame", ui::Look({
        {"background", "2e0a24ff"},
        {"", ""}})},

{"ToolBar", ui::Look({
        {"background", "ff0000ff"},
        {"", ""}})},

{"MainView", ui::Look({
        {"background", "00ff00ff"},
        {"", ""}})},

{"SideBar", ui::Look({
        {"background", "0000ffff"},
        {"", ""}})},

{"blueSwitch", ui::Look({
        {"background", "0011aaff"},
        {"pressed_background", "000888ff"},
        {"border", "444444ff"},
        {"font", "ubuntu_mono"},
        {"text_size", "20"},
        {"text_color", "ffeeeeff"},
        {"", ""}})},

{"redSwitch", ui::Look({
        {"background", "aa1100ff"},
        {"pressed_background", "880800ff"},
        {"border", "444444ff"},
        {"font", "ubuntu_mono"},
        {"text_size", "20"},
        {"text_color", "ffeeeeff"},
        {"", ""}})},
});

