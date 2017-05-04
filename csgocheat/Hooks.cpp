#include "stdafx.h"
#include "Tools/Menu/Menu.h"
#include "Tools/Hacks/Misc.h"
#include "Tools/Utils/Playerlist.h"
#include "Tools/Hacks/Esp.h"
#include "Tools/Hacks/Antiaim.h"
#include "Tools/Hacks/Aimbot.h"
#include "Tools/Hacks/Legit.h"
#include "Tools/Hacks/CreateMoveETC.h"
#include "Tools/Menu/SettingsManager.h"
#include "Tools/Hacks/UI/UI.h"
#include "Tools/Hacks/KnifeBot.h"
#define DEV

#define PI_2 PI/2
#define PI_4 PI/4
CHackManager Hacks;

VTHookManager VMTPanel;
VTHookManager VMTClient;
VTHookManager VMTEngine;
VTHookManager VMTModelRender;
VTHookManager VMTClientMode;
VTHookManager VMTGameEvent;
VTHookManager VMTSurface;

class Hitbox;

std::vector< int > AutoStraferConstants = {};
std::vector< const char* > smoke_materials =
{
	"particle/vistasmokev1/vistasmokev1_smokegrenade",
	"particle/vistasmokev1/vistasmokev1_emods",
	"particle/vistasmokev1/vistasmokev1_emods_impactdust",
	"particle/vistasmokev1/vistasmokev1_fire",
};

typedef void(*Fucntion)(IGameEvent* event);

void ChatMsg(IGameEvent* event)
{
	GUI.ChatBox.AddMessage(Message(false, event->GetInt("userid"), (char*)event->GetString("text")));
}

void Death(IGameEvent* event)
{
	GUI.Feed.AddDeath(DeathInfo(event->GetInt("attacker"), event->GetInt("userid"), (char*)event->GetString("weapon"), event->GetInt("headshot")));
}

class CGameEventListener : public IGameEventListener2
{
private:
	std::string eventName;
	Fucntion Call;
	bool server;
public:
	CGameEventListener() : eventName(""), Call(nullptr), server(false)
	{
	}

	CGameEventListener(const std::string& gameEventName, const Fucntion Func, bool Server) : eventName(gameEventName), Call(Func), server(Server)
	{
		this->RegListener();
	}

	virtual void FireGameEvent(IGameEvent* event)
	{
		this->Call(event);
	}

	virtual int IndicateEventHandling(void)
	{
		return 0x2A;
	}
	 
	void RegListener(void)
	{
		static bool first = true;
		if (!first)
			return;
		if (Interfaces.g_GameEventMgr->AddListener(this, this->eventName.c_str(), server))
			Hacks.listeners.push_back(this);
	}
};

#define REG_EVENT_LISTENER(p, e, n, s) p = new CGameEventListener(n, e, s)
CGameEventListener* Deaths;
CGameEventListener* Msgs;

void Init()
{
	REG_EVENT_LISTENER(Deaths, &Death, "player_death", false);
	REG_EVENT_LISTENER(Msgs, &ChatMsg, "player_say", false);
}

void __stdcall Hooked_PaintTraverse(unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	Hacks.oPaintTraverse(Interfaces.pPanel, vguiPanel, forceRepaint, allowForce);

	if (strcmp("FocusOverlayPanel", Interfaces.pPanel->GetName(vguiPanel)))
	{
		return;
	}
	if (Interfaces.pEngine->IsConnected() && Interfaces.pEngine->IsInGame())
	{
		Hacks.LocalPlayer = Interfaces.pEntList->GetClientEntity(Interfaces.pEngine->GetLocalPlayer());
		Hacks.LocalWeapon = Hacks.LocalPlayer->GetActiveBaseCombatWeapon();

		if (Hacks.LocalPlayer)
		{
			Misc::DrawScope();
			Esp::DoEsp();
			GUI.Draw();
		}
	}
	Esp::Watermark();
	DrawMenu();
}

