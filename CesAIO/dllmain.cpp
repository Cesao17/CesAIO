// Headers declaration

#include "../plugin_sdk/plugin_sdk.hpp"

// Internal Modules Imports

#include "seraphine.hpp"

// Local Variables
TreeTab* plugin_tab = nullptr;

// SDK Declarations

PLUGIN_NAME("CesAIO");
PLUGIN_TYPE(plugin_type::champion);

SUPPORTED_CHAMPIONS(
    champion_id::Seraphine
);

// SDK Handlers

PLUGIN_API bool on_sdk_load(plugin_sdk_core* plugin_sdk)
{
    DECLARE_GLOBALS(plugin_sdk);

	plugin_tab = menu->create_tab("cesaio.main_tab", "CesAIO");
	plugin_tab->set_texture(myhero->get_square_icon_portrait());

	switch (myhero->get_champion())
	{
		case champion_id::Seraphine:
		{
			Seraphine::on_load(plugin_tab);
			break;
		}

		default:
			console->print("[CesAIO] ~ An unsupported champion (%s) was loaded with the plugin.", myhero->get_model_cstr());
			break;
	}

    return true;
}

PLUGIN_API void on_sdk_unload()
{
	switch (myhero->get_champion())
	{
		case champion_id::Seraphine:
		{
			Seraphine::on_unload();
			break;
		}

		default:
			break;
	}

	menu->delete_tab(plugin_tab);
}