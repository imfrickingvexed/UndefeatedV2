#pragma once
#include "stdafx.h"
#include "../Utils/Hitbox.h"
#include "Misc.h"
#include "../Utils/LocalInfo.h"
#include "../Utils/Playerlist.h"
/*
class Legit {
	int GetBestPoint(Vector& aimspot, Hitbox box) {
		Vector Aimangles;
		Misc::CalcAngle(Hacks.LocalPlayer->GetEyePosition(), box.points[0], Aimangles);
		if (Misc::FovTo(Hacks.CurrentCmd->viewangles, Aimangles) > Menu::AimbotMenu::Selection::Fov.value) return false;
		float bestdamage = 0;
		for (int index = 0; index < 27; ++index) {
			Vector Aimangles;
			Misc::CalcAngle(Hacks.LocalPlayer->GetEyePosition(), box.points[index], Aimangles);
			if (Misc::FovTo(Hacks.CurrentCmd->viewangles, Aimangles) > Menu::AimbotMenu::Selection::Fov.value) continue;
			float damage = Autowall::GetDamage(box.points[index]);
			if (damage > bestdamage) {
				aimspot = box.points[index];
				bestdamage = damage;
			}
		}
		return bestdamage;
	}

	bool LegitAim(float fov, int hitbox) {
		if (!(Menu::LegitMenu::TriggerbotEnable.active || Hacks.CurrentCmd->buttons & IN_ATTACK && Misc::bullettime() && LocalInfo.Choked < 13)) return false;
		std::vector<Vector> possibleaimspots;
		std::vector<CBaseEntity*> possibletargets;
		for (auto i = 0; i <= Interfaces.pEntList->GetHighestEntityIndex(); i++) {
			auto pEntity = static_cast<CBaseEntity*> (Interfaces.pEntList->GetClientEntity(i));
			if (pEntity == nullptr) continue;
			if (pEntity == Hacks.LocalPlayer) continue;
			if (!pEntity->isAlive()) continue;
			if (!(pEntity->GetHealth() > 0)) continue;
			if (pEntity->GetTeam() == Hacks.LocalPlayer->GetTeam() && !Menu::AimbotMenu::Selection::TeamEnable.active) continue;
			if (pEntity->IsDormant()) continue;
			//if(GetChockedPackets(pEntity) == 0) continue;
			if (pEntity->HasGunGameImmunity()) continue;
			player_info_t info;
			if (!(Interfaces.pEngine->GetPlayerInfo(pEntity->GetIndex(), &info))) continue;
			possibletargets.emplace_back(pEntity);
		}
		if ((int)possibletargets.size()) {
			for (auto pEntity : possibletargets) {
				Hitbox box;
				if (!box.GetHitbox(pEntity, hitbox)) continue;
				Vector Aimspot;
				if (GetBestPoint(Aimspot, box) != 0) {
					//FindActualHitbox(pEntity, Aimspot);

					plist.Update();
					PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
					Player->entity = pEntity;
					possibleaimspots.emplace_back(Aimspot);
				}
			}
		}
		for (auto Spot : possibleaimspots) {
			Vector vecCurPos = Hacks.LocalPlayer->GetEyePosition();
			Vector angs;
			Misc::CalcAngle(vecCurPos, Spot, angs);
			Vector localangs;
			Interfaces.pEngine->GetViewAngles(localangs);
			if (Misc::FovTo(localangs, angs) <= fov) {
				if (!Menu::LegitMenu::SilentEnable.active) Interfaces.pEngine->SetViewAngles(angs);
				Hacks.CurrentCmd->viewangles = angs;
				Hacks.CurrentCmd->buttons |= IN_ATTACK;
				Hacks.SendPacket = false;
				return true;
			}
		}
	}

	void GetSettings(float& fov, float& recoil, int& Hitbox) {
		if (Hacks.LocalWeapon->isRifle() || Hacks.LocalWeapon->isSmgW()) {
			fov = Menu::LegitMenu::Rifle::FOV.value;
			recoil = Menu::LegitMenu::Rifle::Recoil.value;
			Hitbox = Menu::LegitMenu::Rifle::Hitbox.activeid;
		}
		else if (Hacks.LocalWeapon->isSniper()) {
			fov = Menu::LegitMenu::Sniper::FOV.value;
			recoil = Menu::LegitMenu::Sniper::Recoil.value;
			Hitbox = Menu::LegitMenu::Sniper::Hitbox.activeid;
		}
		else if (Hacks.LocalWeapon->isPistol()) {
			fov = Menu::LegitMenu::Pistol::FOV.value;
			recoil = Menu::LegitMenu::Pistol::Recoil.value;
			Hitbox = Menu::LegitMenu::Pistol::Hitbox.activeid;
		}
	}

public:
	void Run() {
		if (!Menu::LegitMenu::LegitEnable.active) return;
		if (!LocalInfo.Flags & FL_ONGROUND && Hacks.LocalPlayer->GetFlags() & FL_ONGROUND && Hacks.CurrentCmd->buttons & IN_JUMP && Menu::LegitMenu::Bhop.active) Hacks.CurrentCmd->buttons &= ~IN_JUMP;
		float fov = 0, recoil = 0;
		int hitbox = 0;
		GetSettings(fov, recoil, hitbox);
		LegitAim(fov, hitbox);
		Hacks.CurrentCmd->viewangles -= LocalInfo.PunchAns * (recoil * 2);
	}
} LegitBot;
*/