void New_CreateMove(CInput* thisptr, void* _EAX, int sequence_number, float input_sample_frametime, bool active)
{
	CInput::CVerifiedUserCmd* VerifiedCmd;
	AA.ShouldAA = false;
	CreateMoveETC::GetCmds(sequence_number, Hacks.CurrentCmd, VerifiedCmd);
	Aimbot.Target = Vector(0, 0, 0);

	Misc::ServerRankReveal();

	if (Hacks.CurrentCmd && VerifiedCmd && Interfaces.pEngine->IsConnected() && Interfaces.pEngine->IsInGame() && Hacks.LocalPlayer)
	{
		//plist.Update();
		MEMCHECK;
		Misc::ClantagChanger();
		Hacks.LocalWeapon = Hacks.LocalPlayer->GetActiveBaseCombatWeapon();
		Vector OrigAng = Hacks.CurrentCmd->viewangles;
		Misc::Normalize(OrigAng);
		//Misc::ThirdPerson();
		CreateMoveETC::BhopMovement(OrigAng);
		if (Hacks.LocalWeapon)
		{
			if (Hacks.LocalPlayer->isAlive())
			{
			/*	if( Settings.GetMenuSetting( Tab_Misc, Misc_Aimstep ) )
				{
					for (int i = 0; i < Interfaces.pEntList->GetHighestEntityIndex(); i++ )
					{
						CBaseEntity* pEntity = Interfaces.pEntList->GetClientEntity( i );
						if( pEntity )
						{
							string s = "echo " + std::to_string( Hacks.CurrentCmd->tick_count ) + ":" + std::to_string( pEntity->pelvisangs() );
							Interfaces.pEngine->ClientCmd_Unrestricted( s.c_str(), 0 );
						}
					}
				}*/

				CreateMoveETC::LocalPrediction();
				Settings.weaponconfigs();
				//LegitBot.Run();
				if (Settings.GetSetting(Tab_Misc, Misc_KnifeBot))
				KnifeBot::Run();
				if (Hacks.LocalWeapon->HasAmmo())
				{
					Aimbot.Aim(Hacks.CurrentCmd);
					if (Misc::Shooting() && LocalInfo.Choked < 13)
					{
						if (!Settings.GetSetting(Tab_Ragebot, Ragebot_GOTV) && !Hacks.LocalWeapon->IsMiscWeapon())
							Hacks.SendPacket = false;
						if (!Settings.GetSetting(Tab_Ragebot, Ragebot_SilentAim) && Settings.GetSetting(Tab_Ragebot, Ragebot_AimbotEnabled))
							Interfaces.pEngine->SetViewAngles(Hacks.CurrentCmd->viewangles);
						if (Settings.GetSetting(Tab_Ragebot, Ragebot_AimbotEnabled))
							Hacks.CurrentCmd->viewangles -= LocalInfo.PunchAns * (vec_t)2.f;
					}
					else if (!Hacks.LocalWeapon->IsNade())
						Hacks.CurrentCmd->buttons &= ~IN_ATTACK;
					if (*Hacks.LocalWeapon->GetItemDefinitionIndex() == weapon_revolver && Settings.GetSetting(Tab_Ragebot, Ragebot_AimbotEnabled))
					{
						if (Hacks.LocalWeapon->GetPostponeFireReadyTime() - Misc::GetServerTime() > 0.05)
							Hacks.CurrentCmd->buttons |= IN_ATTACK;
					}
				}
				else
				{
					Hacks.CurrentCmd->buttons &= ~IN_ATTACK;
					Hacks.CurrentCmd->buttons |= IN_RELOAD;
				}
				if (!Misc::Shooting())
					AA.Run();
			}
		}

		CreateMoveETC::CleanUp(OrigAng);
		LocalInfo.Choked = Hacks.SendPacket ? 0 : LocalInfo.Choked + 1;
		CreateMoveETC::VerifyCmd(VerifiedCmd);
		//plist.UpdateSim();
	}
}

void __declspec(naked) __fastcall Hooked_Createmove(CInput* thisptr, void* _EAX, int sequence_number, float input_sample_frametime, bool active)
{
	Hacks.SendPacket = true;
	__asm
	{
		MOV Hacks.SendPacket, BL
		PUSH EBP
		MOV EBP, ESP
		SUB ESP, 8
		PUSHAD
		PUSH active
		PUSH input_sample_frametime
		PUSH sequence_number
		CALL Hacks.oCreateMove
	}
	Hacks.LocalPlayer = Interfaces.pEntList->GetClientEntity(Interfaces.pEngine->GetLocalPlayer());
	New_CreateMove(thisptr, _EAX, sequence_number, input_sample_frametime, active);
	__asm
	{
		POPAD
		MOV BL, Hacks.SendPacket
		MOV ESP, EBP
		POP EBP
		RETN 0x0C
	}
}

