#ifndef EVENTS_PD
#define EVENTS_PD

namespace pd
{
    enum class Events
    {
        NU,
        NEW_BOARD_CARD,
        NEW_GAME,
        PLAYER_ELIMINATED
    };
    
    class EventGuard
    {
    private:
        pd::Events type;
    public:
        EventGuard();
        pd::Events get() const;
        void update(pd::Events&&);
        void reset();
    };
}

#endif