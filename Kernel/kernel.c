#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <drivers/videoDriver.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const userCodeModuleAddress = (void*)0xA00000;
static void * const userDataModuleAddress = (void*)0xB00000;

typedef int (*EntryPoint)();

void load_idt();

void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	void * moduleAddresses[] = {
		userCodeModuleAddress,
		userDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

void start_shell() {
    ((EntryPoint)userCodeModuleAddress)();
}

int main()
{	
	load_idt();
    set_font_size(2);
    clear_video_text_buffer();
    write_to_video_text_buffer("Kernel initialized\nRunning user code...\n\n", 41, HEX_GRAY);

    start_shell();

	while(1);

	return 0;
}
