#include "debugger.h"
#include "ui.h"
#include "cpu.h"

UIWindow debugger;
extern CPU cpu;

static void Debugger_print_registers() {
	UI_render_string(&debugger, "REGISTERS", 0, 3);
	UI_render_string(&debugger, "B  0x00", 0, 5);
	UI_render_string(&debugger, "C  0x00", 0, 6);
	UI_render_string(&debugger, "D  0x00", 0, 7);
	UI_render_string(&debugger, "E  0x00", 0, 8);
	UI_render_string(&debugger, "H  0x00", 0, 9);
	UI_render_string(&debugger, "L  0x00", 0, 10);
	UI_render_string(&debugger, "A  0x00", 0, 11);
	UI_render_string(&debugger, "F  0x00", 0, 12);
	UI_render_string(&debugger, "BC 0x0000", 0, 13);
	UI_render_string(&debugger, "DE 0x0000", 0, 14);
	UI_render_string(&debugger, "HL 0x0000", 0, 15);
	UI_render_string(&debugger, "AF 0x0000", 0, 16);
	UI_render_string(&debugger, "PC 0x0000", 0, 17);

	UI_render_string(&debugger, "FLAGS", 12, 3);
	UI_render_string(&debugger, "Z -", 12, 5);
	UI_render_string(&debugger, "N +", 12, 6);
	UI_render_string(&debugger, "H -", 12, 7);
	UI_render_string(&debugger, "C -", 12, 8);
}

void Debugger_update() {

}

void Debugger_draw() {
	UI_render_string(&debugger, "<CPU> MMU  GPU  APU  LOG", 0, 0);
	UI_render_string(&debugger, "-----------------------------------------------------", 0, 1);
	Debugger_print_registers();
}
