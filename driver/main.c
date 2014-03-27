#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/proc_fs.h>

#include "init.h"
#include "simulated_input.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shahbaz Youssefi");

extern skink_sensor_layer_size		the_layers_count;
extern layer_data			*the_layers;
extern module_data			*the_modules;
extern patch_data			*the_patches;

static struct task_struct		*_initialization_task;

static bool				_busy						= false;
static bool				_initialized					= false;

static int				_device_id					= -1;

static int				_last_start[30]					= {0, 0, 0, 0};		// for each layer
#define					SPEED						1000

static void details(skink_sensor_layer *layer)
{
	uint32_t i;
	layer_data *layer_here = the_layers + layer->id_in_device;
	if (the_layers == NULL || the_modules == NULL || the_patches == NULL)
	{
		SKINK_LOG("details called while the device is not properly initialized");
		return;
	}
	strncpy(layer->name, layer_here->name, SKINK_MAX_NAME_LENGTH);
	layer->name[SKINK_MAX_NAME_LENGTH] = '\0';
	layer->sensors_count = layer_here->sensors_count;
	layer->modules_count = layer_here->modules_count;
	layer->patches_count = layer_here->patches_count;
	layer->acquisition_rate = layer_here->acquisition_rate;
	for (i = 0; i < layer_here->modules_count; ++i)
		layer->modules[i].sensors_count = the_modules[layer_here->module_data_begin + i].sensors_count;
	for (i = 0; i < layer_here->patches_count; ++i)
		layer->patches[i].modules_count = the_patches[layer_here->patch_data_begin + i].modules_count;
}

static void busy(bool b)
{
	_busy = b;
}

static int acquire(skink_sensor_layer *layer, skink_sensor_id *skinmap, uint8_t buffer)
{
	skink_sensor_id i;
	for (i = 0; i < layer->sensors_count; ++i) {
		layer->sensors[skinmap[i]].response[buffer] = 
      simulated_input[i];
  }
	_last_start[layer->id_in_device] += SPEED;
	return SKINK_SUCCESS;
}

static int init_thread(void *arg)
{
	skink_registration_data data;
	int ret;
	uint32_t i;
	skink_sensor_size sensors_count = 0;
	skink_module_size modules_count = 0;
	skink_patch_size  patches_count = 0;
	SKINK_LOG("Init thread: Stage 1...");
	// Stage 1: Data initialization
	if (init_data())
	{
		SKINK_LOG("Stage 1: Skin test device module stopping due to error in data initialization!");
		_initialized = true;
		do_exit(0);
		return 0;
	}
	for (i = 0; i < the_layers_count; ++i)
	{
		modules_count += the_layers[i].modules_count;
		patches_count += the_layers[i].patches_count;
		sensors_count += the_layers[i].sensors_count;
	}
	SKINK_LOG("Stage 1: Detected %u sensors in %u modules in %u patches", (unsigned int)sensors_count, (unsigned int)modules_count, (unsigned int)patches_count);
	SKINK_LOG("Init thread: Stage 1...done");
	SKINK_LOG("Init thread: Stage 2...");
	// Stage 2: registering device
	data.sensor_layers_count = the_layers_count;
	data.sensors_count = sensors_count;
	data.modules_count = modules_count;
	data.patches_count = patches_count;
	data.details_callback = details;
	data.busy_callback = busy;
	data.acquire_callback = acquire;
	data.device_name = SKINK_MODULE_NAME;
	while (true)
	{
		bool revived;
		ret = skink_device_register_nonblocking(&data, &revived);
		if (ret >= 0)
		{
			if (revived)
				SKINK_LOG("Stage 2: Revived!");
			_device_id = ret;
			break;
		}
		else if (ret == SKINK_IN_USE)
		{
			SKINK_LOG("Stage 2: Skin test device module stopping because another device with same name is in use!");
			goto exit_done;
		}
		else if (ret == SKINK_TOO_LATE)
		{
			SKINK_LOG("Stage 2: Skin test device module stopping because it is introduced to the skin kernel too late!");
			goto exit_done;
		}
		else if (ret == SKINK_BAD_ID)
		{
			SKINK_LOG("Stage 2: Skin test device module stopping because middle-ware rejected its major and minor ids!");
			goto exit_done;
		}
		else if (ret == SKINK_BAD_DATA)
		{
			SKINK_LOG("Stage 3: Skin test device module stopping because middle-ware rejected its registration data!");
			goto exit_done;
		}
		else if (ret == SKINK_TOO_EARLY)
		{
			set_current_state(TASK_INTERRUPTIBLE);
			msleep(1);
			// And retry
		}
		else
		{
			SKINK_LOG("Stage 2: Got unknown return value from the middle-ware: %d!", ret);
			goto exit_done;
		}
	}
	if (_device_id >= 0)
		if (skink_device_resume(_device_id) != SKINK_SUCCESS)
			SKINK_LOG("Stage 2: Error - Failed to resume network");
	SKINK_LOG("Init thread: Stage 2...done");
exit_done:
	_initialized = true;
	_initialization_task = NULL;
	do_exit(0);
	return 0;		// should never reach
}

int init_module(void)
{
	SKINK_LOG("Skin test device module initializing...");
	SKINK_LOG("Using skin kernel version %s", SKINK_VERSION);
	_busy = false;
	_initialized = false;
	if ((_initialization_task = kthread_run(init_thread, NULL, "skin_test_init_thread")) == ERR_PTR(-ENOMEM))
	{
		_initialization_task = NULL;
		SKINK_LOG("Error creating the initial thread!");
		SKINK_LOG("Stopping functionality!");
		_initialized = true;
		return -ENOMEM;
	}
	SKINK_LOG("Skin test device module initializing...done!");
	return 0;
}

void cleanup_module(void)
{
	SKINK_LOG("Skin test device module exiting...");
	while (!_initialized)
	{
		set_current_state(TASK_INTERRUPTIBLE);
		msleep(1);
	}
	if (_device_id >= 0)
		skink_device_pause(_device_id);
	if (_initialization_task != NULL)
		send_sig(SIGKILL, _initialization_task, 1);
	if (_busy)
		SKINK_LOG("Still in use by the middle-ware. Waiting...");
	while (_busy)
	{
		set_current_state(TASK_INTERRUPTIBLE);
		msleep(1);
	}
	free_data();
	SKINK_LOG("Skin test device module exiting...done!");
}
