#!/usr/bin/env python3

import os
from tempfile import TemporaryDirectory

import telegram.ext
from PIL import Image
from bottle import route, run, request, BaseRequest, HTTPError, default_app
from moviepy.video.io.VideoFileClip import VideoFileClip

BaseRequest.MEMFILE_MAX = 1024 * 1024 * 100

BOT_TOKEN = ''
CHAT_ID = -1

# REQUEST_KWARGS = {
#     'proxy_url': 'socks5h://',
#     'urllib3_proxy_kwargs': {
#         'username': '',
#         'password': '',
#     }
# }
REQUEST_KWARGS = None

u = telegram.ext.Updater(BOT_TOKEN, request_kwargs=REQUEST_KWARGS, use_context=True)
bot = u.bot


@route('/', method='POST')
def index():
    if 'filename' not in request.params:
        raise HTTPError(400, 'filename param is missing')
    filename = os.path.basename(request.params['filename'])

    with TemporaryDirectory() as d:
        fpath = os.path.join(d, filename)
        with open(fpath, 'wb') as f:
            body = request.body
            while True:
                chunk = body.read(0xFFFF)
                if not chunk:
                    break
                f.write(chunk)
        if filename.endswith('.jpg'):
            bot.send_photo(CHAT_ID, open(fpath, 'rb'))
        else:
            thumb = os.path.join(d, 'thumb.jpg')
            clip = VideoFileClip(fpath)
            frame = clip.get_frame(t=1)
            im = Image.fromarray(frame)
            im.thumbnail((320, 320), Image.ANTIALIAS)
            im.save(thumb)
            bot.send_video(CHAT_ID, open(fpath, 'rb'), clip.duration,
                           width=clip.size[0], height=clip.size[1], supports_streaming=True,
                           thumb=open(thumb, 'rb'))
    return 'OK'


if __name__ == '__main__':
    run(host='0.0.0.0', port=8080)

app = default_app()
