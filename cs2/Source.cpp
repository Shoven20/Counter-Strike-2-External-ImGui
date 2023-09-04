#include "overlay.h"
#include "CSSPlayer.hpp"
#include "utils.hpp"
#include "driverloader.h"

 
ImVec4 enemyespcolor(255, 255, 255, 255);
ImVec4 teamespcolor(0, 255, 0, 255);

 

bool Aimbot = false;
int AimbotValue = 60;
int AimbotSmooth = 1;

bool EnemyRainbow = false;
bool TeamRainbow = false;
bool TeamVisible = false;
bool TeamIgnore = false;

bool EspStroke = false;
bool BoxESP = false;
int espstyle;
const char* selectespstyle[]{ ("Corner Box"), ("3D Box"), ("2D Box") };

int healthbarstyle;
const char* selecthealthbarstyle[]{ ("Corner"), ("Top"), ("Bottom") };

bool Fillbox = false;
bool HealthBar = false;
bool Distance = false;
bool NameTag = false;
bool Snapline = false;

bool showmenu = true;
int sayfa = 1;

void RenderUI()
{
	ImVec2 s, p, gs = ImVec2{ 520, 395 };
	ImGui::SetNextWindowSize(ImVec2(gs));
	POINT mPos;
	GetCursorPos(&mPos);
	static int selected = 0;
	static int sub_selected = 0;
	ImGui::Begin(_xor_("IMGUI PAGE").c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar); {
		ImGui::AlignTextToFramePadding();
		auto pos = ImGui::GetWindowPos();
		auto size = ImGui::GetWindowSize();
		auto isHovering = mPos.x >= pos.x && mPos.y >= pos.y && mPos.x <= (pos.x + size.x) && mPos.y <= (pos.y + size.y);

		auto draw = ImGui::GetWindowDrawList();

		draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + size.x, pos.y + 35), ImColor(20, 20, 20, 255)); // upper rect

		draw->AddLine(ImVec2(pos.x, pos.y + 35), ImVec2(pos.x + size.x, pos.y + 35), ImColor(255, 255, 255, 15)); // upper line

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 340);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 20);
		ImGui::BeginGroup();
		if (selected == 0) {
			ImGui::BeginGroup();
			ImGui::SetCursorPos(ImVec2(15, 14));
			if (custom_interface::subtab(_xor_("AIMBOT").c_str(), 0 == sub_selected)) {
				sub_selected = 0;
			}
			ImGui::SetCursorPos(ImVec2(75, 14));
			if (custom_interface::subtab(_xor_("VISUALS").c_str(), 1 == sub_selected)) {
				sub_selected = 1;
			}
			ImGui::EndGroup();
			ImGui::BeginGroup();
			ImGui::SetCursorPos(ImVec2(300, 10));
			ImGui::Text(_xor_("Counter-Strike 2 EXTERNAL").c_str());
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7);
			ImGui::TextColored(ImColor(255, 0, 0, 255), _xor_(" SHOVEN").c_str());
			ImGui::EndGroup();
			 
			ImGui::SetCursorPos(ImVec2(15, 44));
			ImGui::BeginChild(_xor_("esp").c_str(), ImVec2(240, 342));
			{
				if (sub_selected == 0)
				{
					ImGui::Checkbox(_xor_("Aimbot").c_str(), &Aimbot);
 					HotkeyButton(AimbotButon, ChangeKey, keystatus);
					ImGui::SliderInt(_xor_("Aimbot Fov").c_str(), &AimbotValue, 10, 360);
					ImGui::SliderInt(_xor_("Aimbot Smooth").c_str(), &AimbotSmooth, 1, 10);
					ImGui::Checkbox(_xor_("TeamIgnore").c_str(), &TeamIgnore);
				}
				if (sub_selected == 1)
				{
					ImGui::Checkbox(_xor_("Box Esp").c_str(), &BoxESP);
 					ImGui::Checkbox(_xor_("Esp Stroke").c_str(), &EspStroke);
					ImGui::Checkbox(_xor_("Health Bar").c_str(), &HealthBar);
					ImGui::Checkbox(_xor_("Fill Box").c_str(), &Fillbox);
 					ImGui::Checkbox(_xor_("Distance").c_str(), &Distance);
					ImGui::Checkbox(_xor_("NameTag").c_str(), &NameTag);
					ImGui::Checkbox(_xor_("Snapline").c_str(), &Snapline);
				}
			}
			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(262, 44));
			ImGui::BeginChild(_xor_("esp color").c_str(), ImVec2(240, 342));
			{
				if (sub_selected == 1)
				{
					ImGui::Combo(_xor_("Esp Style").c_str(), &espstyle, selectespstyle, IM_ARRAYSIZE(selectespstyle));
					ImGui::Combo(_xor_("Healthbar Style").c_str(), &healthbarstyle, selecthealthbarstyle, IM_ARRAYSIZE(selecthealthbarstyle));
					ImGui::ColorEdit4(_xor_("Enemy Esp Color").c_str(), (float*)&enemyespcolor, ImGuiColorEditFlags_NoDragDrop | ImGuiWindowFlags_NoScrollbar | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox(_xor_("Enemy Rainbow Mode").c_str(), &EnemyRainbow);
					ImGui::ColorEdit4(_xor_("Team Esp Color").c_str(), (float*)&teamespcolor, ImGuiColorEditFlags_NoDragDrop | ImGuiWindowFlags_NoScrollbar | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox(_xor_("Team Rainbow Mode").c_str(), &TeamRainbow);
					ImGui::Checkbox(_xor_("Visible Team").c_str(), &TeamVisible);
				}
			}
			ImGui::EndChild();
 

		}
	}
	ImGui::End();
}


