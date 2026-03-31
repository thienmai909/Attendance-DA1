#include <UiColors.hpp>

auto UiColors::titleColor()
{
    return color(Color::Cyan);
}

auto UiColors::warningColor()
{
    return color(Color::Yellow);
}

auto UiColors::errorColor()
{
    return color(Color::Red);
}

auto UiColors::okColor()
{
    return color(Color::Green);
}

auto UiColors::dimColor()
{
    return dim;
}
