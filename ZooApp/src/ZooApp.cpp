#include <ZooEngine.h>

#include "EntryPoint.h"

SerengetiZoo::Application* SerengetiZoo::CreateApplication(int argc, char** argv)
{
    ApplicationSpecification specification;

    Application* app = new Application(specification);

    return app;
}
