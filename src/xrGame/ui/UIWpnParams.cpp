#include "stdafx.h"
#include "pch_script.h"
#include "UIWpnParams.h"
#include "../../xrUI/UIXmlInit.h"
#include "../Level.h"
#include "game_base_space.h"
#include "../ai_space.h"
#include "../../xrScripts/script_engine.h"
#include "inventory_item_object.h"
#include "UIInventoryUtilities.h"
#include "Weapon.h"
#include "WeaponBinoculars.h"
#include "WeaponKnife.h"
#include "Silencer.h"
#include "../../xrUI/UIHelper.h"

struct SLuaWpnParams
{
	luabind::functor<float>		m_functorRPM;
	luabind::functor<float>		m_functorAccuracy;
	luabind::functor<float>		m_functorDamage;
	luabind::functor<float>		m_functorDamageMP;
	luabind::functor<float>		m_functorHandling;

	SLuaWpnParams();
	~SLuaWpnParams();
};

SLuaWpnParams::SLuaWpnParams()
{
	bool	functor_exists;
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetRPM",		m_functorRPM);		VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetDamage",	m_functorDamage);	VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetDamageMP", m_functorDamageMP);	VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetHandling", m_functorHandling);	VERIFY(functor_exists);
	functor_exists	= ai().script_engine().functor("ui_wpn_params.GetAccuracy", m_functorAccuracy);	VERIFY(functor_exists);
}

SLuaWpnParams::~SLuaWpnParams()
{
}

SLuaWpnParams* g_lua_wpn_params = nullptr;

void destroy_lua_wpn_params()
{
	if(g_lua_wpn_params)
		xr_delete(g_lua_wpn_params);
}

// =====================================================================

CUIWpnParams::CUIWpnParams()
{
	AttachChild(&m_textAccuracy);
	AttachChild(&m_textDamage);
	AttachChild(&m_textHandling);
	AttachChild(&m_textRPM);

	AttachChild(&m_progressAccuracy);
	AttachChild(&m_progressDamage);
	AttachChild(&m_progressHandling);
	AttachChild(&m_progressRPM);
}

CUIWpnParams::~CUIWpnParams()
{
}

void CUIWpnParams::InitFromXml(CUIXml& xml_doc)
{
	if (!xml_doc.NavigateToNode("wpn_params", 0))	return;
	CUIXmlInit::InitWindow			(xml_doc, "wpn_params", 0, this);
	if (xml_doc.NavigateToNode("wpn_params:prop_line"))
		m_Prop_line = UIHelper::CreateStatic(xml_doc, "wpn_params:prop_line", this);

	if (xml_doc.NavigateToNode("wpn_params:static_accuracy"))
		m_icon_acc = UIHelper::CreateStatic(xml_doc, "wpn_params:static_accuracy", this);
	if (xml_doc.NavigateToNode("wpn_params:static_damage"))
		m_icon_dam = UIHelper::CreateStatic(xml_doc, "wpn_params:static_damage", this);
	if (xml_doc.NavigateToNode("wpn_params:static_handling"))
		m_icon_han = UIHelper::CreateStatic(xml_doc, "wpn_params:static_handling", this);
	if (xml_doc.NavigateToNode("wpn_params:static_rpm"))
		m_icon_rpm = UIHelper::CreateStatic(xml_doc, "wpn_params:static_rpm", this);

	CUIXmlInit::InitTextWnd			(xml_doc, "wpn_params:cap_accuracy",		0, &m_textAccuracy);
	CUIXmlInit::InitTextWnd			(xml_doc, "wpn_params:cap_damage",			0, &m_textDamage);
	CUIXmlInit::InitTextWnd			(xml_doc, "wpn_params:cap_handling",		0, &m_textHandling);
	CUIXmlInit::InitTextWnd			(xml_doc, "wpn_params:cap_rpm",				0, &m_textRPM);

	m_progressAccuracy.InitFromXml	( xml_doc, "wpn_params:progress_accuracy" );
	m_progressDamage.InitFromXml	( xml_doc, "wpn_params:progress_damage" );
	m_progressHandling.InitFromXml	( xml_doc, "wpn_params:progress_handling" );
	m_progressRPM.InitFromXml		( xml_doc, "wpn_params:progress_rpm" );

	if(IsGameTypeSingle())
	{
		if (xml_doc.NavigateToNode("wpn_params:static_ammo"))
			m_stAmmo = UIHelper::CreateStatic(xml_doc, "wpn_params:static_ammo", this);
		if (xml_doc.NavigateToNode("wpn_params:cap_ammo_count"))
			m_textAmmoCount = UIHelper::CreateTextWnd(xml_doc, "wpn_params:cap_ammo_count", this);
		if (xml_doc.NavigateToNode("wpn_params:cap_ammo_count2"))
			m_textAmmoCount2 = UIHelper::CreateTextWnd(xml_doc, "wpn_params:cap_ammo_count2", this);
		if (xml_doc.NavigateToNode("wpn_params:cap_ammo_types"))
			m_textAmmoTypes = UIHelper::CreateTextWnd(xml_doc, "wpn_params:cap_ammo_types", this);
		if (xml_doc.NavigateToNode("wpn_params:cap_ammo_used_type"))
			m_textAmmoUsedType = UIHelper::CreateTextWnd(xml_doc, "wpn_params:cap_ammo_used_type", this);
		if (xml_doc.NavigateToNode("wpn_params:static_ammo_type1"))
			m_stAmmoType1 = UIHelper::CreateStatic(xml_doc, "wpn_params:static_ammo_type1", this);
		if (xml_doc.NavigateToNode("wpn_params:static_ammo_type2"))
			m_stAmmoType2 = UIHelper::CreateStatic(xml_doc, "wpn_params:static_ammo_type2", this);
	}

}

