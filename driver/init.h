#ifndef TEST_DRIVER_INIT_H
#define TEST_DRIVER_INIT_H

#ifdef __KERNEL__

#define					SKINK_MODULE_NAME				"skin_test_device"

#include <skink.h>

typedef struct layer_data
{
	char			name[SKINK_MAX_NAME_LENGTH+1];
	skink_sensor_size	sensors_count;
	skink_module_size	modules_count;
	skink_patch_size	patches_count;
	uint32_t		acquisition_rate;
	skink_module_id		module_data_begin;
	skink_patch_id		patch_data_begin;
	uint32_t extra;
} layer_data;

typedef struct module_data
{
	skink_sensor_size	sensors_count;
} module_data;

typedef struct patch_data
{
	skink_module_size	modules_count;
} patch_data;

int init_data(void);
void free_data(void);

#endif

#endif
