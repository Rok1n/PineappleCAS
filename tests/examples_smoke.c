/* Validate all four GUI examples and their read-only catalog. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/calc/examples.h"

int main(void) {
    const pcas_example_t *item;
    unsigned i;

    assert(PCAS_GUI_EXAMPLE_COUNT == 4);
    for(i = 0; i < PCAS_GUI_EXAMPLE_COUNT; ++i) {
        item = pcas_gui_example(i);
        assert(item != 0);
        assert(item->title[0] != '\0');
        assert(item->input[0] != '\0');
        assert(item->output[0] != '\0');
        assert(item->note[0] != '\0');
        /* On-screen example field is less than 160px in the 8px font. */
        assert(strlen(item->input) < 27);
        assert(strlen(item->output) < 27);
    }
    assert(pcas_gui_example(PCAS_GUI_EXAMPLE_COUNT) == 0);
    assert(strcmp(pcas_gui_example(0)->input, "X+X") == 0);
    assert(strcmp(pcas_gui_example(0)->output, "2X") == 0);
    assert(strcmp(pcas_gui_example(1)->input, "2^3") == 0);
    assert(strcmp(pcas_gui_example(1)->output, "8") == 0);
    assert(strcmp(pcas_gui_example(2)->input, "(X+1)^2") == 0);
    assert(strcmp(pcas_gui_example(2)->output, "X^2+2X+1") == 0);
    assert(strcmp(pcas_gui_example(3)->input, "X^2") == 0);
    assert(strcmp(pcas_gui_example(3)->output, "2X") == 0);
    puts("PASS: example catalogue and display limits");
    return 0;
}
