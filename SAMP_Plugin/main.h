#pragma once

#include <stdint.h>
#include <windows.h>

#include <game/common.h>
#include <game/CPools.h>
#include <game/CMenuManager.h>

// Общие функции
namespace util {

	// Дескриптор плагина
	static HMODULE h_module;

	// Загрузить ресурс
	static bool load_resource(
		const uint32_t rId,		// [in]
		const char* rType,		// [in]
		HGLOBAL* pResource,		// [out]
		void** ppAddress,		// [out]
		uint32_t* pSize			// [out]
	) {
		HRSRC res;
		if ((res = FindResource(h_module, MAKEINTRESOURCE(rId), rType)) &&
			(*pResource = LoadResource(h_module, res)) &&
			(*ppAddress = LockResource(*pResource)) &&
			(*pSize = SizeofResource(h_module, res))
			) return true;
		return false;
	}

	// Выгрузить ресурс
	static inline void free_resource(
		HGLOBAL resource
	) {
		FreeResource(resource);
	}

	// Нажата ли клавиша
	static inline bool is_key_down(
		uint8_t key_id
	) {
		return GetKeyState(key_id) & 0x80;
	}

	// Получить статус игры
	static inline bool is_active() {
		return *(uint8_t*)(0x8D621C) && !FrontEndMenuManager.m_bMenuActive;
	}

	// Получить ширину экрана
	static inline int get_width_screen() {
		return GetSystemMetrics(SM_CXSCREEN);
	}

	// Получить высоту экрана
	static inline int get_height_screen() {
		return GetSystemMetrics(SM_CYSCREEN);
	}

}
