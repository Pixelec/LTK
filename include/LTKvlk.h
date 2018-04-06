#ifndef __LTKVLK_H__
#define __LTKVLK_H__

struct LTKgpdev_s;
struct LTKvkque_s;

/* Vulkan instance structure */
typedef struct LTKvulkan_s {
	LTKobj _obj;

	struct LTKgpdev_s *_slv; /* List of loaded vulkan compatible devices */

	VkInstance _instance; /* Vulkan instance */
	VkInstanceCreateInfo _inst_info; /* Vulkan instance infos */
	VkApplicationInfo _app_info; /* App infos */

	uint32_t _dev_cnt;
	struct {
		VkPhysicalDevice _phys_dev; /* Vulkan physical device handler */
		VkPhysicalDeviceProperties _phys_dev_prop; /* Physical proprieties povided by the device */

		uint32_t _que_blk_cnt; /* Number of queue block available for this physical device */
		struct {
			uint32_t _que_abl; /* Number of queues available in this block */
			VkQueueFamilyProperties _que_fam_prop; /* Properties of queue block */
		} *_que_blk;
	} *_dev;
} LTKvulkan;

/* GraPhic/General_Purpose device structure */
typedef struct LTKgpdev_s {
	LTKobj _obj;

	struct LTKvulkan_s *_mtr;
	struct LTKgpdev_s *_prv;
	struct LTKgpdev_s *_nxt;
	struct LTKvkque_s *_slv;

	/* Physical device index in _mtr->_dev[] */
	uint32_t _idx;

	/* Soft device structure and handler */
	VkDevice _dev; /* Vulkan device handler */
	VkDeviceCreateInfo *_dev_crt_info;
} LTKgpdev;

/* Queue and command pool managment structure */
typedef struct LTKvkque_s {
	LTKobj _obj;

	struct LTKgpdev_s *_mtr;
	struct LTKvkque_s *_prv;
	struct LTKvkque_s *_nxt;

	/* Queue family index in _mtr->_dev_crt_info.pQueueCreateInfos[] */
	uint32_t _blk_idx;
	uint32_t _idx;

	VkQueue _que; /* Vulkan queue handler */
	VkQueueFlags _que_flgs; /* Vulkan queue family flags */
	float _que_prior;
	/* Copied from _mtr->_mtr->_dev[_mtr->_idx]._que_blk[_mtr->_dev_crt_info.pQueueCreateInfos[_blk_idx]
					.queueFamilyIndex]._que_fam_prop.queueFlags */

	/* Command buffers are owned by LTKdraw objects */
	VkCommandPool _pool;
} LTKvkque;

LTKctr *LTKVlkCtr();
LTKvulkan *LTKVlk(const char *, uint16_t, uint16_t, uint16_t,
					const char *, uint16_t, uint16_t, uint16_t);

uint32_t *LTKgetVulkanAvailableQueueArrayFromPhysicalDeviceID(LTKvulkan *, uint32_t);
char *LTKgetVulkanPhysicalDeviceNameFromID(LTKvulkan *, uint32_t);
uint32_t LTKgetVulkanPhysicalDeviceCount(LTKvulkan *);
void LTKVulkanInstanceState(LTKvulkan *);
uint32_t LTKgetVulkanPhysicalDeviceQueuePropertyCount(LTKvulkan *, uint32_t);
VkDeviceCreateInfo *LTKVulkanAllocateDeviceInfos(LTKvulkan *, uint32_t, ...);

LTKctr *LTKGpdCtr();
LTKgpdev *LTKGpd(LTKvulkan *, uint32_t, VkDeviceCreateInfo *);

uint32_t LTKgetVulkanDeviceQueueCountFromBlockID(LTKgpdev *, uint32_t);
VkQueueFlags LTKgetVulkanDeviceQueueFlagsFromBlockID(LTKgpdev *, uint32_t);
double LTKgetVulkanDeviceQueuePriorityFromBlockID(LTKgpdev *, uint32_t);
uint32_t LTKgetVulkanFirstFreeQueueCommandPool(LTKgpdev *, VkQueueFlags *, double, uint32_t *);

LTKctr *LTKVqeCtr();
LTKvkque *LTKVqe(LTKgpdev *, VkQueueFlags, double);

#endif /* __LTKVLK_H__ */
