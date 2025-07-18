# X11 Magnification Tool

Xmagnify is a real-time screen magnification tool for X11 systems that
provides a zoomed view of the area around your mouse cursor.

## Features

| Feature | Description |
|---------|-------------|
| **Real-time zoom** | Live magnification of screen content around cursor |
| **Configurable zoom level** | Adjustable magnification factor via command line |
| **Customizable window size** | Configurable zoom window dimensions |
| **Keyboard controls** | Press `q`, `Q`, or `Escape` to quit |

## How to use

> [!IMPORTANT]
> Make sure you have the required dependencies installed before
> building the project.

First, ensure you have the necessary development libraries installed:

```sh
# On Debian/Ubuntu
sudo apt-get install libx11-dev libxfixes-dev libxrender-dev

# On Fedora/RHEL
sudo dnf install libX11-devel libXfixes-devel libXrender-devel

# On Arch Linux
sudo pacman -S libx11 libxfixes libxrender
```

Build the project:

```sh
make
sudo make install
```

Run the magnification tool:

```sh
xmagnify
```

### Command line options

The tool supports several command line options for customization:

```sh
xmagnify [OPTIONS]

Options:
  -z, --zoom LEVEL    Zoom level (default: 2)
  -s, --size SIZE     Window size in pixels (default: 600)
  -h, --help          Show help message
  -q, --quit          Quit the application
```

### Examples

Basic usage with default settings:
```sh
xmagnify
```

Custom zoom level and window size:
```sh
xmagnify --zoom 3 --size 600
```

Short option syntax:
```sh
xmagnify -z 4 -s 500
```

## Building from source

The project uses a simple Makefile for building. Available targets:

```sh
make                  # Build the executable (default)
make clean            # Remove build artifacts
sudo make install     # Install to /usr/local/bin/
sudo make uninstall   # Remove from /usr/local/bin/
```

### Debugging

Enable verbose output by modifying the source code or using
debugging tools like `gdb`:

```sh
gdb ./xmagnify
```

## License

[makext](https://github.com/mitjafelicijan/xmagnfy) was written by [Mitja
Felicijan](https://mitjafelicijan.com) and is released under the BSD
two-clause license, see the LICENSE file for more information.