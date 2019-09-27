This is the great minimal window manager written by Robert Sperling but with these changes:

1. Removed unessessary code including the move and resize code that was awkward to use.
2. Added code to make a window fullsize and to tile left or tile right (win+left, win+right).
3. Added a launcher keybind to launch programs (dmenu_run).
4. Changed the other keybinds.

Default configuration:

  Win + z  cycle through available programs.
  
  Win + q  close the window under the mouse.
  
  Win + r  launch dmenu.
  
  Win + f  make window fullscreen (1920x1080).
  
  Win + Left  tile window to left side.
  
  Win + Right tile window to right side.
  
  
  To build:
  gcc -o failsafewm-mod failsafewm-mod.c -lX11