float Calc2D_Distt(const Vector2& Src, const Vector3& Dst) 
{
	float dx = Src.x - Dst.x;
	float dy = Src.y - Dst.y;
	return sqrt(dx * dx + dy * dy);
}

float AimFov(Vector3 ScreenPos)
{
	return Calc2D_Distt(Vector2(ScreenCenterX, ScreenCenterY), ScreenPos);
}

void ColorChange()
{
	static float Color[3];
	static uintptr_t Tickcount, Tickcheck = 0;
	ImGui::ColorConvertRGBtoHSV(enemyespcolor.x, enemyespcolor.y, enemyespcolor.z, Color[0], Color[1], Color[2]);
	if (GetTickCount() - Tickcount >= 1)
	{
		if (Tickcheck != Tickcount)
		{
			Color[0] += 0.001f * -10.f;
			Tickcheck = Tickcount;
		}
		Tickcount = GetTickCount();
	}
	if (Color[0] < 0.0f) Color[0] += 1.0f;
	ImGui::ColorConvertHSVtoRGB(Color[0], Color[1], Color[2], enemyespcolor.x, enemyespcolor.y, enemyespcolor.z);
}

void ColorChange2()
{
	static float Color[3];
	static uintptr_t Tickcount, Tickcheck = 0;
	ImGui::ColorConvertRGBtoHSV(teamespcolor.x, teamespcolor.y, teamespcolor.z, Color[0], Color[1], Color[2]);
	if (GetTickCount() - Tickcount >= 1)
	{
		if (Tickcheck != Tickcount)
		{
			Color[0] += 0.001f * -10.f;
			Tickcheck = Tickcount;
		}
		Tickcount = GetTickCount();
	}
	if (Color[0] < 0.0f) Color[0] += 1.0f;
	ImGui::ColorConvertHSVtoRGB(Color[0], Color[1], Color[2], teamespcolor.x, teamespcolor.y, teamespcolor.z);
}