void __stdcall Hooked_OverrideView(CViewSetup* pSetup)
{
	if (Interfaces.pEngine->IsInGame() && Interfaces.pEngine->IsConnected() && Settings.GetSetting(Tab_Misc, Misc_Fov) > 0)
	{
		pSetup->fov += Settings.GetSetting(Tab_Misc, Misc_Fov);
		CHECKMEM;
		//pSetup->angles = Aimbot.Angles;
	}
	Hacks.FOV = pSetup->fov;
	Hacks.oOverrideView(pSetup);
}

void __stdcall Hooked_PlaySound(const char* pSample)
{
	Hacks.oPlaySound(pSample);

	if (strstr(pSample, "weapons/hegrenade/beep.wav"))
	{
		if (!Settings.GetMenuSetting(Tab_Misc, Misc_AutoAccept))
			return;

		Interfaces.pEngine->ClientCmd_Unrestricted("echo SOUND_FILE_FOUND", 0);
		DWORD dwIsReady = (Utils.PFindPattern("client.dll", "55 8B EC 83 E4 F8 83 EC 08 56 8B 35 ?? ?? ?? ?? 57 8B 8E"));
		reinterpret_cast< void(*)() >(dwIsReady)();
	}
}

/*void PoseParamProxy(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
float p = pData->m_Value.m_Float;
*(float*)pOut = p;

CBaseEntity* ent = static_cast<CBaseEntity*>(pStruct);

PlayerList::CPlayer* Player = plist.FindPlayer(ent);
Player->entity = ent;
Player->pelvisang = p;
}

void AutoResolver(Vector* & Angle, PlayerList::CPlayer* Player) {
Player->reset.y = Angle->y;

Player->difference = Angle->y - Player->pelvisang;
Angle->y -= Player->difference;
}*/

void AutoResolver(Vector* & Angle, PlayerList::CPlayer* Player)
{
	if (Settings.GetMenuSetting(Tab_Misc, Misc_Aimstep))
	{
		string s = "echo " + std::to_string( Hacks.CurrentCmd->tick_count ) + ":" + std::to_string( Player->entity->pelvisangs() );
		Interfaces.pEngine->ClientCmd_Unrestricted( s.c_str(), 0 );
	}

	static int iLastUpdatedTick = 0;

	Player->reset.y = Angle->y;

	static Vector orginalview = Vector(0, 0, 0);
	if(orginalview.x != Angle->x)
		orginalview.x = Angle->x;
	if (Angle->y != Player->resolved)
	{
		orginalview.y = Angle->y;

		float flResolve = 0.f;
		float flLowerBodyYaw = Player->entity->pelvisangs();

		int difference = orginalview.y - flLowerBodyYaw;

		iLastUpdatedTick++;

		if (flLowerBodyYaw != Player->flLastPelvisAng)
		{
			if (Player->entity->GetVecVelocity().Length2D() == 0)
			{
				int temp = static_cast<int>(floor(Player->flEyeAng - Player->flLastPelvisAng));
				while (temp < 0)
					temp += 360;
				while (temp > 360)
					temp -= 360;
				Player->Backtrack[temp] = flLowerBodyYaw - Player->flEyeAng;
			}

			iLastUpdatedTick = 0;
			Player->flLastPelvisAng = flLowerBodyYaw;
			Player->flEyeAng = orginalview.y;
		}

		if (Player->entity->GetVecVelocity().Length2D() >= 1)
		{
			flResolve = flLowerBodyYaw;
		}
		else
		{
			int temp = static_cast<int>(floor(orginalview.y - flLowerBodyYaw));
			while (temp < 0)
				temp += 360;
			while (temp > 360)
				temp -= 360;
			flResolve = Player->Backtrack[temp] + orginalview.y;
		}
		Angle->y = flResolve;
		Player->resolved = Angle->y;
	}
}

