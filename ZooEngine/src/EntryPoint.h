#pragma once

#ifdef PLATFORM_WINDOWS

extern SerengetiZoo::Application* SerengetiZoo::CreateApplication(int argc, char** argv);

namespace SerengetiZoo
{
    int Main(int argc, char** argv)
    {
        auto app = CreateApplication(argc, argv);

        app->Run();

        delete app;

        return 0;
    }
}

#ifdef RELEASE

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    return SerengetiZoo::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
    return SerengetiZoo::Main(argc, argv);
}

#endif // RELEASE
#endif // PLATFORM_WINDOWS
