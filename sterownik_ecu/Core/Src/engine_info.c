/*
 * engine_info.c
 *
 *  Created on: Aug 5, 2026
 *      Author: admin
 */
#include <engine_info.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
volatile uint16_t adc_buffer[ADC_CHANNELS];
volatile uint16_t rpm = 0;					//rpm

volatile uint16_t ignition_time = 0;		//degree
volatile uint32_t dwell_time = 0;

volatile uint16_t map = 0;					//bar
volatile uint16_t maf = 0;					//g/s
volatile uint16_t lambda = 0;				//lambda
volatile uint16_t voltage = 0;				//100 = 1v
volatile uint16_t accelerator = 0;			//%
volatile int16_t egt = 0;					//C
volatile int16_t iat = 0;					//C
volatile int16_t coolant_temp_engine = 0;	//C
volatile int16_t coolant_temp_cooler = 0;	//C
volatile uint16_t rpm_regulator = 0;		//%

volatile uint32_t full_tooth_time = 0;		//us

volatile uint16_t target_lambda = 0;
volatile uint16_t target_rpm = 0;
volatile uint16_t target_rpm_regulator = 0;


const uint16_t dwell_rpm_axis[6] =
{
     0, 500, 1000, 2000, 3000, 5000
};
const uint16_t dwell_voltage_axis[6] =
{
     600,
	 800,
	 1000,
	 1200,
	 1400,
	 1600

};
const uint16_t dwell_map[6][6] =
{
    //   6.0   8.0  10.0  12.0  14.0 16.0 V

    {4500,4000,3250,2900,2700,2600}, //    0 rpm
    {4000,3500,3000,2750,2550,2500}, //  500
    {3700,3200,2900,2575,2500,2450}, // 1000
    {3600,3100,2800,2475,2400,2350}, // 2000
    {3500,3000,2700,2375,2250,2200}, // 3000
    {3500,3000,2700,2375,2250,2200} // 5000

};

const uint16_t iat_resistance[] =
{
    45313,
    26114,
    15462,
    9397,
    5896,
    3792,
    2500,
    1707,
    1175,
    834,
    596,
    436,
    323,
    243,
    187,
    144,
    113,
    89
};

const int16_t iat_temperature[] =
{
    -40,
    -30,
    -20,
    -10,
    0,
    10,
    20,
    30,
    40,
    50,
    60,
    70,
    80,
    90,
    100,
    110,
    120,
    130
};

static uint8_t find_index(const uint16_t *axis,
                          uint8_t size,
                          uint16_t value)
{
    if(value <= axis[0])
        return 0;

    if(value >= axis[size - 1])
        return size - 2;

    for(uint8_t i = 0; i < size - 1; i++)
    {
        if(value < axis[i + 1])
            return i;
    }

    return size - 2;
}
static uint8_t find_index_desc(const uint16_t *axis,
                               uint8_t size,
                               uint16_t value)
{
    if(value >= axis[0])
        return 0;

    if(value <= axis[size - 1])
        return size - 2;

    for(uint8_t i = 0; i < size - 1; i++)
    {
        if(value > axis[i + 1])
            return i;
    }

    return size - 2;
}

static int16_t interpolate(uint16_t x0,
                           uint16_t x1,
                           int16_t y0,
                           int16_t y1,
                           uint16_t x)
{
    if(x1 <= x0)
        return y0;

    if(x <= x0)
        return y0;

    if(x >= x1)
        return y1;

    int32_t dy = (int32_t)y1 - (int32_t)y0;

    return (int16_t)(y0 + (dy * (int32_t)(x - x0)) / (int32_t)(x1 - x0));
}
static int16_t interpolate_desc(uint16_t x0,
                                uint16_t x1,
                                int16_t y0,
                                int16_t y1,
                                uint16_t x)
{
    if(x0 <= x1)
        return y0;

    if(x >= x0)
        return y0;

    if(x <= x1)
        return y1;

    int32_t dy = (int32_t)y1 - y0;

    return y0 + (dy * (int32_t)(x0 - x)) / (int32_t)(x0 - x1);
}
uint16_t table2d_lookup(const uint16_t *x_axis,
                        uint8_t x_size,
                        const uint16_t *y_axis,
                        uint8_t y_size,
                        const uint16_t *table,
                        uint16_t x,
                        uint16_t y)
{
    uint8_t xi = find_index(x_axis, x_size, x);
    uint8_t yi = find_index(y_axis, y_size, y);

    uint16_t x0 = x_axis[xi];
    uint16_t x1 = x_axis[xi + 1];

    uint16_t y0 = y_axis[yi];
    uint16_t y1 = y_axis[yi + 1];

    uint16_t q11 = table[xi * y_size + yi];
    uint16_t q12 = table[xi * y_size + yi + 1];
    uint16_t q21 = table[(xi + 1) * y_size + yi];
    uint16_t q22 = table[(xi + 1) * y_size + yi + 1];

    uint16_t r1 = interpolate(y0, y1, q11, q12, y);
    uint16_t r2 = interpolate(y0, y1, q21, q22, y);

    return interpolate(x0, x1, r1, r2, x);
}
void voltage_update(void)
{
	voltage = (uint16_t)(((uint32_t)adc_buffer[0] * 1968U) / 4095U);
}
void map_update(void){

	map = (uint16_t)(200U + (((((uint32_t)adc_buffer[1] * 4980U) / 4095U) - 400U) * 2800U) / 4248U);

}
void iat_update(){
	uint16_t iat_resistance_measured =
	    (uint32_t)(2470U * adc_buffer[5]) / (4095U - adc_buffer[5]);

	if(adc_buffer[5] >= 4095)
	{
	    iat = -40;
	    return;
	}

	uint8_t i = find_index_desc(
	    iat_resistance,
	    ARRAY_SIZE(iat_resistance),
	    iat_resistance_measured);

	iat = interpolate_desc(
	        iat_resistance[i],
	        iat_resistance[i + 1],
	        iat_temperature[i],
	        iat_temperature[i + 1],
	        iat_resistance_measured);
}
void engine_update(){
	rpm = 1000000 / full_tooth_time;


	voltage_update();
	dwell_update();
	map_update();
	iat_update();
}

void dwell_update(void)
{

    dwell_time = table2d_lookup(dwell_rpm_axis,
                                6,
                                dwell_voltage_axis,
                                6,
                                &dwell_map[0][0],
                                rpm,
                                voltage);
}

void maf_update(){
	maf = 0;
}




