#pragma once

#include <iostream>
#include <optional>

namespace rib
{

class NoizyObject
{
    std::optional<int> id;

    std::ostream& log() const
    {
        std::cout << "NoizyObject";
        if (id) std::cout << "@" << *id;
        return std::cout << ": ";
    }

public:
    NoizyObject() { log() << "default ctor." << std::endl; }
    NoizyObject(int i) : id(i) { log() << "ctor." << std::endl; }
    NoizyObject(const NoizyObject& other) : id(other.id) { log() << "copy ctor." << std::endl; }
    NoizyObject(NoizyObject&& other) : id(other.id) { log() << "move ctor." << std::endl; }

    NoizyObject& operator=(const NoizyObject& other)
    {
        if (other.id)
            log() << "copy assign. id -> " << *other.id << std::endl;
        else
            log() << "copy assign. id -> none" << std::endl;
        id = other.id;
    }
    NoizyObject& operator=(NoizyObject&& other)
    {
        if (other.id)
            log() << "move assign. id -> " << *other.id << std::endl;
        else
            log() << "move assign. id -> none" << std::endl;
        id = other.id;
    }
};

} // namespace rib
