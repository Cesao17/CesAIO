#include "module_example.hpp"

using namespace Module;

namespace Example
{
	//
	// Local header declaration
	//
	void on_update();
	void on_draw();

	bool q_logic(game_object_script object, uint32_t mode = 0);
	bool w_logic(game_object_script object, uint32_t mode = 0);
	bool e_logic(game_object_script object, uint32_t mode = 0);
	bool r_logic(game_object_script object, uint32_t mode = 0);

	//
	// Local namespace declaration
	//

	namespace hashs
	{
		uint32_t your_buff_hash = buff_hash("YOUR_BUFF");
	}

	namespace spells
	{
		script_spell* local_spell_q = nullptr;
		script_spell* local_spell_w = nullptr;
		script_spell* local_spell_e = nullptr;
		script_spell* local_spell_r = nullptr;

		// Pre-Processor: Q properties
		#define _Q_RANGE              0.f
		#define _Q_DAMAGE_TYPE        damage_type::physical

		// Pre-Processor: W properties
		#define _W_RANGE              0.f
		#define _W_DAMAGE_TYPE        damage_type::physical

		// Pre-Processor: E properties
		#define _E_RANGE              0.f
		#define _E_DAMAGE_TYPE        damage_type::physical

		// Pre-Processor: R properties
		#define _R_RANGE              0.f
		#define _R_DAMAGE_TYPE        damage_type::physical
	}

	namespace settings
	{
		namespace combo
		{
			TreeEntry* use_q = nullptr;
			TreeEntry* use_w = nullptr;
			TreeEntry* use_e = nullptr;
			TreeEntry* use_r = nullptr;
		}

		namespace flee
		{
			TreeEntry* use_q = nullptr;
			TreeEntry* use_w = nullptr;
			TreeEntry* use_e = nullptr;
		}

		namespace farm
		{
			TreeEntry* use_q = nullptr;
			TreeEntry* use_w = nullptr;
			TreeEntry* use_e = nullptr;
		}

		namespace render
		{
			TreeEntry* draw_only_ready = nullptr;
			TreeEntry* tickness = nullptr;

			TreeEntry* q_range_draw = nullptr;
			TreeEntry* q_color_draw = nullptr;

			TreeEntry* w_range_draw = nullptr;
			TreeEntry* w_color_draw = nullptr;

			TreeEntry* e_range_draw = nullptr;
			TreeEntry* e_color_draw = nullptr;

			TreeEntry* r_range_draw = nullptr;
			TreeEntry* r_color_draw = nullptr;
		}
	}

	//
	// Module "main" functions 
	//

	#pragma region _MODULE_INPUT
	void on_load(TreeTab* main_tab)
	{
		// Declaring local module infos.
		module_name = "MODULE_NAME"; // Module::module_name
		module_version = "MODULE_VERSION";      // Module::module_version

		// SDK Script Spells declaration.
		spells::local_spell_q = plugin_sdk->register_spell(spellslot::q, _Q_RANGE);
		spells::local_spell_w = plugin_sdk->register_spell(spellslot::w, _W_RANGE);
		spells::local_spell_e = plugin_sdk->register_spell(spellslot::e, _E_RANGE);
		spells::local_spell_r = plugin_sdk->register_spell(spellslot::r, _R_RANGE);

		spells::local_spell_q->set_damage_type(_Q_DAMAGE_TYPE);
		spells::local_spell_w->set_damage_type(_W_DAMAGE_TYPE);
		spells::local_spell_e->set_damage_type(_E_DAMAGE_TYPE);
		spells::local_spell_r->set_damage_type(_R_DAMAGE_TYPE);

		// Registering menu tabs.
		registry_tabs(main_tab);

		// Registering SDK event-handlers.
		event_handler<events::on_update>::add_callback(on_update);
		event_handler<events::on_draw>::add_callback(on_draw);
	}

	void on_unload()
	{
		// SDK Script Spells declaration.
		plugin_sdk->remove_spell(spells::local_spell_q);
		plugin_sdk->remove_spell(spells::local_spell_w);
		plugin_sdk->remove_spell(spells::local_spell_e);
		plugin_sdk->remove_spell(spells::local_spell_r);

		// Removing SDK event-handlers.
		event_handler<events::on_update>::remove_handler(on_update);
		event_handler<events::on_draw>::remove_handler(on_draw);
	}

	void registry_tabs(TreeTab* main_tab)
	{
		main_tab->add_separator("info.header", module_name.c_str());

		auto combo_tab = main_tab->add_tab("tab.combo", "Combo");
		{
			combo_tab->add_separator("separator.q", "- - Q Settings - -");
			settings::combo::use_q = combo_tab->add_checkbox("use_q", "Use Q in Combo:", true);
			settings::combo::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());

			combo_tab->add_separator("separator.w", "- - W Settings - -");
			settings::combo::use_w = combo_tab->add_checkbox("use_w", "Use W in Combo:", true);
			settings::combo::use_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());

