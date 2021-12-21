#include "imports/syscall/syscall.h"
#include "imports/lazy_importer.h"
#include "utils/globals.h"
#include "renderer/renderer.h"
#include "utils/utils.h"
#include <dxgi1_4.h>
#include "game/game.h"

extern "C" { unsigned int _fltused = 1; }

struct __declspec(uuid("189819f1-1db6-4b57-be54-1821339b85f7")) ID3D12Device;

__declspec(dllexport) uintptr_t present_orignal_address = 0xffffffffffff;

bool init = false;
extern "C" long __declspec(dllexport) hook_main(IDXGISwapChain3* swapchain, UINT sync, UINT flags) {
	if (!init) {
		utils::log_init("D:\\overflow.log");
		utils::log("starting");

		globals::window = game::get_window();
		if (!globals::window) {
			utils::log("invalid window");
		}

		utils::log("window", globals::window);

		if (!SUCCEEDED((globals::swapchain = swapchain)->GetDevice(__uuidof(ID3D12Device), (void**)&globals::d3d_device))) {
			utils::log("failed to get d3d device");
        }

		renderer::init();

		utils::log("intialzied");
		init = true;
	}
	
	renderer::begin();
	renderer::draw_rect_filled({ 200, 200 }, { 200 + 40, 200 + 100 }, { 255, 0, 0, 255 });
	renderer::draw_text("overflow", { 15, 10 }, 18, { 255, 0, 0, 255 });
	renderer::end();

	if (syscall<short>(0x1044, 0x23) & 1) { // VK_END
		uintptr_t relative_address{};
		auto intruction_address = utils::pattern_scan((uintptr_t)LI_MODULE("DiscordHook64.dll").safe_cached(), "FF 15 ?? ?? ?? ?? 89 C6 48 8D ?? ?? ?? E8 ?? ?? ?? ?? 48 8B ?? ?? ?? 48 31 ?? E8 ?? ?? ?? ?? 89 F0 48 83 C4 ?? 5B 5F 5E C3");
		memcpy(&relative_address, (void*)(intruction_address + 0x2), 0x4);
		*(uintptr_t*)(intruction_address + 0x6 + relative_address) = present_orignal_address;
	}

	using original = long(*)(IDXGISwapChain3* swapchain, UINT sync, UINT flags);
	return ((original)present_orignal_address)(swapchain, sync, flags);
}
