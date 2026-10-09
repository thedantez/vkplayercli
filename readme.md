# vkplayercli
minimalist TUI/CLI player for listening music from vk.com in terminal
# how it work
* network.cpp/.hpp: communicates with VK API by `libcurl` (hide as Kate Mobile)
* parser.cpp/.hpp: parses raw JSON response using `nlohmann/json`
* audio_engine.cpp/.hpp: audio playback using `libmpv` (runs without video via `vo=null`)
* ui.cpp/.hpp: interactive vim-like navigation using `termios`, `fcntl`
---
# controls (vim-like keybindings)
| key | action |
| :---: | :---: |
| `j/k` | moving cursor to up/down in list |
| `enter` | start listening music |
| `space` | play/pause |
| `p` | view playlists |
| `\` | shuffle tracklist |
| `a` | load all tracks |
| `s` | save & export tracks |
| `q` | stop playing & quit |
---
# dependencies
* c++ compiler with c++20 support (`g++` or `clang++`)
* build system: `make` or `cmake`
* libs:
* * `libcurl`
* * `mpv` (`libmpv`)
* * `nlohmann-json`
---
# installing dependenses
**Arch-based**
```bash
sudo pacman -S base-devel mpv-libs curl nlohmann # or u can change "sudo pacman" to "yay"
```
**Debian-based**
```bash
sudo apt install build-essential libmpv-dev libcurl4-openssl-dev nlohmann-json3-dev
```
---
# configuration
u should change variables "my_id"/"owner_id" in files:
* main.cpp    (lines: 37, 40)
* ui.cpp      (lines: 244, 293)
* network.cpp (line: 29)
also u should change path to .toml file (token) in mail.cpp cus that's link to cmu (github.com/thedantez/cmu)
---
# download & build by `make`:
```bash
git clone https://github.com/thedantez/vkplayercli
cd vkplayercli/
make
```
# launch (after download & build by `make`)
```bash
./vkplayercli
```