void __stdcall Hooked_Frame_Stage_Notify(ClientFrameStage_t curStage)
{
	Hacks.LocalPlayer = Interfaces.pEntList->GetClientEntity(Interfaces.pEngine->GetLocalPlayer());
	if (Hacks.LocalPlayer)
	{
		LocalInfo.PunchAns = *Hacks.LocalPlayer->GetPunchAnglePtr();
		LocalInfo.Flags = Hacks.LocalPlayer->GetFlags();
	}

	ConVar* mat_postprocess_enable = Interfaces.g_ICVars->FindVar("mat_postprocess_enable");
	mat_postprocess_enable->SetValue(0);

	Vector*pPunchAngle = nullptr, *pViewPunchAngle = nullptr, vPunchAngle, vViewPunchAngle;
	if (Interfaces.pEngine->IsInGame() && Interfaces.pEngine->IsConnected())
	{
		if (Hacks.LocalPlayer)
		{
			Hacks.LocalWeapon = Hacks.LocalPlayer->GetActiveBaseCombatWeapon();
			if (curStage == FRAME_RENDER_START)
			{
				MEMCHECK;
				if (Settings.GetSetting(Tab_Visuals, Visuals_NoRecoil))
				{
					pPunchAngle = Hacks.LocalPlayer->GetPunchAnglePtr();
					pViewPunchAngle = Hacks.LocalPlayer->GetViewPunchAnglePtr();

					if (pPunchAngle && pViewPunchAngle)
					{
						vPunchAngle = *pPunchAngle;
						pPunchAngle->Init();
						vViewPunchAngle = *pViewPunchAngle;
						pViewPunchAngle->Init();
					}
				}
				for (auto matName : smoke_materials)
				{
					IMaterial* mat = Interfaces.pMaterialSystem->FindMaterial(matName, "Other textures");
					mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, Settings.GetSetting(Tab_Visuals, Visuals_NoSmoke));
				}
			}
		}
		if (*reinterpret_cast<bool* >(reinterpret_cast<DWORD >(Interfaces.pInput) + 0xA5))
			*reinterpret_cast<Vector* >(reinterpret_cast<DWORD >(Hacks.LocalPlayer) + 0x31C8) = Hacks.LastAngles;
	}

	Hacks.oFrameStageNotify(curStage);

	if (pPunchAngle && pViewPunchAngle)
	{
		*pPunchAngle = vPunchAngle;
		*pViewPunchAngle = vViewPunchAngle;
	}
	static auto spin = 0;
	if (Settings.GetMenuSetting(Tab_Misc, Misc_Aimstep))
	{
		/*std::array< float, 24 >* flPoseParameters = Hacks.LocalPlayer->GetPoseParameters();

		for (int i = 0; i < 24; i++)
		{
			Interfaces.pEngine->ClientCmd_Unrestricted(string("echo " + std::string("#" + std::to_string(i) + ": " + std::to_string(flPoseParameters->at(i) * 360.f) + "\n")).c_str(), 0);
		}
		Interfaces.pEngine->ClientCmd_Unrestricted(string("echo " + std::string("EYE_YAW: " + std::to_string(Hacks.LocalPlayer->GetEyeAngles().y) + "\n")).c_str(), 0);
		Interfaces.pEngine->ClientCmd_Unrestricted(string("echo " + std::string("LB_YAW: " + std::to_string(Hacks.LocalPlayer->pelvisangs()) + "\n")).c_str(), 0);

		Settings.SetSetting(Tab_Misc, Misc_Aimstep, false);*/
	}
	if (Settings.GetSetting(Tab_Ragebot, Ragebot_Resolver))
	{
		//spin += 120;
		//if( spin > 180 )
		//	spin -= 360;
		plist.Update();
		if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{
			if (Interfaces.pEngine->IsInGame() && Interfaces.pEngine->IsConnected())
			{
				for (auto i = 0; i <= Interfaces.pEntList->GetHighestEntityIndex(); i++)
				{
					auto pEntity = static_cast< CBaseEntity* >(Interfaces.pEntList->GetClientEntity(i));
					if (pEntity == nullptr)
						continue;
					if (pEntity == Hacks.LocalPlayer)
						continue;
					if (!Hacks.LocalPlayer->isAlive())
						continue;
					if (pEntity->GetTeam() == Hacks.LocalPlayer->GetTeam())
						continue;
					if (!pEntity->isAlive())
						continue;
					if (!(pEntity->GetHealth() > 0))
						continue;
					if (pEntity->IsDormant())
						continue;
					player_info_t info;
					if (!(Interfaces.pEngine->GetPlayerInfo(pEntity->GetIndex(), &info)))
						continue;
					PlayerList::CPlayer* Player = plist.FindPlayer(pEntity);
					Player->entity = pEntity;

					Vector* angs = Player->entity->GetEyeAnglesPointer();
					if (!Settings.GetSetting(Tab_Misc, Misc_AntiUntrusted))
					{
						if (angs->x > 90.0f && angs->x < 270.f)
						{
							if (angs->x > 180.f)
								angs->x = -89.f;
							else
								angs->x = 89.0f;
						}
						//angs->y = spin;
					}
					AutoResolver(angs, Player);
					/*Vector AimAngles;
					CalcAngle(Player->entity->GetEyePosition(), Hacks.LocalPlayer->GetVecOrigin(), AimAngles);
					if (AimAngles.IsValid ())
					{
					if (Player->entity->GetFlags() & FL_ONGROUND)
					Player->LastDifference = Player->entity->pelvisangs() - AimAngles.y;
					angs->y = Player->LastDifference + AimAngles.y;
					}*/
				}
			}
		}
	}
}

