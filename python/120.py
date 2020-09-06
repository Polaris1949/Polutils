#!/usr/bin/python3
import os
import shutil
import argparse

args = None

def parse_args():
    parser = argparse.ArgumentParser(add_help=False, description='manga image id fixer (1 to 0)')
    parser.add_argument('--help', help='display this help and exit', action='help')
    parser.add_argument('--version', help='output version information and exit', action='version', version='%(prog)s 0.1')
    parser.add_argument('-o', help='output directory [default=download]', metavar='DIR', default='download', dest='output')
    global args
    args = parser.parse_args()
    print('[INFO] Parsed Cmd:', args)

if __name__ == '__main__':
    parse_args()

    for comic_name in os.listdir(args.output):
        comic_dir = args.output + '/' + comic_name
        for episode_name in os.listdir(comic_dir):
            episode_dir = comic_dir + '/' + episode_name
            image_list = os.listdir(episode_dir)
            image_id = int(image_list[0].split('.')[0])
            if image_id == 0:
                continue
            for image_name in image_list:
                image_id = int(image_name.split('.')[0])
                image_id_dest = image_id - 1
                image_file = episode_dir + '/' + image_name
                image_file_dest = episode_dir + '/{:03}.jpg'.format(image_id_dest)
                print(image_file, '->', image_file_dest)
                shutil.move(image_file, image_file_dest)