			combo_tab->add_separator("separator.e", "- - E Settings - -");
			settings::combo::use_e = combo_tab->add_checkbox("use_e", "Use E in Combo:", true);
			settings::combo::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());

			combo_tab->add_separator("separator.r", "- - R Settings - -");
			settings::combo::use_r = combo_tab->add_checkbox("use_r", "Use R in Combo:", true);
			settings::combo::use_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
		}

		auto render_tab = main_tab->add_tab("tab.farm", "Render");
		{
			float default_color[4] = { 255, 170, 12, 176 };

			settings::render::draw_only_ready = render_tab->add_checkbox("draw_only_ready", "Only Draw if Spell is Ready?", false);
			settings::render::tickness = render_tab->add_slider("tickness", "Draw Tickness:", 2, 1, 5);

			render_tab->add_separator("separator.q", "- - Q Settings - -");
			settings::render::q_range_draw = render_tab->add_checkbox("q_draw", "Draw Q Range:", true);
			settings::render::q_range_draw->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
			settings::render::q_color_draw = render_tab->add_colorpick("q_colorpciker", "Draw Color:", default_color);

			render_tab->add_separator("separator.w", "- - W Settings - -");
			settings::render::w_range_draw = render_tab->add_checkbox("w_draw", "Draw W Range:", true);
			settings::render::w_range_draw->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
			settings::render::w_color_draw = render_tab->add_colorpick("w_colorpciker", "Draw Color:", default_color);

			render_tab->add_separator("separator.e", "- - E Settings - -");
			settings::render::e_range_draw = render_tab->add_checkbox("e_draw", "Draw E Range:", true);
			settings::render::e_range_draw->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
			settings::render::e_color_draw = render_tab->add_colorpick("e_colorpciker", "Draw Color:", default_color);

			render_tab->add_separator("separator.r", "- - R Settings - -");
			settings::render::r_range_draw = render_tab->add_checkbox("r_draw", "Draw R Range:", true);
			settings::render::r_range_draw->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
			settings::render::r_color_draw = render_tab->add_colorpick("r_colorpciker", "Draw Color:", default_color);
		}

		auto flee_tab = main_tab->add_tab("tab.flee", "Flee");
		{
			flee_tab->add_separator("separator.w", "- - Q Settings - -");
			settings::flee::use_q = flee_tab->add_checkbox("use_w", "Use W to Flee?", true);
			settings::flee::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());

			flee_tab->add_separator("separator.w", "- - W Settings - -");
			settings::flee::use_w = flee_tab->add_checkbox("use_w", "Use W to Flee?", true);
			settings::flee::use_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());

			flee_tab->add_separator("separator.w", "- - E Settings - -");
			settings::flee::use_q = flee_tab->add_checkbox("use_w", "Use W to Flee?", true);
			settings::flee::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
		}

		main_tab->add_separator("info.footer", string("Version: ").append(module_version).c_str());
	}
	#pragma endregion

	//
	// SDK Event-handlers
	//

	#pragma region _SDK_EVENT_HANDLERS
	
	// The on_update event allows you to create automatic and semi-automatic logic.
	void on_update()
	{
		if (myhero->is_dead())
			return;

		/*
			If your logic doesn't need a target, just check that the skill is ready to use and that the user allows its use:
				> settings::combo::use_q->get_bool() && spells::local_spell_q->is_ready()

			-=-

			If your logic needs a valid target:

				* In case your logic needs to check if there are any enemies in a RANGE, you can use the model:
					> spells::YOUR_SPELL->is_ready() && myhero->count_enemies_in_range(RANGE)

				* In case your logic needs to check if there are any ally in a RANGE, you can use the model:
					> spells::YOUR_SPELL->is_ready() && myhero->count_allies_in_range(RANGE)

				(NOTE: myhero->count_X_in_range(RANGE) ~ Will return if there are any enemies/allies within the RANGE area)

			-=-

			Usual orbwalker states (All these methods'll return bools values):
				> orbwalker->combo_mode();
				> orbwalker->flee_mode();
				> orbwalker->lane_clear_mode();
				> orbwalker->last_hit_mode();
				> orbwalker->mixed_mode(); 
		*/

		// Your code-logic here.
	}

	void on_draw()
	{
		if (myhero->is_dead())
			return;

		const auto mypos = myhero->get_position();

		if (settings::render::q_range_draw->get_bool() && (!settings::render::draw_only_ready->get_bool() || spells::local_spell_q->is_ready()))
		{
			draw_manager->add_circle(mypos, _Q_RANGE, settings::render::q_color_draw->get_color(), settings::render::tickness->get_int());
		}

		if (settings::render::w_range_draw->get_bool() && (!settings::render::draw_only_ready->get_bool() || spells::local_spell_w->is_ready()))
		{
			draw_manager->add_circle(mypos, _W_RANGE, settings::render::w_color_draw->get_color(), settings::render::tickness->get_int());
		}

		if (settings::render::e_range_draw->get_bool() && (!settings::render::draw_only_ready->get_bool() || spells::local_spell_e->is_ready()))
		{
			draw_manager->add_circle(mypos, _E_RANGE, settings::render::e_color_draw->get_color(), settings::render::tickness->get_int());
		}

		if (settings::render::r_range_draw->get_bool() && (!settings::render::draw_only_ready->get_bool() || spells::local_spell_r->is_ready()))
		{
			draw_manager->add_circle(mypos, _R_RANGE, settings::render::r_color_draw->get_color(), settings::render::tickness->get_int());
		}
	}
	#pragma endregion

	//
	// Module logical functions
	//

	#pragma region _Q_LOGIC
	bool q_logic(game_object_script object, uint32_t mode)
	{
		if (!spells::local_spell_q->is_ready())
			return false;

		// ... Code logic here ...

		return false;
	}
	#pragma endregion

	#pragma region _W_LOGIC
	bool w_logic(game_object_script object, uint32_t mode)
	{
		if (!spells::local_spell_w->is_ready())
			return false;

		// ... Code logic here ...

		return false;
	}
	#pragma endregion

	#pragma region _E_LOGIC
	bool e_logic(game_object_script object, uint32_t mode)
	{
		if (!spells::local_spell_e->is_ready())
			return false;

		// ... Code logic here ...

		return false;
	}
	#pragma endregion

	#pragma region _R_LOGIC
	bool r_logic(game_object_script object, uint32_t mode)
	{
		if (!spells::local_spell_r->is_ready())
			return false;

		// ... Code logic here ...

		return false;
	}
	#pragma endregion
}