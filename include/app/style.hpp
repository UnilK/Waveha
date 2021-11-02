#pragma once

#include "ui/style.h"

ui::Style defaultStyle(
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
        });

