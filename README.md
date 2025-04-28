# scop


1) Pre-installed GLFW
```
~ dpkg -l | grep glfw                           
ii  libglfw3:amd64                         3.3.6-1                                                                       amd64        portable library for OpenGL, window and input (x11 libraries)
ii  libglfw3-dev:amd64                     3.3.6-1                                                                       amd64        portable library for OpenGL, window and input (development files)
```


2) Pre-Installed Vulkan
```
~ vulkaninfo | grep apiVersion                  
WARNING: [Loader Message] Code 0 : terminator_CreateInstance: Failed to CreateInstance in ICD 3.  Skipping ICD.
	apiVersion        = 4206847 (1.3.255)
	apiVersion        = 4206847 (1.3.255)
```
