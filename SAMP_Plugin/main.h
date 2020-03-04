#pragma once

#include <stdint.h>
#include <windows.h>

#include <game/common.h>
#include <game/CPools.h>
#include <game/CMenuManager.h>

// ����� �������
namespace util {

	// ���������� �������
	static HMODULE h_module;

	// ��������� ������
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

	// ��������� ������
	static inline void free_resource(
		HGLOBAL resource
	) {
		FreeResource(resource);
	}

	// ������ �� �������
	static inline bool is_key_down(
		uint8_t key_id
	) {
		return GetKeyState(key_id) & 0x80;
	}

	// �������� ������ ����
	static inline bool is_active() {
		return *(uint8_t*)(0x8D621C) && !FrontEndMenuManager.m_bMenuActive;
	}

	// �������� ������ ������
	static inline int get_width_screen() {
		return GetSystemMetrics(SM_CXSCREEN);
	}

	// �������� ������ ������
	static inline int get_height_screen() {
		return GetSystemMetrics(SM_CYSCREEN);
	}

}
