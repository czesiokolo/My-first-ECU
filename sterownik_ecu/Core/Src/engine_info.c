/*
 * engine_info.c
 *
 *  Created on: Aug 5, 2026
 *      Author: admin
 */
#include <engine_info.h>

volatile uint16_t rpm = 0;					//rpm

volatile uint16_t ignition_time = 0;		//degree
volatile uint32_t dwell_time = 2000;

volatile uint16_t map = 0;					//bar
volatile uint16_t maf = 0;					//g/s
volatile uint16_t lambda = 0;				//lambda
volatile uint16_t voltage = 1200;			//100 = 1v
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

    for(uint8_t i = 0; i < size - 1; i++)
    {
        if(value < axis[i + 1])
            return i;
    }

    return size - 1;
}


void engine_update(){
	rpm = 1000000 / full_tooth_time;
	dwell_update();
}
void map_update(){
	map = 0;
}
void maf_update(){
	maf = 0;
}
void dwell_update(void)
{
    uint8_t rpm_idx =
        find_index(dwell_rpm_axis, 8, rpm);

    uint8_t volt_idx =
        find_index(dwell_voltage_axis, 8, voltage);

    dwell_time = dwell_map[rpm_idx][volt_idx];
}




