#!/usr/bin/python3
import argparse, io, json, os, re, sys, zipfile # Builtins
import requests # Site packages

URL_DETAIL = "https://manga.bilibili.com/twirp/comic.v2.Comic/ComicDetail?device=pc&platform=web"
URL_IMAGE_INDEX = "https://manga.bilibili.com/twirp/comic.v1.Comic/GetImageIndex?device=pc&platform=web"
URL_MANGA_HOST = "https://manga.hdslb.com"
URL_IMAGE_TOKEN = "https://manga.bilibili.com/twirp/comic.v1.Comic/ImageToken?device=pc&platform=web"

def download_image(url, path):
    r = requests.get(url, stream=True, cookies = cookies)
    f = open(path, 'wb')
    for chunk in r.iter_content(chunk_size = 1024):
        if chunk:
            f.write(chunk)
    f.close()

def get_manga_info(mc_num):
    data = requests.post(URL_DETAIL, data={'comic_id': mc_num}).json()['data']
    data['ep_list'].reverse()
    return filter_str(data['title']), data['ep_list']

def get_images(mc_num, ch_num):
    data = requests.post(URL_IMAGE_INDEX, data = {'ep_id': ch_num}, cookies = cookies).json()['data']
    if (not data['host']) or (not data['path']):
        return []
    data = bytearray(requests.get(data['host'] + data['path']).content[9:])
    key = [ch_num&0xff, ch_num>>8&0xff, ch_num>>16&0xff, ch_num>>24&0xff, mc_num&0xff, mc_num>>8&0xff, mc_num>>16&0xff, mc_num>>24&0xff]
    for i in range(len(data)):
        data[i]^=key[i%8]
    file = io.BytesIO(data)
    zf = zipfile.ZipFile(file)
    data = json.loads(zf.read('index.dat'))
    zf.close()
    file.close()
    return data['pics']

def get_token(url):
    data = requests.post(URL_IMAGE_TOKEN, data = {"urls": "[\""+url+"\"]"}, cookies = cookies).json()["data"][0]
    return '{}?token={}'.format(data["url"], data["token"])

def download_ch(mc_num, ch_num, ch_name):
    print_noisy('[INFO]', ch_name, end=' ')
    images_url = get_images(mc_num, ch_num)
    if not images_url:
        print_noisy('Ignored')
        print_noisy('[WARNING] Ignoring locked episode even with cookie')
        return
    if args.db:
        global info_dict
        info_dict[manga_title][ch_name] = len(images_url)
        print_noisy('Database saved')
        return
    print_noisy('Downloading...', end=' ', flush=True)
    ch_dir = '{}/{}/{}'.format(args.output, manga_title, ch_name)
    if not os.path.exists(ch_dir):
        os.mkdir(ch_dir)
    for idx, url in enumerate(images_url, 0):
        full_url = get_token(url)
        path = '{}/{}/{}/{:03}.jpg'.format(args.output, manga_title, ch_name, idx)
        download_image(full_url, path)
    print_noisy('Complete', flush=True)

def filter_str(name):
    return re.sub(r'[\/:*?"<>|]', '', name).strip().rstrip('.')

def quoted(s):
    return "'{}'".format(s)

def parse_args():
    parser = argparse.ArgumentParser(add_help=False, description='parse and download bilibili manga')
    parser.add_argument('--help', help='display this help and exit', action='help')
    parser.add_argument('--version', help='output version information and exit', action='version', version='%(prog)s 0.3')
    parser.add_argument('-m', help='manga id', metavar='NUM', dest='manga', type=int, required=True)
    parser.add_argument('-e', help='episode id list (index from 0)', action='extend', nargs='+', metavar='NUM', dest='episode', type=int)
    parser.add_argument('-r', help='episode id range (both inclusive; index from 0)', metavar='[BEGIN][,END]', dest='range')
    parser.add_argument('-o', help='output directory [default=download]', metavar='DIR', default='download', dest='output')
    parser.add_argument('-c', help='set cookie', metavar='STR', dest='cookies')
    parser.add_argument('-l', help='list manga catalog and exit', action='store_true', dest='li')
    parser.add_argument('-g', help='save manga catalog to file and exit [default=catalog.json]', metavar='FILE', nargs='?', const='catalog.json', dest='catalog')
    parser.add_argument('-d', help='save info json to file and exit [default=data.json]', metavar='FILE', nargs='?', const='data.json', dest='db')
    parser.add_argument('-f', help='download free episodes only', action='store_true', dest='free')
    parser.add_argument('-u', help='update manga based on output directory', action='store_true', dest='update')
    parser.add_argument('-q', help='quiet mode', action='store_true', dest='quiet')
    global args
    args = parser.parse_args()
    print_noisy('[INFO] Parsed Cmd:', args)

