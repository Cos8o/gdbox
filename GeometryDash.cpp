#include <Windows.h>

#include "GeometryDash.hpp"

#include <mutex>
#include <vector>
#include <functional>

using namespace GeometryDash;

typedef void* (__fastcall* create_T)(
	void* protocol,
	const char* title,
	const char* button1,
	const char* button2,
	float width,
	unsigned long smth,
	unsigned long smth2,
	std::string text);

typedef void(__thiscall* show_T)(void*);

//Globals

DWORD updateAddress = 0x0;
DWORD menuAddress = 0x0;

static create_T flalertlayer_create = nullptr;
static show_T flalertlayer_show = nullptr;

static std::mutex mtx;
static std::vector<std::function<void()>> funcs;

//Helper

void msgboxHelper(
	std::string const& title,
	std::string const& text,
	std::string const& button1 = "Ok",
	std::string const& button2 = "")
{
	auto box = flalertlayer_create(
		nullptr,
		title.c_str(),
		button1.size() ? button1.c_str() : "Ok",
		button2.size() ? button2.c_str() : nullptr,
		300.0,
		0,
		0,
		text);

	__asm
	{
		add esp, 44
	}

	flalertlayer_show(box);
}

//Engine

__declspec(naked) void __fastcall updateTrampoline(void* pthis, void* edx, float f)
{
	__asm
	{
		push ebp
		mov ebp, esp
		push esi
		push edi

		mov edi, [updateAddress]
		add edi, 5
		jmp edi
	}
}

void __fastcall updateCallback(void* pthis, void* edx, float f)
{
	updateTrampoline(pthis, edx, f);

	mtx.lock();
	auto calls = std::move(funcs);
	mtx.unlock();

	for (auto const& f : calls) f();
}

//GeometryDash

bool GeometryDash::init(void* addr)
{
	DWORD base = reinterpret_cast<DWORD>(GetModuleHandleA(NULL));
	auto cocos = GetModuleHandleA("libcocos2d.dll");

	flalertlayer_create = reinterpret_cast<create_T>(base + 0x227E0);
	flalertlayer_show = reinterpret_cast<show_T>(base + 0x23560);

	menuAddress = base + 0x1907B0;

	updateAddress = reinterpret_cast<DWORD>(
		GetProcAddress(cocos, "?update@CCScheduler@cocos2d@@UAEXM@Z"));

	//Hook1
	DWORD offset = reinterpret_cast<DWORD>(&updateCallback) - updateAddress - 5;
	DWORD tmp;

	VirtualProtect(
		reinterpret_cast<LPVOID>(updateAddress),
		0x1000,
		PAGE_EXECUTE_READWRITE,
		&tmp);

	*reinterpret_cast<byte*>(updateAddress) = 0xE9;
	memcpy(reinterpret_cast<void*>(updateAddress + 1), &offset, sizeof(DWORD));

	//Hook2
	offset = reinterpret_cast<DWORD>(addr) - menuAddress - 5;

	VirtualProtect(
		reinterpret_cast<LPVOID>(menuAddress),
		0x1000,
		PAGE_EXECUTE_READWRITE,
		&tmp);
	*reinterpret_cast<byte*>(menuAddress) = 0xE9;
	memcpy(reinterpret_cast<void*>(menuAddress + 1), &offset, sizeof(DWORD));

	return true;
}

bool GeometryDash::showMessageBox(
	std::string const& title,
	std::string const& text,
	std::string const& button1,
	std::string const& button2)
{
	std::lock_guard<std::mutex> lock(mtx);

	if (flalertlayer_create && flalertlayer_show)
	{
		funcs.push_back([title, text, button1, button2]()
			{
				msgboxHelper(title, text, button1, button2);
			});

		return true;
	}

	return false;
}

__declspec(naked) void GeometryDash::showMenu(void* pthis)
{
	__asm
	{
		push ebp
		mov ebp, esp
		and esp, 0xFFFFFFF8

		mov eax, [menuAddress]
		add eax, 6
		jmp eax
	}
}