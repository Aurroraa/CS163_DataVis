#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "../include/States/MenuState.h"
#include "../include/App.h"
int main() {
    App app;

    app.ChangeState(new MenuState());

    app.Run();

    return 0;
}