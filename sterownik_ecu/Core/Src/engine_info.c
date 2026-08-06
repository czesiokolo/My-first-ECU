/*
 * engine_info.c
 *
 *  Created on: Aug 5, 2026
 *      Author: admin
 */
#include <engine_info.h>

volatile uint16_t adc_buffer[ADC_CHANNELS];
volatile uint16_t rpm = 0;					//rpm

volatile uint16_t ignition_time = 0;		//degree
volatile uint32_t dwell_time = 0;

volatile uint16_t map = 0;					//bar
volatile uint16_t maf = 0;					//g/s
volatile uint16_t lambda = 0;				//lambda
volatile uint16_t voltage = 1200;				//100 = 1v
volatile uint16_t accelerator = 0;			//%
volatile uint16_t egt = 0;					//C
volatile uint16_t iat = 0;					//C
volatile uint16_t coolant_temp_engine = 0;	//C
volatile uint16_t coolant_temp_cooler = 0;	//C
volatile uint16_t rpm_regulator = 0;		//%

volatile uint32_t full_tooth_time = 0;		//us

volatile uint16_t target_lambda = 0;
volatile uint16_t target_rpm = 0;
volatile uint16_t target_rpm_regulator = 0;


const uint16_t dwell_rpm_axis[8] =
{
     0, 500, 1000, 2000,
  3000, 4000, 5500, 7000
};
const uint16_t dwell_voltage_axis[8] =
{
     900,
    1000,
    1100,
    1200,
    1300,
    1400,
    1450,
    1500
};
const uint16_t dwell_map[8][8] =
{
    //   9.0   10.0  11.0  12.0  13.0  14.0  14.5  15.0 V

    {5200,4700,4300,3900,3500,3200,3000,2900}, //    0 rpm
    {5100,4600,4200,3800,3400,3150,2950,2850}, //  500
    {5000,4500,4100,3700,3350,3100,2900,2800}, // 1000
    {4900,4400,4000,3600,3250,3000,2850,2750}, // 2000
    {4800,4300,3900,3500,3200,2950,2800,2700}, // 3000
    {4700,4200,3800,3450,3150,2900,2750,2650}, // 4000
    {4600,4100,3700,3400,3100,2850,2700,2600}, // 5500
    {4500,4000,3600,3300,3000,2800,2650,2550}  // 7000
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

static uint16_t interpolate(uint16_t x0,
                            uint16_t x1,
                            uint16_t y0,
                            uint16_t y1,
                            uint16_t x)
{
    if(x1 == x0)
        return y0;

    int32_t dy = (int32_t)y1 - (int32_t)y0;

    return (uint16_t)(y0 + (dy * (int32_t)(x - x0)) / (int32_t)(x1 - x0));
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
	//voltage = (uint16_t)(((uint32_t)adc_buffer[0] * 24100U) / 40950U);
}

void engine_update(){
	rpm = 1000000 / full_tooth_time;


	voltage_update();
	dwell_update();
}

void dwell_update(void)
{

    dwell_time = table2d_lookup(dwell_rpm_axis,
                                8,
                                dwell_voltage_axis,
                                8,
                                &dwell_map[0][0],
                                rpm,
                                voltage);
}
void map_update(){
	map = 0;
}
void maf_update(){
	maf = 0;
}




