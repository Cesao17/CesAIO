#pragma once

#include "../plugin_sdk/plugin_sdk.hpp"

#define COLOR_RED(alpha)   = D3DCOLOR_ARGB(alpha, 255,   0,   0)
#define COLOR_GREEN(alpha) = D3DCOLOR_ARGB(alpha,   0, 255,   0)
#define COLOR_BLUE(alpha)  = D3DCOLOR_ARGB(alpha,   0,   0, 255)

using std::string;
using std::to_string;

namespace Module
{
	string module_name;
	string module_version;
}