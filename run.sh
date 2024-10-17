#!/bin/bash

function display
{
    # Get the OS type
    OS=$(uname)

    # Path to the image (modify as needed)
    IMAGE_PATH="dfa.png"

    # Check which OS is being used
    if [[ "$OS" == "Darwin" ]]; then
        # macOS
        echo "macOS detected. Opening with Preview..."
        open "$IMAGE_PATH"
        
    elif [[ "$OS" == "Linux" ]]; then
        # Linux
        if command -v feh >/dev/null 2>&1; then
            # If 'feh' is installed
            echo "Linux detected. Opening with feh..."
            feh "$IMAGE_PATH"
        elif command -v xdg-open >/dev/null 2>&1; then
            # Default Linux image viewer
            echo "Linux detected. Opening with xdg-open..."
            xdg-open "$IMAGE_PATH"
        else
            echo "No suitable image viewer found. Please install 'feh' or use 'xdg-open'."
        fi

    elif [[ "$OS" == "MINGW64_NT"* || "$OS" == "CYGWIN_NT"* ]]; then
        # Windows (via Git Bash or Cygwin)
        echo "Windows detected. Opening with default image viewer..."
        explorer "$IMAGE_PATH"
        
    elif [[ "$OS" == "WSL" ]]; then
        # Windows Subsystem for Linux
        echo "WSL detected. Opening with default Windows viewer..."
        powershell.exe Start-Process "$IMAGE_PATH"
        
    else
        echo "Unsupported OS: $OS"
    fi
}

# Generate parser
if [ -e ./graphGen ]; then
  echo "graphGen already exists."
else
  cd Parser
  make fresh
  cd ..
  mv Parser/graphGen .
  clear
  echo "graphGen newly created using flex-bison"
fi

# Get graph output
echo "Generating graph output"
./graphGen < inp.txt > out.txt
echo "Graph output genearted"

# Fetch svg and convert to png
echo "Generating png from graph"
python fetch_svg.py 

# Open png
display
