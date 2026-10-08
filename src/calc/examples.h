/* Non-interactive input/output examples shown by each GUI operation.
 * Keep these synchronized with the five consecutive function contexts:
 * Simplify, Evaluate, Expand, Derivative, Convert to Radicals.
 * Examples do not modify any calculator variables.
 */
#ifndef PCAS_GUI_EXAMPLES_H
#define PCAS_GUI_EXAMPLES_H

typedef struct {
    const char *title;
    const char *input;
    const char *output;
    const char *note;
} pcas_example_t;

#define PCAS_GUI_EXAMPLE_COUNT 5

static const pcas_example_t pcas_gui_examples[PCAS_GUI_EXAMPLE_COUNT] = {
    {"化简",   "X+X",        "2X",             "Combine like terms"},
    {"求值",   "2^3",        "8",              "Evaluate constants"},
    {"展开",   "(X+1)^2",    "X^2+2X+1",       "Expand expressions"},
    {"求导",   "X^2",        "2X",             "Derivative w.r.t. X"},
    {"转根式", "X^(-5/3)",   "1/(3rootX)^5",   "3rootX = cube root X"}
};

static const pcas_example_t *pcas_gui_example(unsigned index) {
    return index < PCAS_GUI_EXAMPLE_COUNT ? &pcas_gui_examples[index] : 0;
}

#endif
