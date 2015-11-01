# Introduction #

This page details the preferred way to work on Chromium WM.



# Prerequisites #

  * Follow Chromium's [UsingGit](https://code.google.com/p/chromium/wiki/UsingGit) instructions to get a git checkout of the Chromium code.

# Checkout the chromium-wm code #

```
  cd src
  git remote add experimental https://chromium.googlesource.com/experimental/chromium/src.git
  git config --add remote.experimental.fetch '+refs/wip/chromium-wm:refs/remotes/experimental/chromium-wm'
  git fetch experimental
```

To create a local working branch:

```
  git branch mywork experimental/chromium-wm
```

# Building #

Configure Chromium to build with wm functionality:
```
  ./build/gyp_chromium -Dchromeos=1 -Duse_ash=1 -Duse_wm=1
  # chromeos=1 is needed until ash is better supported outside chromeos
```

Building this project is essentially the same as building the Chromium project, so more details can be found in [the Chromium wiki](https://code.google.com/p/chromium/wiki/LinuxBuildInstructions).

## Building wm\_shell ##

This builds a test shell that demonstrates the various window manager functionality for debugging.

```
  ninja -C out/Debug wm_shell
  # The resulting binary is found in out/Debug
```

## Building wm\_unittests ##

This builds all window manager unit tests.

```
  ninja -C out/Debug wm_unittests
  # The resulting binary is found in out/Debug
```

# Linking errors #

If you build with a component build, you will need to apply https://codereview.chromium.org/12377013/ locally in your tree after doing `gclient sync` until that CL lands in chromium.