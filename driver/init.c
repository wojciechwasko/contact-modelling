#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#include "init.h"

skink_sensor_layer_size			the_layers_count					= 0;
layer_data				*the_layers						= NULL;
module_data				*the_modules						= NULL;
patch_data				*the_patches						= NULL;

#define					LAYERS_COUNT						1
#define					PATHCES_PER_LAYER					1
#define					MODULES_PER_PATCH					1
#define					SENSORS_PER_MODULE					72
static char				_layer_names[][SKINK_MAX_NAME_LENGTH+1]			= {"Hello"};
static int				_acquisition_rates[]					= {13};

int init_data(void)
{
	skink_module_size total_modules = 0;
	skink_patch_size  total_patches = 0;
	uint32_t i;
	// Get the sensor layers count and allocate memory for the sensors, modules and patches
	the_layers_count = LAYERS_COUNT;
	the_layers = vmalloc(the_layers_count * sizeof(*the_layers));
	if (the_layers == NULL)
	{
		SKINK_LOG("Stage 1: Could not allocate memory");
		return -1;
	}
	total_modules = 0;
	total_patches = 0;
	for (i = 0; i < the_layers_count; ++i)
	{
		strncpy(the_layers[i].name, _layer_names[i], SKINK_MAX_NAME_LENGTH);
		the_layers[i].name[SKINK_MAX_NAME_LENGTH] = '\0';
		the_layers[i].acquisition_rate = _acquisition_rates[i];
		the_layers[i].sensors_count = PATHCES_PER_LAYER * MODULES_PER_PATCH * SENSORS_PER_MODULE;
		the_layers[i].modules_count = PATHCES_PER_LAYER * MODULES_PER_PATCH;
		the_layers[i].patches_count = PATHCES_PER_LAYER;
		the_layers[i].module_data_begin = total_modules;
		the_layers[i].patch_data_begin = total_patches;
		total_modules += the_layers[i].modules_count;
		total_patches += the_layers[i].patches_count;
		the_layers[i].extra = 0;
	}
	the_modules = vmalloc(total_modules * sizeof(*the_modules));
	the_patches = vmalloc(total_patches * sizeof(*the_patches));
	if (the_modules == NULL || the_patches == NULL)
	{
		SKINK_LOG("Stage 1: Could not allocate memory");
		free_data();
		return -1;
	}
	for (i = 0; i < total_modules; ++i)
		the_modules[i].sensors_count = SENSORS_PER_MODULE;
	for (i = 0; i < total_patches; ++i)
		the_patches[i].modules_count = MODULES_PER_PATCH;
	return 0;
}

void free_data(void)
{
	if (the_layers)
		vfree(the_layers);
	if (the_modules)
		vfree(the_modules);
	if (the_patches)
		vfree(the_patches);
	the_layers = NULL;
	the_modules = NULL;
	the_patches = NULL;
	the_layers_count = 0;
}