void CUIWpnParams::SetInfo( CInventoryItem* slot_wpn, CInventoryItem& cur_wpn )
{
	if ( !g_lua_wpn_params )
	{
		g_lua_wpn_params = new SLuaWpnParams();
	}
	
	LPCSTR cur_section  = cur_wpn.object().cNameSect().c_str();
	string2048 str_upgrades;
	str_upgrades[0] = 0;
	cur_wpn.get_upgrades_str( str_upgrades );

	float cur_rpm    = iFloor(g_lua_wpn_params->m_functorRPM( cur_section, str_upgrades )*53.0f)/53.0f;
	float cur_accur  = iFloor(g_lua_wpn_params->m_functorAccuracy( cur_section, str_upgrades )*53.0f)/53.0f;
	float cur_hand   = iFloor(g_lua_wpn_params->m_functorHandling( cur_section, str_upgrades )*53.0f)/53.0f;
	float cur_damage = (IsGameTypeSingle()) ?
		iFloor(g_lua_wpn_params->m_functorDamage( cur_section, str_upgrades )*53.0f)/53.0f
		: iFloor(g_lua_wpn_params->m_functorDamageMP( cur_section, str_upgrades )*53.0f)/53.0f;

	float slot_rpm    = cur_rpm;
	float slot_accur  = cur_accur;
	float slot_hand   = cur_hand;
	float slot_damage = cur_damage;

	if ( slot_wpn && (slot_wpn != &cur_wpn) )
	{
		LPCSTR slot_section  = slot_wpn->object().cNameSect().c_str();
		str_upgrades[0] = 0;
		slot_wpn->get_upgrades_str( str_upgrades );

		slot_rpm    = iFloor(g_lua_wpn_params->m_functorRPM( slot_section, str_upgrades )*53.0f)/53.0f;
		slot_accur  = iFloor(g_lua_wpn_params->m_functorAccuracy( slot_section, str_upgrades )*53.0f)/53.0f;
		slot_hand   = iFloor(g_lua_wpn_params->m_functorHandling( slot_section, str_upgrades )*53.0f)/53.0f;
		slot_damage = (IsGameTypeSingle()) ?
			iFloor(g_lua_wpn_params->m_functorDamage( slot_section, str_upgrades )*53.0f)/53.0f
			: iFloor(g_lua_wpn_params->m_functorDamageMP( slot_section, str_upgrades )*53.0f)/53.0f;
	}
	
	m_progressAccuracy.SetTwoPos( cur_accur,  slot_accur );
	m_progressDamage.SetTwoPos(   cur_damage, slot_damage );
	m_progressHandling.SetTwoPos( cur_hand,   slot_hand );
	m_progressRPM.SetTwoPos(      cur_rpm,    slot_rpm );

	if(IsGameTypeSingle())
	{
		xr_vector<shared_str> ammo_types;

		CWeapon* weapon = cur_wpn.cast_weapon();
		if(!weapon)
			return;

		int ammo_count = weapon->GetAmmoMagSize();
		int ammo_count2 = ammo_count;

		if(slot_wpn)
		{
			CWeapon* slot_weapon = slot_wpn->cast_weapon();
			if(slot_weapon)
				ammo_count2 = slot_weapon->GetAmmoMagSize(); 
		}

        if (m_textAmmoCount2)
        {
            if (ammo_count == ammo_count2)
                m_textAmmoCount2->SetTextColor(color_rgba(170, 170, 170, 255));
            else if (ammo_count < ammo_count2)
                m_textAmmoCount2->SetTextColor(color_rgba(255, 0, 0, 255));
            else
                m_textAmmoCount2->SetTextColor(color_rgba(0, 255, 0, 255));

            string128 str;
            xr_sprintf(str, sizeof(str), "%d", ammo_count);
            m_textAmmoCount2->SetText(str);
        }

		ammo_types = weapon->m_ammoTypes;
		if(ammo_types.empty())
			return;

		if (m_textAmmoUsedType)
		{
			string128 str;
			xr_sprintf(str, sizeof(str), "%s", pSettings->r_string(ammo_types[0].c_str(), "inv_name_short"));
			m_textAmmoUsedType->SetTextST(str);
		}

		Frect tex_rect = {};
		if (m_stAmmoType1)
		{
			m_stAmmoType1->SetShader(InventoryUtilities::GetEquipmentIconsShader());
			tex_rect.x1 = float(pSettings->r_u32(ammo_types[0].c_str(), "inv_grid_x") * INV_GRID_WIDTH(isHQIcons));
			tex_rect.y1 = float(pSettings->r_u32(ammo_types[0].c_str(), "inv_grid_y") * INV_GRID_HEIGHT(isHQIcons));
			tex_rect.x2 = float(pSettings->r_u32(ammo_types[0].c_str(), "inv_grid_width") * INV_GRID_WIDTH(isHQIcons));
			tex_rect.y2 = float(pSettings->r_u32(ammo_types[0].c_str(), "inv_grid_height") * INV_GRID_HEIGHT(isHQIcons));
			tex_rect.rb.add(tex_rect.lt);
			m_stAmmoType1->SetTextureRect(tex_rect);
			m_stAmmoType1->TextureOn();
			m_stAmmoType1->SetStretchTexture(true);

			if (isHQIcons)
				m_stAmmoType1->SetWndSize(Fvector2().set((tex_rect.x2 - tex_rect.x1) * UI().get_current_kx() / 2, (tex_rect.y2 - tex_rect.y1) / 2));
			else
				m_stAmmoType1->SetWndSize(Fvector2().set((tex_rect.x2 - tex_rect.x1) * UI().get_current_kx(), tex_rect.y2 - tex_rect.y1));
		}

		if (m_stAmmoType2)
		{
			m_stAmmoType2->SetShader(InventoryUtilities::GetEquipmentIconsShader());
			if (ammo_types.size() == 1)
			{
				tex_rect.set(0, 0, 1, 1);
			}
			else
			{
				tex_rect.x1 = float(pSettings->r_u32(ammo_types[1].c_str(), "inv_grid_x") * INV_GRID_WIDTH(isHQIcons));
				tex_rect.y1 = float(pSettings->r_u32(ammo_types[1].c_str(), "inv_grid_y") * INV_GRID_HEIGHT(isHQIcons));
				tex_rect.x2 = float(pSettings->r_u32(ammo_types[1].c_str(), "inv_grid_width") * INV_GRID_WIDTH(isHQIcons));
				tex_rect.y2 = float(pSettings->r_u32(ammo_types[1].c_str(), "inv_grid_height") * INV_GRID_HEIGHT(isHQIcons));
				tex_rect.rb.add(tex_rect.lt);
			}
			m_stAmmoType2->SetTextureRect(tex_rect);
			m_stAmmoType2->TextureOn();
			m_stAmmoType2->SetStretchTexture(true);

			if (isHQIcons)
				m_stAmmoType2->SetWndSize(Fvector2().set((tex_rect.x2 - tex_rect.x1) * UI().get_current_kx() / 2, (tex_rect.y2 - tex_rect.y1) / 2));
			else
				m_stAmmoType2->SetWndSize(Fvector2().set((tex_rect.x2 - tex_rect.x1) * UI().get_current_kx(), tex_rect.y2 - tex_rect.y1));
		}
	}
}