bool birkere;
bool birkere2;
void render()
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (EnemyRainbow)
	{
		if (!birkere)
		{
			enemyespcolor = ImColor(255, 0, 0);
			birkere = true;
		}
		ColorChange();
	}
	else
	{
		birkere = false;
	}
	if (TeamRainbow)
	{
		if (!birkere2)
		{
			teamespcolor = ImColor(255, 0, 0);
			birkere2 = true;
		}
		ColorChange2();
	}
	else
	{
		birkere2 = false;
	}
		
	view_matrix_t viewmatrix = read<view_matrix_t>(client + viewmatrix_Offset);
	uint64_t localplayer = read<uint64_t>(client + localplayer_Offset);

	Vector3 localpos = read<Vector3>(localplayer + s_Position_Offset);
	int localteam = read<int>(localplayer + s_teamnum_Offset);
	g_dwEntList = read<uint64_t>(client + s_dwEntityList_Offset);
	CCSPlayerController* pPlayerController = nullptr;

	for (int nId = 0; nId < 32; nId++)
	{
		C_BaseEntity* pBaseEntity = C_BaseEntity::GetBaseEntity(nId);
		if (!pBaseEntity || strcmp(pBaseEntity->m_pEntityIdentity()->m_designerName().c_str(), _xor_("cs_player_controller").c_str()))
			continue;

		while (pBaseEntity->m_pEntityIdentity()->m_pPrevByClass())
			pBaseEntity = pBaseEntity->m_pEntityIdentity()->m_pPrevByClass();

		pPlayerController = reinterpret_cast<CCSPlayerController*>(pBaseEntity);
		break;
	}
	for (; pPlayerController; pPlayerController = reinterpret_cast<CCSPlayerController*>(pPlayerController->m_pEntityIdentity()->m_pNextByClass()))
	{
		if (pPlayerController->m_bIsLocalPlayerController())
			continue;

		if (pPlayerController->m_iPawnHealth() <= 0 || pPlayerController->m_iPawnHealth() > 100)
			continue;

		if (!TeamVisible && pPlayerController->Teamnum() == localteam)
			continue;

		Vector3 playerpos = pPlayerController->pos(pPlayerController->m_hPlayerPawn());
		Vector3 pos;

		Vector3 head_pos = Vector3(playerpos.x, playerpos.y, playerpos.z + 63);
		Vector3 head;

		Vector3 aim_pos = Vector3(playerpos.x, playerpos.y, playerpos.z + 45);
		Vector3 aim;
		

		if (!world_to_screen(playerpos, pos, viewmatrix))
			continue;

		if (!world_to_screen(head_pos, head, viewmatrix))
			continue;

		if (!world_to_screen(aim_pos, aim, viewmatrix))
			continue;

 
		const float Entity_h = fabsf(head.y - pos.y);
		const float Entity_w = Entity_h / 1.5f;
		int distance = static_cast<int>(localpos.distancee(playerpos) / 25);
		std::string text;
		if (Distance && NameTag) text = pPlayerController->m_sSanitizedPlayerName() + _xor_(" [").c_str() + std::to_string(distance) + _xor_(" m]").c_str();
		else if (Distance) text = _xor_("[").c_str() + std::to_string(distance) + _xor_("m]").c_str();
		else text = pPlayerController->m_sSanitizedPlayerName();
		if (Distance || NameTag) {
			if(healthbarstyle == 1)
			    DrawString(14, pos.x, head.y - 5, ImColor(enemyespcolor.x, enemyespcolor.y, enemyespcolor.z), true, true, text.c_str());
			else
				DrawString(14, pos.x, head.y, ImColor(enemyespcolor.x, enemyespcolor.y, enemyespcolor.z), true, true, text.c_str());
		}

		if(BoxESP && espstyle == 1)
		{
			Vector3 vBaseBone = playerpos;
			Vector3 bottom1;
			if (!world_to_screen(Vector3(vBaseBone.x + 15, vBaseBone.y - 15, vBaseBone.z), bottom1, viewmatrix)) {
				continue;
			}
			Vector3 bottom2;
			if (!world_to_screen(Vector3(vBaseBone.x - 15, vBaseBone.y - 15, vBaseBone.z), bottom2, viewmatrix)) {
				continue;
			}
			Vector3 bottom3;
			if (!world_to_screen(Vector3(vBaseBone.x - 15, vBaseBone.y + 15, vBaseBone.z), bottom3, viewmatrix)) {
				continue;
			}
			Vector3 bottom4;
			if (!world_to_screen(Vector3(vBaseBone.x + 15, vBaseBone.y + 15, vBaseBone.z), bottom4, viewmatrix)) {
				continue;
			}

			Vector3 vHeadBone = playerpos;
			Vector3 top1;
			if (!world_to_screen(Vector3(vHeadBone.x + 15, vHeadBone.y - 15, head_pos.z + 4), top1, viewmatrix)) {
				continue;																 
			}																			 
			Vector3 top2;																 
			if (!world_to_screen(Vector3(vHeadBone.x - 15, vHeadBone.y - 15, head_pos.z + 4), top2, viewmatrix)) {
				continue;																 
			}																			 
			Vector3 top3;																 
			if (!world_to_screen(Vector3(vHeadBone.x - 15, vHeadBone.y + 15, head_pos.z + 4), top3, viewmatrix)) {
				continue;															 
			}																		 
			Vector3 top4;															 
			if (!world_to_screen(Vector3(vHeadBone.x + 15, vHeadBone.y + 15, head_pos.z + 4), top4, viewmatrix)) {
				continue;
			}


			if (EspStroke) {
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x + 1, bottom1.y + 1), ImVec2(top1.x + 1, top1.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x + 1, bottom2.y + 1), ImVec2(top2.x + 1, top2.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x + 1, bottom3.y + 1), ImVec2(top3.x + 1, top3.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x + 1, bottom4.y + 1), ImVec2(top4.x + 1, top4.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x + 1, bottom1.y + 1), ImVec2(bottom2.x + 1, bottom2.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x + 1, bottom2.y + 1), ImVec2(bottom3.x + 1, bottom3.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x + 1, bottom3.y + 1), ImVec2(bottom4.x + 1, bottom4.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x + 1, bottom4.y + 1), ImVec2(bottom1.x + 1, bottom1.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(top1.x, top1.y + 1), ImVec2(top2.x, top2.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(top2.x, top2.y + 1), ImVec2(top3.x, top3.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(top3.x, top3.y + 1), ImVec2(top4.x, top4.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(top4.x, top4.y + 1), ImVec2(top1.x, top1.y + 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);

				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x - 1, bottom1.y - 1), ImVec2(top1.x - 1, top1.y - 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x - 1, bottom2.y - 1), ImVec2(top2.x - 1, top2.y - 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x - 1, bottom3.y - 1), ImVec2(top3.x - 1, top3.y - 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x - 1, bottom4.y - 1), ImVec2(top4.x - 1, top4.y - 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x - 1, bottom1.y - 1), ImVec2(bottom2.x - 1, bottom2.y - 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x - 1, bottom2.y - 1), ImVec2(bottom3.x - 1, bottom3.y - 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x - 1, bottom3.y - 1), ImVec2(bottom4.x - 1, bottom4.y - 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x - 1, bottom4.y - 1), ImVec2(bottom1.x - 1, bottom1.y - 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(top1.x, top1.y - 1), ImVec2(top2.x, top2.y - 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(top2.x, top2.y - 1), ImVec2(top3.x, top3.y - 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(top3.x, top3.y - 1), ImVec2(top4.x, top4.y - 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(top4.x, top4.y - 1), ImVec2(top1.x, top1.y - 1), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0)), 0.1f);
			}


			ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(top1.x, top1.y), ImGui::ColorConvertFloat4ToU32(enemyespcolor), 0.1f);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(top2.x, top2.y), ImGui::ColorConvertFloat4ToU32(enemyespcolor), 0.1f);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(top3.x, top3.y), ImGui::ColorConvertFloat4ToU32(enemyespcolor), 0.1f);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(top4.x, top4.y), ImGui::ColorConvertFloat4ToU32(enemyespcolor), 0.1f);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(bottom2.x, bottom2.y), ImGui::ColorConvertFloat4ToU32(enemyespcolor), 0.1f);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(bottom3.x, bottom3.y), ImGui::ColorConvertFloat4ToU32(enemyespcolor), 0.1f);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(bottom4.x, bottom4.y), ImGui::ColorConvertFloat4ToU32(enemyespcolor), 0.1f);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(bottom1.x, bottom1.y), ImGui::ColorConvertFloat4ToU32(enemyespcolor), 0.1f);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2(top1.x, top1.y), ImVec2(top2.x, top2.y), ImGui::ColorConvertFloat4ToU32(enemyespcolor), 0.1f);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2(top2.x, top2.y), ImVec2(top3.x, top3.y), ImGui::ColorConvertFloat4ToU32(enemyespcolor), 0.1f);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2(top3.x, top3.y), ImVec2(top4.x, top4.y), ImGui::ColorConvertFloat4ToU32(enemyespcolor), 0.1f);
			ImGui::GetOverlayDrawList()->AddLine(ImVec2(top4.x, top4.y), ImVec2(top1.x, top1.y), ImGui::ColorConvertFloat4ToU32(enemyespcolor), 0.1f);
		}

		if (Snapline)
		{
			if (TeamVisible)
			{
				if (pPlayerController->Teamnum() == localteam)
					DrawSnapline(ScreenCenterX, ScreenCenterY * 2, pos.x, pos.y, teamespcolor, 1.f);
				else
					DrawSnapline(ScreenCenterX, ScreenCenterY * 2, pos.x, pos.y, enemyespcolor, 1.f);
			}
			else
				DrawSnapline(ScreenCenterX, ScreenCenterY * 2, pos.x, pos.y, enemyespcolor, 1.f);
		}
		if (Fillbox && espstyle != 1) ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(pos.x - Entity_w / 2, pos.y), ImVec2(pos.x + Entity_w / 2, pos.y - Entity_h), ImColor(0, 0, 0, 100));
		if (BoxESP && espstyle == 0)
		{
			if (EspStroke) {
				DrawCorneredBox(pos.x - (Entity_w / 2.0f) - 1, pos.y - 1, Entity_w + 2, -Entity_h + 2, ImVec4(0.f, 0.f, 0.f, 255.f), 1.f);
				DrawCorneredBox(pos.x - (Entity_w / 2.0f) + 1, pos.y + 1, Entity_w - 2, -Entity_h - 2, ImVec4(0.f, 0.f, 0.f, 255.f), 1.f);
			}
			if (TeamVisible)
			{
				if (pPlayerController->Teamnum() == localteam)
					DrawCorneredBox(pos.x - (Entity_w / 2.0f), pos.y, Entity_w, -Entity_h, teamespcolor, 1.f);
				else
					DrawCorneredBox(pos.x - (Entity_w / 2.0f), pos.y, Entity_w, -Entity_h, enemyespcolor, 1.f);
			}
			else
			DrawCorneredBox(pos.x - (Entity_w / 2.0f), pos.y, Entity_w, -Entity_h, enemyespcolor, 1.f);
		}
		if (BoxESP && espstyle == 2)
		{
			if (EspStroke) {
				DrawNormalBox(pos.x - (Entity_w / 2.0f) - 1, pos.y - 1, Entity_w + 2, -Entity_h + 2, 1.f,ImVec4(0.f, 0.f, 0.f, 255.f));
				DrawNormalBox(pos.x - (Entity_w / 2.0f) + 1, pos.y + 1, Entity_w - 2, -Entity_h - 2, 1.f,ImVec4(0.f, 0.f, 0.f, 255.f));
			}
			if (TeamVisible)
			{
				if (pPlayerController->Teamnum() == localteam)
					DrawNormalBox(pos.x - (Entity_w / 2.0f), pos.y, Entity_w, -Entity_h, 1.f, teamespcolor);
				else
					DrawNormalBox(pos.x - (Entity_w / 2.0f), pos.y, Entity_w, -Entity_h, 1.f, enemyespcolor);
			}
			else
				DrawNormalBox(pos.x - (Entity_w / 2.0f), pos.y, Entity_w, -Entity_h, 1.f, enemyespcolor);
		}

		if (HealthBar)
		{
			float Health = (float)pPlayerController->m_iPawnHealth();
			if (Health > 100) Health = 100;

			const int entity_half_w = Entity_w / 2;
			const float health_ratio = Health / 100;
			if (healthbarstyle == 0)
			{
 				ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(pos.x - entity_half_w - 5, pos.y), ImVec2(pos.x - entity_half_w - 1, pos.y - Entity_h), ImColor(30, 30, 30));
				ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(pos.x - entity_half_w - 4, pos.y), ImVec2(pos.x - entity_half_w - 2, pos.y - (Health * Entity_h) / 100), ImColor(0, 170, 0));
			}
			if (healthbarstyle == 1)
			{
				float health_height = health_ratio * (Entity_w + 22);
				DrawFilledRectImGui(pos.x - entity_half_w - 11, head.y - 4, Entity_w + 22, 4, ImColor(30, 30, 30));
				DrawFilledRectImGui(pos.x - entity_half_w - 11, head.y - 4, health_height, 4, ImColor(0, 170, 0));
			}
			if (healthbarstyle == 2)
			{
				float health_height = health_ratio * (Entity_w + 22);
				DrawFilledRectImGui(pos.x - entity_half_w - 11, pos.y + 2, Entity_w + 22, 4, ImColor(30, 30, 30));
				DrawFilledRectImGui(pos.x - entity_half_w - 11, pos.y + 2, health_height, 4, ImColor(0, 170, 0));
			}
		}

		if (TeamIgnore && pPlayerController->Teamnum() == localteam)
			continue;

		if (Aimbot && GetAsyncKeyState(AimbotButon) && (AimFov(aim) < AimbotValue))
		{
			mouse_event(MOUSEEVENTF_MOVE, (aim.x - (ScreenCenterX)) / AimbotSmooth, (aim.y - (ScreenCenterY)) / AimbotSmooth, 0, 0);
		}
	}

	if (Aimbot)
		ImGui::GetOverlayDrawList()->AddCircle(ImVec2(ScreenCenterX, ScreenCenterY), AimbotValue, IM_COL32_WHITE, 10000, 1);

	if (GetAsyncKeyState(VK_INSERT) & 1) { showmenu = !showmenu; }
	if (showmenu) { RenderUI(); }


	ImGui::EndFrame();
	p_Device->SetRenderState(D3DRS_ZENABLE, false);
	p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (p_Device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		p_Device->EndScene();
	}
	HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);
	if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		p_Device->Reset(&d3dpp);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}
