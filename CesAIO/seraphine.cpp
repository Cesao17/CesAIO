#include "seraphine.hpp"

using namespace Module;

namespace Seraphine
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
		//uint32_t buff_passive = buff_hash("...");
	}

	namespace spells
	{
		script_spell* local_spell_q = nullptr;
		script_spell* local_spell_w = nullptr;
		script_spell* local_spell_e = nullptr;
		script_spell* local_spell_r = nullptr;

		// Pre-Processor: Q properties
		#define _Q_RANGE              0.f
		#define _Q_CAST_DELAY         0.f
		#define _Q_MISSILE_RADIUS     0.f
		#define _Q_MISSILE_SPEED      0.f
		#define _Q_DAMAGE_TYPE        damage_type::magical

		// Pre-Processor: W properties
		#define _W_RANGE              0.f
		#define _W_CAST_DELAY         0.f
		#define _W_MISSILE_RADIUS     0.f
		#define _W_MISSILE_SPEED      0.f
		#define _W_DAMAGE_TYPE        damage_type::magical

		// Pre-Processor: E properties
		#define _E_RANGE              0.f
		#define _E_CAST_DELAY         0.f
		#define _E_CAST_RADIUS        0.f

		// Pre-Processor: R properties
		#define _R_RANGE              0.f
		#define _R_CAST_DELAY         0.f
		#define _R_MISSILE_RADIUS     0.f
		#define _R_MISSILE_SPEED      0.f
		#define _R_DAMAGE_TYPE        damage_type::magical
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
			TreeEntry* r_pred_draw = nullptr;
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
		module_name = "Seraphine-OTP"; // Module::module_name
		module_version = "0.0.1";      // Module::module_version

		// SDK Script Spells declaration.
		spells::local_spell_q = plugin_sdk->register_spell(spellslot::q, _Q_RANGE);
		spells::local_spell_w = plugin_sdk->register_spell(spellslot::w, _W_RANGE);
		spells::local_spell_e = plugin_sdk->register_spell(spellslot::e, _E_RANGE);
		spells::local_spell_r = plugin_sdk->register_spell(spellslot::r, _R_RANGE);

		spells::local_spell_q->set_damage_type(_Q_DAMAGE_TYPE);
		spells::local_spell_w->set_damage_type(_W_DAMAGE_TYPE);
		spells::local_spell_r->set_damage_type(_R_DAMAGE_TYPE);

		spells::local_spell_q->set_skillshot(_Q_CAST_DELAY, _Q_MISSILE_RADIUS, _Q_MISSILE_SPEED, { collisionable_objects::yasuo_wall }, skillshot_type::skillshot_circle);
		spells::local_spell_w->set_skillshot(_W_CAST_DELAY, _W_MISSILE_RADIUS, _W_MISSILE_SPEED, { collisionable_objects::yasuo_wall }, skillshot_type::skillshot_line);
		spells::local_spell_r->set_skillshot(_R_CAST_DELAY, _R_MISSILE_RADIUS, _R_MISSILE_SPEED, { collisionable_objects::yasuo_wall }, skillshot_type::skillshot_line);

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
			settings::combo::use_r->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());

			combo_tab->add_separator("separator.w", "- - W Settings - -");
			settings::combo::use_w = combo_tab->add_checkbox("use_w", "Use W in Combo:", true);
			settings::combo::use_r->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());

			combo_tab->add_separator("separator.e", "- - E Settings - -");
			settings::combo::use_e = combo_tab->add_checkbox("use_e", "Use E in Combo:", true);
			settings::combo::use_r->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());

			combo_tab->add_separator("separator.r", "- - R Settings - -");
			settings::combo::use_r = combo_tab->add_checkbox("use_r", "Use R in Combo:", true);
			settings::combo::use_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
		}

		auto render_tab = main_tab->add_tab("tab.render", "Render");
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
			settings::render::r_pred_draw = render_tab->add_checkbox("r_pred_draw", "Draw R Prediction:", true);
			settings::render::r_color_draw = render_tab->add_colorpick("r_colorpciker", "Draw Color:", default_color);

			const auto menu_r_updater = [](TreeEntry* entry)
			{
				settings::render::r_pred_draw->is_hidden() = settings::render::r_range_draw->get_bool();
			};

			settings::render::r_range_draw->add_property_change_callback(menu_r_updater);
			menu_r_updater(nullptr);
		}

		auto flee_tab = main_tab->add_tab("tab.flee", "Flee");
		{
			flee_tab->add_separator("separator.w", "- - W Settings - -");
			settings::flee::use_w = flee_tab->add_checkbox("use_w", "Use W to Flee?", true);
			settings::flee::use_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());

			flee_tab->add_separator("separator.e", "- - E Settings - -");
			settings::flee::use_e = flee_tab->add_checkbox("use_e", "Use E to Flee?", true);
			settings::flee::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
		}

		main_tab->add_separator("info.footer", string("Version: ").append(module_version).c_str());
	}
	#pragma endregion

	//
	// SDK Event-handlers
	//

	#pragma region _SDK_EVENT_HANDLERS
	void on_update()
	{
		if (myhero->is_dead())
			return;

		if (orbwalker->combo_mode())
		{
			if (settings::combo::use_q->get_bool() && spells::local_spell_q->is_ready() && myhero->count_enemies_in_range(_Q_RANGE))
			{
				q_logic(nullptr);
			}

			if (settings::combo::use_w->get_bool() && spells::local_spell_w->is_ready() && myhero->count_enemies_in_range(_W_RANGE))
			{
				w_logic(nullptr);
			}

			if (settings::combo::use_r->get_bool() && spells::local_spell_r->is_ready() && myhero->count_enemies_in_range(_R_RANGE))
			{
				r_logic(nullptr);
			}

			return;
		}

		if (orbwalker->flee_mode())
		{
			if (settings::flee::use_w->get_bool() && spells::local_spell_w->is_ready() && myhero->count_enemies_in_range(_W_RANGE))
			{
				w_logic(nullptr);
			}

			return;
		}
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

		if (settings::render::r_range_draw->get_bool() && (!settings::render::draw_only_ready->get_bool() || spells::local_spell_r->is_ready()))
		{
			draw_manager->add_circle(mypos, _R_RANGE, settings::render::r_color_draw->get_color(), settings::render::tickness->get_int());

			if (settings::render::r_pred_draw->get_bool())
			{
				// ...
			}
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

		auto _selected_target = target_selector->get_selected_target();
		auto _target = (_selected_target && _selected_target->is_valid_target(_Q_RANGE) ? _selected_target : object);

		if (!_target || !_target->is_valid_target(_Q_RANGE) || _target->is_dead() || _target->is_invulnerable())
		{
			_target = target_selector->get_target(spells::local_spell_q, spells::local_spell_q->_damage_type);
		}

		if (_target && _target->is_valid_target(_Q_RANGE) && !_target->is_dead() && !_target->is_invulnerable())
		{
			const auto _prediction = spells::local_spell_q->get_prediction(_target);

			if (_prediction.hitchance >= hit_chance::medium)
			{
				const auto _cast_position = _prediction._cast_position;

				if (myhero->get_distance(_cast_position) > _Q_RANGE)
				{
					return spells::local_spell_q->fast_cast(myhero->get_position().extend(_target->get_position(), _Q_RANGE));
				}
				else
				{
					return spells::local_spell_q->fast_cast(_cast_position);
				}
			}
		}

		return false;
	}
	#pragma endregion

	#pragma region _W_LOGIC
	bool w_logic(game_object_script object, uint32_t mode)
	{
		if (!spells::local_spell_w->is_ready())
			return false;

		return spells::local_spell_w->cast();
	}
	#pragma endregion

	#pragma region _E_LOGIC
	bool e_logic(game_object_script object, uint32_t mode)
	{
		if (!spells::local_spell_e->is_ready())
			return false;

		auto _selected_target = target_selector->get_selected_target();
		auto _target = (_selected_target && _selected_target->is_valid_target(_E_RANGE) ? _selected_target : object);

		if (!_target || !_target->is_valid_target(_E_RANGE) || _target->is_dead() || _target->is_invulnerable())
		{
			_target = target_selector->get_target(spells::local_spell_e, spells::local_spell_e->_damage_type);
		}

		if (_target && _target->is_valid_target(_E_RANGE) && !_target->is_dead() && !_target->is_invulnerable())
		{
			const auto _prediction = spells::local_spell_e->get_prediction(_target);

			if (_prediction.hitchance >= hit_chance::high)
			{
				const auto _cast_position = _prediction._cast_position;

				if (myhero->get_distance(_cast_position) > _E_RANGE)
				{
					return spells::local_spell_e->fast_cast(myhero->get_position().extend(_target->get_position(), _E_RANGE));
				}
				else
				{
					return spells::local_spell_e->fast_cast(_cast_position);
				}
			}
		}

		return false;
	}
	#pragma endregion

	#pragma region _R_LOGIC
	bool r_logic(game_object_script object, uint32_t mode)
	{
		if (!spells::local_spell_r->is_ready())
			return false;

		auto _selected_target = target_selector->get_selected_target();
		auto _target = (_selected_target && _selected_target->is_valid_target(_R_RANGE) ? _selected_target : object);

		if (!_target || !_target->is_valid_target(_R_RANGE) || _target->is_dead() || _target->is_invulnerable())
		{
			_target = target_selector->get_target(spells::local_spell_r, spells::local_spell_r->_damage_type);
		}

		if (_target && _target->is_valid_target(_R_RANGE) && !_target->is_dead() && !_target->is_invulnerable())
		{
			const auto _prediction = spells::local_spell_r->get_prediction(_target);

			if (_prediction.hitchance >= hit_chance::high)
			{
				const auto _cast_position = _prediction._cast_position;

				if (myhero->get_distance(_cast_position) > _R_RANGE)
				{
					return spells::local_spell_r->fast_cast(myhero->get_position().extend(_target->get_position(), _R_RANGE));
				}
				else
				{
					return spells::local_spell_r->fast_cast(_cast_position);
				}
			}
		}

		return false;
	}
	#pragma endregion
}