bool CUIWpnParams::Check(CInventoryItem& wpn_section)
{
	LPCSTR wpn_sect = wpn_section.object().cNameSect().c_str();
	if (pSettings->line_exist(wpn_sect, "fire_dispersion_base"))
	{
		if (smart_cast<CSilencer*>(&wpn_section))
			return false;
		if (smart_cast<CWeaponBinoculars*>(&wpn_section))
			return false;
		if (smart_cast<CWeaponKnife*>(&wpn_section))
			return false;

        return true;		
	}
	return false;
}

// -------------------------------------------------------------------------------------------------

CUIConditionParams::CUIConditionParams()
{
	AttachChild( &m_progress );
	AttachChild( &m_text );
}

CUIConditionParams::~CUIConditionParams()
{
}

void CUIConditionParams::InitFromXml(CUIXml& xml_doc)
{
	if (!xml_doc.NavigateToNode("condition_params", 0))	return;
	CUIXmlInit::InitWindow	(xml_doc, "condition_params", 0, this);
	CUIXmlInit::InitStatic	( xml_doc, "condition_params:caption", 0, &m_text );
	m_progress.InitFromXml	( xml_doc, "condition_params:progress_state" );
}

void CUIConditionParams::SetInfo( CInventoryItem const* slot_item, CInventoryItem const& cur_item )
{
	float cur_value  = cur_item.GetConditionToShow() * 100.0f + 1.0f - EPS;
	float slot_value = cur_value;

	if ( slot_item && (slot_item != &cur_item) /*&& (cur_item.object().cNameSect()._get() == slot_item->object().cNameSect()._get())*/ )
	{
		slot_value = slot_item->GetConditionToShow() * 100.0f + 1.0f - EPS;
	}
	m_progress.SetTwoPos( cur_value, slot_value );
}