void setupoverlay()
{
	randomclassname2 = _xor_("Shoven").c_str();
	HWND Entryhwnd = NULL;
	Entryhwnd = get_process_wnd(r_pid);
	setup_window();
	init_wndparams(MyWnd);
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));
	GetWindowRect(hWnd, &rect);
	while (Message.message != WM_QUIT)
	{
		RECT desktop;
		const HWND hDesktop = FindWindow(NULL, "Counter-Strike 2");
		GetWindowRect(hDesktop, &desktop);
		ScreenCenterX = desktop.right / 2;
		ScreenCenterY = desktop.bottom / 2;
		screensize = { (float)desktop.right,(float)desktop.bottom };

		if (!FindWindow(NULL, "Counter-Strike 2") || GetAsyncKeyState(VK_END)) exit(0);
		HWND hwnd_active = GetForegroundWindow();
		if (hwnd_active == GameWnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(MyWnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		GameWnd = get_process_wnd(r_pid);
		if (GameWnd) {
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetWindowRect(GameWnd, &GameRect);
			MoveWindow(MyWnd, GameRect.left, GameRect.top, rect.right, rect.bottom, true);
		}
		if (PeekMessage(&Message, MyWnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(GameWnd, &rc);
		ClientToScreen(GameWnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = GameWnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;
		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else io.MouseDown[0] = false;

		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom) {

			old_rc = rc;

			rect.right = rc.right;
			rect.bottom = rc.bottom;

			p_Params.BackBufferWidth = rc.right;
			p_Params.BackBufferHeight = rc.bottom;
			SetWindowPos(MyWnd, (HWND)0, xy.x, xy.y, rc.right, rc.bottom, SWP_NOREDRAW);
			p_Device->Reset(&p_Params);
		}

		render();
		//Sleep(1);
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	cleanup_d3d();
	DestroyWindow(MyWnd);
}
 

int main()
{
    initdrv();
    Sleep(1);
    r_socket = connect();
    if (r_socket == INVALID_SOCKET) {
        BypassLoader();
    }
    else {
        r_pid = process_id(_xor_("cs2.exe").c_str());
        client = dll_oku(r_socket, r_pid, 0);
        printf(_xor_("[+] client addr founded: %u\n").c_str(), client);
    }

	FreeConsole();
	setupoverlay();
}
