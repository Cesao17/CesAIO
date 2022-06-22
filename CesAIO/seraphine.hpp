#pragma once

#include "module_base.hpp"

namespace Seraphine
{
	void on_load(TreeTab* main_tab);
	void on_unload();

	void registry_tabs(TreeTab* main_tab);
}