#include "../headers/events.h"

pd::EventGuard::EventGuard() : type(Events::NU) {}

pd::Events pd::EventGuard::get() const { return type; }

void pd::EventGuard::update(pd::Events&& event)
{
    switch(event)
    {
        // We switch event type only if 'event' is not NU
        case pd::Events::NEW_BOARD_CARD:
            [[fallthrough]];
        case pd::Events::NEW_GAME:
            this->type = event;
            break;
    }
}

//Switch event type to NU
void pd::EventGuard::reset() { this->type = pd::Events::NU; }