void InitKeyValues(KeyValues* keyValues, char* name)
{
	DWORD dwFunction = (DWORD)Utils.PatternSearch("client.dll", (PBYTE)"\x68\x00\x00\x00\x00\x8B\xC8\xE8\x00\x00\x00\x00\x89\x45\xFC\xEB\x07\xC7\x45\x00\x00\x00\x00\x00\x8B\x03\x56", "x????xxx????xxxxxxx?????xxx", NULL, NULL);
	dwFunction += 7;
	dwFunction = dwFunction + *reinterpret_cast< PDWORD_PTR >(dwFunction + 1) + 5;
	__asm
	{
		push name
		mov ecx, keyValues
		call dwFunction
	}
}

void LoadFromBuffer(KeyValues* keyValues,
	char const* resourceName, const char* pBuffer)
{
	DWORD dwFunction = (DWORD)Utils.PatternSearch("client.dll", (PBYTE)"\xE8\x00\x00\x00\x00\x8A\xD8\xFF\x15\x00\x00\x00\x00\x84\xDB", "x????xxxx????xx", NULL, NULL);
	dwFunction = dwFunction + *reinterpret_cast< PDWORD_PTR >(dwFunction + 1) + 5;
	__asm
	{
		push 0
		push 0
		push 0
		push pBuffer
		push resourceName
		mov ecx, keyValues
		call dwFunction
	}
}

IMaterial* Create_Material(bool Ignore, bool Lit, bool Wireframe)
{
	static int created = 0;
	static const char tmp[] =
	{
		"\"%s\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"\"\
		\n\t\"$ignorez\" \"%i\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$flat\" \"1\"\
		\n\t\"$nocull\"  \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\"  \"0\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"%i\"\
        \n}\n"
	};
	char* baseType = (Lit == true ? "VertexLitGeneric" : "UnlitGeneric");
	char material[512];
	char name[512];
	sprintf_s(material, sizeof(material), tmp, baseType, (Ignore) ? 1 : 0, (Wireframe) ? 1 : 0);
	sprintf_s(name, sizeof(name), "#Aimpulse_Chams_%i.vmt", created);
	++created;
	KeyValues* keyValues = static_cast< KeyValues* >(malloc(sizeof(KeyValues)));
	InitKeyValues(keyValues, baseType);
	LoadFromBuffer(keyValues, name, material);
	IMaterial* createdMaterial = Interfaces.pMaterialSystem->CreateMaterial(name, keyValues);
	createdMaterial->IncrementReferenceCount();
	return createdMaterial;
}