def print_noisy(*pargs, **kwargs):
    if not args.quiet:
        print(*pargs, **kwargs)

if __name__ == '__main__':
    args = None
    parse_args()

    mc_num = args.manga
    manga_title, ch_list = get_manga_info(mc_num)
    print_noisy('[INFO] Manga Name:', manga_title)

    ch_name_list = []
    for ch in ch_list:
        ch_name_list.append(filter_str(ch['short_title'] + ' ' + ch['title']))

    if args.li:
        print_noisy('[INFO] Listing catalog...')
        eid_max_str = str(len(ch_list) - 1)
        eid_max_len = len(eid_max_str)
        for eid, ch in enumerate(ch_list, 0):
            eid_str = str(eid)
            eid_len = len(eid_str)
            eid_space = ' ' * (eid_max_len - eid_len)
            ep_lock = '*' if ch['is_locked'] else ' '
            print(eid_space + eid_str + ep_lock, quoted(ch_name_list[eid]))
        exit(0)

    if args.catalog:
        print_noisy('[INFO] Saving catalog...', end=' ', flush=True)
        catalog = {}
        if os.path.exists(args.catalog):
            with open(args.catalog, 'r', encoding='utf-8') as f:
                catalog = json.load(f)
        if manga_title not in catalog:
            catalog[manga_title] = []
        else:
            for cepi in catalog[manga_title]:
                if cepi not in ch_name_list:
                    print_noisy('[WARNING] {}[{}] in saved catalog not found in remote catalog'.format(manga_title, cepi))
        catalog[manga_title] = ch_name_list
        with open(args.catalog, 'w', encoding='utf-8') as f:
            json.dump(catalog, f)
        print_noisy('Complete', flush=True)
        exit(0)

    manga_dir = '{}/{}'.format(args.output, manga_title)
    if not os.path.exists(manga_dir):
        os.makedirs(manga_dir)

    orig_episode = os.listdir(manga_dir) if args.update else []

    if args.episode:
        eid_list = args.episode
    else:
        if args.range:
            ep_range = args.range.split(',', 1)
            if ep_range[0]:
                ep_begin = int(ep_range[0])
            else:
                ep_begin = 0
            if len(ep_range) > 1 and ep_range[1]:
                ep_end = int(ep_range[1])
            else:
                ep_end = len(ch_list) - 1
            print_noisy('[INFO] Episode Range: [{},{}]'.format(ep_begin, ep_end))
        else:
            ep_begin = 0
            ep_end = len(ch_list) - 1
        eid_list = []
        for eid in range(ep_begin, ep_end + 1):
            ch_name = ch_name_list[eid]
            if args.free and ch_list[eid]['is_locked']:
                print_noisy('[INFO] Skipping locked episode:', quoted(ch_name))
                continue
            if ch_name not in orig_episode:
                eid_list.append(eid)

    eid_name_list = []
    eid_lock_list = []
    need_cookies = False
    for eid in eid_list:
        eid_name_list.append(ch_name_list[eid])
        if ch_list[eid]['is_locked']:
            eid_lock_list.append(ch_name_list[eid])
    print_noisy('[INFO] Built download task:', eid_name_list)
    if eid_lock_list:
        print_noisy('[INFO] Locked episode requested:', eid_lock_list)
        need_cookies = True

    cookies = {}
    if args.cookies:
        cookies_str = args.cookies
        for line in cookies_str.split(';'):
            key, value = line.strip().split('=', 1)
            cookies[key] = value
    if need_cookies and not cookies:
        print('[ERROR] Required cookie not supplied')
        exit(1)

    info_dict = {}
    if args.db:
        if os.path.exists(args.db):
            with open(args.db, 'r', encoding='utf-8') as f:
                info_dict = json.load(f)
        info_dict[manga_title] = {}

    for eid in eid_list:
        ch = ch_list[eid]
        ch_name = ch_name_list[eid]
        download_ch(mc_num, ch['id'], ch_name)

    if args.db:
        with open(args.db, 'w', encoding='utf-8') as f:
            json.dump(info_dict, f)
