#include "zepch.h"

#include "Helpers.h"

namespace SerengetiZoo
{
    // Adds a name back into the unique name list
    void Helpers::AddRandomName(const LPCTSTR name)
    {
        auto guard = s_cs.lock();

        for (auto& uniqueName : s_uniqueNames)
        {
            if (uniqueName == nullptr)
            {
                uniqueName = name;

                break;
            }
        }
    }

    // Get a random name from the unique name list
    LPCTSTR Helpers::GetRandomName()
    {
        auto guard = s_cs.lock();

        std::random_device generator;
        std::uniform_int_distribution range(0, static_cast<int>(s_uniqueNames.size()) - 1);

        int index;
        LPCTSTR selectedName;

        do
        {
            index = range(generator);
            selectedName = s_uniqueNames[index];
        }
        while (selectedName == nullptr);

        s_uniqueNames.at(index) = nullptr;

        return selectedName;
    }
}
