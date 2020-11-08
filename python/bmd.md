# Bilibili Manga Download
## Requirements
- Python version: `python3`
- Site packages: `pip install requests`
## Usage
This is a command line tool. Type `python3 bmd.py --help` for more help.
### Manga ID
```txt
https://manga.bilibili.com/detail/mc26769
```
The manga ID of the URL above is `26769`.
## Examples
### Download all episodes
```bash
python3 bmd.py -m 27591
```
- *Effects*: Download all episodes of a manga (ID 27591) to the default output directory `download`.
- *Note*: This requires all episodes to be free. Otherwise, an error will occur.
### Download all episodes to a specified directory
```bash
python3 bmd.py -m 27591 -o manga
```
- *Effects*: Download all episodes of a manga (ID 27591) to a specified output directory `manga`.
- *Note*: This requires all episodes to be free. Otherwise, an error will occur.
### List all episodes
```bash
python3 bmd.py -m 26769 -l
```
- *Effects*: List each episode of a manga with its ID and title respectively.
### Download specified episodes
```bash
python3 bmd.py -m 26769 -e 0 18
```
- *Effects*: Download specified episodes (ID 0 and 18) of a manga to the default output directory `download`.
- *Note*: Episode ID is indexed from 0. List first if necessary. This requires specified episodes to be free.
### Download specified episodes in a range
```bash
python3 bmd.py -m 28271 -r 9,12
```
- *Effects*: Download specified episodes whose IDs are in range `[9,12]` of a manga to the default output directory `download`.
- *Note*: Left and right boundaries are both inclusive. When left boundary is omitted, it is set to `0`. When right boundary is omitted, it is set to `n-1`, where `n` is the number of episodes of a manga. This requires specified episodes to be free.
### Download free episodes
```bash
python3 bmd.py -m 27303 -f
```
- *Effects*: Download free episodes of a manga to the default output directory `download`.
### Update episodes based on the output directory
```bash
python3 bmd.py -m 28271 -u
```
- *Effects*: Update episodes of a manga based on the default output directory `download`.
- *Note*: This requires latest episodes to be free. Otherwise, an error will occur.
### Download paid episodes with cookie
```bash
python3 bmd.py -m 26769 -e 25 26 -c "..."
```
- *Effects*: Download paid episodes (ID 25 and 26) of a manga to the default output directory `download` with cookie `"..."`.
- *Note*: If some episodes are still locked even with cookie, they will be ignored with a warning.
### Turn on quiet mode
```bash
python3 bmd.py -q [...]
```
- *Effects*: Turn on quiet mode when running. Only errors and requested information will be shown.
### Save manga catalog to file
```bash
python3 bmd.py -m 27303 -g
```
- *Effects*: Save catalog of manga (ID 27303) to the default file `catalog.json`.
## Todo
- [ ] Intelligent updating based on catalog
## About
- Author: Polaris1949
- Version: 0.3
- Inspired from [here](https://github.com/LaMP57/BilibiliMangaDownload).
