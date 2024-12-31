#include "szpch.h"

#include "Application.h"

int _tmain() {
    SerengetiZoo::Application* app = SerengetiZoo::CreateApplication();

    app->Run();
    
    delete app;

    return 0;
}
