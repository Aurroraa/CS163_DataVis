#include "../include/App.h"
#include "../include/States/MenuState.h"

// Start the app with MenuState
int main() {
    App app;

    // Load the first screen
    app.ChangeState(new MenuState());

    // Run the infinite loop
    app.Run();

    return 0;
}