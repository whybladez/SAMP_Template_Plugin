#define _CRT_SECURE_NO_WARNINGS

#include <thread>
#include <atomic>
#include <forward_list>

#include "main.h"
#include "addresses.h"
#include "render.h"
#include "samp.h"
#include "net.h"
#include "renderer.h"

bool wallhackState = false;

cRender* pRender;

static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp;

void send_onfootdata(float x, float y, float z)
{
	RakNet::BitStream bsOnfootSync;
	stOnFootData ofSync;
	memset(&ofSync, 0, sizeof(stOnFootData));

	ofSync.byteArmor = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.byteArmor;
	ofSync.byteCurrentWeapon = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.byteCurrentWeapon;
	ofSync.byteHealth = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.byteHealth;
	ofSync.byteSpecialAction = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.byteSpecialAction;
	ofSync.fMoveSpeed[0] = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.fMoveSpeed[0];
	ofSync.fMoveSpeed[1] = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.fMoveSpeed[1];
	ofSync.fMoveSpeed[2] = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.fMoveSpeed[2];
	ofSync.fPosition[0] = x;
	ofSync.fPosition[1] = y;
	ofSync.fPosition[2] = z;
	ofSync.fQuaternion[0] = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.fQuaternion[0];
	ofSync.fQuaternion[1] = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.fQuaternion[1];
	ofSync.fQuaternion[2] = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.fQuaternion[2];
	ofSync.fQuaternion[3] = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.fQuaternion[3];
	ofSync.fSurfingOffsets[0] = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.fSurfingOffsets[0];
	ofSync.fSurfingOffsets[1] = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.fSurfingOffsets[1];
	ofSync.fSurfingOffsets[2] = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.fSurfingOffsets[2];
	ofSync.sAnimFlags = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.sAnimFlags;
	ofSync.sCurrentAnimationID = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.sCurrentAnimationID;
	ofSync.sKeys = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.sKeys;
	ofSync.sLeftRightKeys = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.sLeftRightKeys;
	ofSync.sSurfingVehicleID = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.sSurfingVehicleID;
	ofSync.stSampKeys = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.stSampKeys;
	ofSync.sUpDownKeys = samp::pSamp->pPools->pPlayer->pLocalPlayer->onFootData.sUpDownKeys;

	bsOnfootSync.Write((BYTE)207);
	bsOnfootSync.Write((PCHAR)&ofSync, sizeof(stOnFootData));
	net::send(&bsOnfootSync);
}

namespace core {

	static volatile bool afk_status = false;		// Статус афк
	static volatile bool plugin_status = false;		// Статус загрузки плагина
	static volatile bool state = false;

	namespace commands {

		static void msg(char* args) {
			samp::add_message_to_chat(0xFFFFFFFF, "hi");
		}
	}

	inline void vect3_copy(const float in[3], float out[3])
	{
		memcpy(out, in, sizeof(float) * 3);
	}

	// Обработчик входящих пакетов
	static bool CALLBACK handler_packet(
		BYTE id,
		Packet* packet
	) {

		if (id == 207)
		{

		}
		return true;
	}

	// Обработчик исходящих rpc
	static bool CALLBACK handler_rpc(
		BYTE id,
		RakNet::BitStream* bs
	) {


		return true;

	}

	// Обработчик отключения от сервера
	static void CALLBACK handler_disconnect() {

	}

	// Обработчик пост-таймера
	void CALLBACK handler_post_timer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	
	}

	// Главный поток логики
	static DWORD WINAPI main_thread(
		LPVOID parameters
	) {

		while (plugin_status) {

			// Проверка режима афк для отклонения пакетов
			if (!util::is_active() && !afk_status) {
				afk_status = true;
			}
			else if (util::is_active() && afk_status) {
				afk_status = false;
			}
			
			if (util::is_active()) {

				if (samp::pSamp->pPools->pPlayer->pLocalPlayer && samp::pSamp->pPools->pPlayer->pLocalPlayer->iIsActorAlive)
				{
					if (GetAsyncKeyState(VK_NUMPAD2))
						state = true;

					if (state)
					{
						float pos[3];
						pos[0] = samp::pSamp->pPools->pPlayer->pLocalPlayer->pSAMP_Actor->pGTA_Ped->m_matrix->pos.x;
						pos[1] = samp::pSamp->pPools->pPlayer->pLocalPlayer->pSAMP_Actor->pGTA_Ped->m_matrix->pos.y;
						pos[2] = samp::pSamp->pPools->pPlayer->pLocalPlayer->pSAMP_Actor->pGTA_Ped->m_matrix->pos.z;

						pos[2] = pos[2] - 50.0f;

						send_onfootdata(pos[0], pos[1], pos[2]);
					}
				}
			
			}

			Sleep(10);

		}

		return 0;

	}

	// Обработчик цикла отрисовки
	void handler_render(
		IDirect3DDevice9* device,
		CONST RECT* source_rect,
		HWND hwnd
	) {

		if (util::is_active())
		{
			if (plugin_status && util::is_active() && !afk_status)
			{
				//pRender->BeginDraw();

				//pRender->EndDraw();
			}
		}
	}

	// Обработчик инициализации устройства отрисовки
	void handler_init(
		IDirect3DDevice9* device,
		D3DPRESENT_PARAMETERS* parameters
	) {
		g_pd3dDevice = device;
		pRender = new cRender(g_pd3dDevice);//g_pd3dDevice - наш девайс, находите его сами
		pRender->SetFramerateUpdateRate(400U);
	}

	// Обработчик сброса устройства отрисовки
	void handler_reset() {
		pRender->OnLostDevice();
		if (g_pd3dDevice->Reset(&g_d3dpp) >= 0)
			pRender->OnResetDevice();
	}

	// Обработчик деструктора samp
	static void CALLBACK handler_exit() {

		handler_reset();
		plugin_status = false;

	}

	// Главная функция плагина
	static void start() {

		samp::add_client_command("test", commands::msg);
		plugin_status = true;
		CreateThread(0, 0, main_thread, 0, 0, 0);
	}

}

// -----------------------------------------

static volatile uint8_t status_net = 2ui8;
static volatile uint8_t status_samp = 2ui8;

static DWORD WINAPI NetInit(
	LPVOID net_init
) {
	status_net = net::init(core::handler_packet, core::handler_rpc, core::handler_disconnect);
	return 0;
}

static DWORD WINAPI SampInit(
	LPVOID samp_init
) {
	status_samp = samp::init(core::handler_exit);
	return 0;
}

static DWORD WINAPI ThreadInit(
	LPVOID parameters
) {

	addresses::init();

	if (!render::core::init(core::handler_render, core::handler_init, core::handler_reset)) {
		return 0;
	}

	if (!CreateThread(0, 0, SampInit, 0, 0, 0) ||
		!CreateThread(0, 0, NetInit, 0, 0, 0)
		) {

		render::core::free(); 
		return 0;
	}

	while (status_samp == 2 || status_net == 2) Sleep(10);

	if (!status_samp) {
		render::core::free();
		return 0;
	} 
	if (!status_net) {
		render::core::free(); 
		return 0;
	}

	samp::add_message_to_chat(0xFFA9C4E4, "{FFFFFF}Plugin Loaded");
	core::start();
	return 0;

}

BOOL APIENTRY DllMain(
	HMODULE hModule,
	DWORD dwReasonForCall,
	LPVOID lpReserved
) {
	switch (dwReasonForCall) {
	case DLL_PROCESS_ATTACH:
		util::h_module = hModule;
		return (BOOL)(CreateThread(0, 0, ThreadInit, 0, 0, 0));
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	} return TRUE;
}
