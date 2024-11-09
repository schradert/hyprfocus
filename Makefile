SOURCE_FILES=$(wildcard src/*.cpp)

all:
	$(CXX) -shared -fPIC --no-gnu-unique $(SOURCE_FILES) -o hyprfocus.so -g `pkg-config --cflags pixman-1 libdrm hyprland pangocairo libinput libudev wayland-server xkbcommon` -std=c++2b -O2
	strip hyprfocus.so

debug:
	$(CXX) -shared -fPIC --no-gnu-unique $(SOURCE_FILES) -o hyprfocus.so -g `pkg-config --cflags pixman-1 libdrm hyprland pangocairo libinput libudev wayland-server xkbcommon` -std=c++2b -O2

clean:
	rm ./hyprfocus.so
