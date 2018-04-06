#include"LTKlib.h"

int main(int argc, char *argv[]) {
	LTKvulkan *vulkan = LTKVlk(0, 0, 0, 0, 0, 0, 0, 0);
	LTKgpdev *videocard;

	if(!vulkan) {
		return 0;
	}

	LTKVulkanInstanceState(vulkan);

	if(!(videocard = LTKGpd(vulkan, 0, 0))) {
		return 0;
	}

	printf("video created -------\n");
	LTKVulkanInstanceState(vulkan);
	LTKFre(&videocard->_obj);
	printf("video destroyed -------\n");
	LTKVulkanInstanceState(vulkan);

	LTKFre(0);

    return 0;
}
