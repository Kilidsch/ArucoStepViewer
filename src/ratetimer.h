#ifndef RATETIMER_H
#define RATETIMER_H

#include <chrono>
#include <thread>

class RateTimer
{
    using clock = std::chrono::steady_clock;

  public:
    explicit RateTimer(clock::duration period) : m_last(clock::now()), m_period(period)
    {
    }

    void sleep()
    {
        auto next = m_last + m_period;
        auto now = clock::now();
        auto to_sleep = next - now;
        m_last += m_period;
        if (to_sleep > clock::duration(0))
        {
            std::this_thread::sleep_until(next);
        }
        else if (now > next + m_period)
        {
            // missed more than one period
            m_last = now + m_period;
        }
    }

  private:
    clock::time_point m_last;
    clock::duration m_period;
};

#endif // RATETIMER_H
