

#include <scheduler.h>
#include <stdbool.h>
#include <ignition.h>

#define EVENT_COUNT             32
#define OC_MIN_LEAD_US          5U

extern TIM_HandleTypeDef htim5;



typedef struct
{
    uint32_t deadline;
    event_type_t type;
    uint8_t channel;
    uint8_t active;
} scheduled_event_t;


static scheduled_event_t events[EVENT_COUNT] = {0};

static inline bool time_before(uint32_t a, uint32_t b)
{
    return (int32_t)(a - b) < 0;
}

static inline bool time_reached(uint32_t now, uint32_t deadline)
{
    return (int32_t)(now - deadline) >= 0;
}


static int find_next_due_event(uint32_t now)
{
    int best = -1;

    for (int i = 0; i < EVENT_COUNT; i++)
    {
        if (!events[i].active)
            continue;

        if (!time_reached(now, events[i].deadline))
            continue;

        if (best < 0 ||
            time_before(events[i].deadline,
                        events[best].deadline))
        {
            best = i;
        }
    }

    return best;
}

static void arm_next_event(void)
{
    uint32_t now = TIM5->CNT;
    int best = -1;

    for (int i = 0; i < EVENT_COUNT; i++)
    {
        if (!events[i].active)
            continue;

        if (best < 0 ||
            time_before(events[i].deadline,
                        events[best].deadline))
        {
            best = i;
        }
    }

    // Brak aktywnych zdarzeń.
    if (best < 0)
    {
        __HAL_TIM_DISABLE_IT(&htim5, TIM_IT_CC1);
        return;
    }

    uint32_t deadline = events[best].deadline;

    // Jeśli termin już nadszedł lub jest zbyt blisko,
    // ustaw krótkie wyprzedzenie, aby nie przegapić OC.
    if (time_reached(now, deadline) ||
        (uint32_t)(deadline - now) < OC_MIN_LEAD_US)
    {
        deadline = now + OC_MIN_LEAD_US;
    }

    // Wyłącz przerwanie na czas przeprogramowania kanału.
    __HAL_TIM_DISABLE_IT(&htim5, TIM_IT_CC1);

    __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, deadline);

    // Usuń ewentualną starą flagę porównania.
    __HAL_TIM_CLEAR_FLAG(&htim5, TIM_FLAG_CC1);

    // Sprawdź, czy termin nie minął podczas konfiguracji.
    now = TIM5->CNT;

    if (time_reached(now, deadline))
    {
        // Termin został przegapiony podczas uzbrajania.
        // Zaplanuj krótkie wyprzedzenie od aktualnego czasu.
        deadline = now + OC_MIN_LEAD_US;

        __HAL_TIM_SET_COMPARE(
            &htim5,
            TIM_CHANNEL_1,
            deadline
        );

        __HAL_TIM_CLEAR_FLAG(&htim5, TIM_FLAG_CC1);
    }

    __HAL_TIM_ENABLE_IT(&htim5, TIM_IT_CC1);
}


bool events_add_or_update(event_type_t type,
                          uint8_t channel,
                          uint32_t deadline)
{
    int free_index = -1;

    // Najpierw sprawdź, czy takie zdarzenie już istnieje
    for (uint8_t i = 0; i < EVENT_COUNT; i++)
    {
        if (events[i].active)
        {
            if (events[i].type == type &&
                events[i].channel == channel)
            {
                // Zaktualizuj termin istniejącego zdarzenia
                events[i].deadline = deadline;

                arm_next_event();
                return true;
            }
        }
        else if (free_index < 0)
        {
            // Zapamiętaj pierwszy wolny wpis
            free_index = i;
        }
    }

    // Nie znaleziono istniejącego zdarzenia — dodaj nowe
    if (free_index >= 0)
    {
        events[free_index].deadline = deadline;
        events[free_index].type = type;
        events[free_index].channel = channel;
        events[free_index].active = 1;

        arm_next_event();
        return true;
    }

    // Tablica jest pełna
    return false;
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance != TIM5 ||
        htim->Channel != HAL_TIM_ACTIVE_CHANNEL_1)
    {
        return;
    }

    uint32_t now = __HAL_TIM_GET_COUNTER(htim);

    // Obsłuż wszystkie zdarzenia, których termin już nadszedł.
    while (1)
    {
        int index = find_next_due_event(now);

        if (index < 0)
            break;

        scheduled_event_t event = events[index];

        // Usuń zdarzenie przed wykonaniem akcji.
        events[index].active = 0;

        switch (event.type)
        {
            case EVENT_IGNITION_DWELL_ON:
                ignition_coil_on(event.channel+1);
                break;

            case EVENT_IGNITION_SPARK_OFF:
                ignition_coil_off(event.channel+1);
                break;

            case EVENT_INJECTOR_ON:
                //injector_on(event.channel);
                break;

            case EVENT_INJECTOR_OFF:
                //injector_off(event.channel);
                break;

            default:
                break;
        }

        // Odśwież czas, bo obsługa zdarzenia zajmuje czas.
        now = __HAL_TIM_GET_COUNTER(htim);
    }

    // Uzbrój następne zdarzenie.
    arm_next_event();
}
