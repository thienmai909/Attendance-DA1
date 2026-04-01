#include <UiColors.hpp>

Element UiColors::asTitle(Element e)
{
    return e | bold | color(Color::Cyan);
}

Element UiColors::asSubtitle(Element e)
{
    return e | dim;
}

Element UiColors::asOk(Element e)
{
    return e | color(Color::Green);
}

Element UiColors::asWarn(Element e)
{
    return e | color(Color::Yellow);
}

Element UiColors::asError(Element e)
{
    return e | color(Color::Red);
}

Element UiColors::asDim(Element e)
{
    return e | dim;
}

Element UiColors::asSelected(Element e)
{
    return e | color(Color::Cyan) | bold;
}

Element UiColors::asCamThi(Element e)
{
    return e | color(Color::Red) | bold;
}
