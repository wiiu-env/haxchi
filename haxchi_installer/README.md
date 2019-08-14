# Haxchi Installer
This installer can be used to install haxchi onto your console.
To install haxchi, you need a compatible title installed on your console.

## Building
Before you can build the haxchi installer, make sure to generate the haxchi-payloads using the payload-generator.

You also need [wut](https://github.com/devkitPro/wut/), [libiosuhax](https://github.com/wiiu-env/libiosuhax) and the following portlibs installed:
```
pacman -Syu ppc-zlib ppc-libxml2
```

Then compile the installer via 
```
make
```

## Usage
Start the resulting `haxchi_installer.rpx` using the {homebrew launcher](https://github.com/wiiu-env/homebrew_launcher)

## credit
FIX94 and orboditilt.
