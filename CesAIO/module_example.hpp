#pragma once

#include "module_base.hpp"

namespace Example
{
	void on_load(TreeTab* main_tab);
	void on_unload();

	void registry_tabs(TreeTab* main_tab);

	namespace API
	{}
}