bool Do_Chams(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld)
{
	static IMaterial* Covered_Lit = Create_Material(true, true, false);
	static IMaterial* Visable_Lit = Create_Material(false, true, false);

	CBaseEntity* Model_Entity = Interfaces.pEntList->GetClientEntity(pInfo.entity_index);
	auto Model_Name = Interfaces.g_pModelInfo->GetModelName(const_cast< model_t* >(pInfo.pModel));
	if (Settings.GetSetting(Tab_Visuals, Visuals_Chams))
	{
		if (Model_Entity->IsPlayer())
		{
			if (Model_Entity->GetTeam() == Hacks.LocalPlayer->GetTeam() && !Settings.GetSetting(Tab_Visuals, Visuals_ChamsTeam))
				return false;

			if (Model_Entity->GetTeam() == 2)
			{
				Covered_Lit->ColorModulate(180 / 255.f, 137 / 255.f, 180 / 255.f); // t colour
				Visable_Lit->ColorModulate(255 / 255.f, 137 / 255.f, 255 / 255.f); // t colour Visable
			}
			else
			{
				Covered_Lit->ColorModulate(0 / 255.f, 141 / 255.f, 180 / 255.f); // ct colour
				Visable_Lit->ColorModulate(0 / 255.f, 200 / 255.f, 255 / 255.f); // ct colour Visable
			}

			if (!Model_Entity->isAlive())
				return false;

			if (Model_Entity->HasGunGameImmunity())
				Covered_Lit->AlphaModulate(0.75f);
			else
				Covered_Lit->AlphaModulate(1.f);

			if (!Settings.GetSetting(Tab_Visuals, Visuals_ChamsXQZ))
			{
				Interfaces.g_pModelRender->ForcedMaterialOverride(Covered_Lit, OVERRIDE_NORMAL);
				Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
			}

			Interfaces.g_pModelRender->ForcedMaterialOverride(Visable_Lit, OVERRIDE_NORMAL);
			Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

			return true;
		}
		if (Model_Entity->IsWeapon() && Settings.GetSetting(Tab_Visuals, Visuals_ChamsGuns))
		{
			Covered_Lit->ColorModulate(0 / 255.f, 237 / 255.f, 255 / 255.f);
			Visable_Lit->ColorModulate(0 / 255.f, 200 / 255.f, 255 / 255.f);
			Visable_Lit->AlphaModulate(1.0f);
			Covered_Lit->AlphaModulate(1.0f);

			if (!Settings.GetSetting(Tab_Visuals, Visuals_ChamsXQZ))
			{
				Interfaces.g_pModelRender->ForcedMaterialOverride(Covered_Lit, OVERRIDE_NORMAL);
				Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
			}

			Interfaces.g_pModelRender->ForcedMaterialOverride(Visable_Lit, OVERRIDE_NORMAL);
			Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

			return true;
		}
	}
	if (Settings.GetSetting(Tab_Visuals, Visuals_NoFlash))
	{
		IMaterial* Flash = Interfaces.pMaterialSystem->FindMaterial("effects\\flashbang", "ClientEffect textures");
		IMaterial* FlashWhite = Interfaces.pMaterialSystem->FindMaterial("effects\\flashbang_white", "ClientEffect textures");
		Flash->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		FlashWhite->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		return false;
	}
	return false;
}

void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t& pInfo, matrix3x4* pCustomBoneToWorld)
{
	if (!Do_Chams(thisptr, edx, ctx, state, pInfo, pCustomBoneToWorld))
		Hacks.oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
	Interfaces.g_pModelRender->ForcedMaterialOverride(nullptr, OVERRIDE_NORMAL);
}

void __stdcall Hooked_ClientCmdUnrestricted(const char* szCmdString, char flag)
{
	string str(szCmdString);
	std::string prefix;
	if (!str.compare(0, string("Clantag ").size(), "Clantag "))
	{
		str.replace(0, string("Clantag ").size(), "");
		prefix = string("\\n");
		std::size_t found = str.find(prefix);
		if (found != std::string::npos)
			str.replace(found, found + prefix.size(), "\n");
		Misc::SetClanTag(str.c_str(), "");
	}
	else if (!str.compare(0, string("ReleaseName").size(), "ReleaseName"))
	{
		Misc::setName("\n\xAD\xAD\xAD­­­");
	}
	else if (!str.compare(0, string("Name ").size(), "Name "))
	{
		str.replace(0, string("Name ").size(), "");
		prefix = string("\\n");
		std::size_t found = str.find(prefix);
		if (found != std::string::npos)
			str.replace(found, found + prefix.size(), "\n");
		Misc::setName(str.c_str());
	}
	/*
	else if (!str.compare(0, string("Save ").size(), "Save ")) {
	str.replace(0, string("Save ").size(), "");
	std::ofstream output(str);
	Menu::Menu.Save(output);
	}
	else if (!str.compare(0, string("Load ").size(), "Load ")) {
	str.replace(0, string("Load ").size(), "");
	std::ifstream input(str);
	if (input.good()) Menu::Menu.Load(input);
	}
	else if (!str.compare(0, string("Delete ").size(), "Delete ")) {
	str.replace(0, string("Delete ").size(), "");
	std::ofstream output(str);
	if (output.good()) {
	output.close();
	std::remove(str.c_str());
	}
	}
	*/
	else
		Hacks.oClientCmdUnresticted(szCmdString, flag);
}

LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//if (Menu::Menu.active) return true;

	return CallWindowProc(INIT::OldWindow, hWnd, uMsg, wParam, lParam);
}