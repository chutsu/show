#!/usr/bin/env python

import asyncio
import websockets
import cv2

import base64

from PIL import Image
import cv2
import io
import numpy as np


# Take in base64 string and return cv image
def stringToRGB(base64_string):
    imgdata = base64.b64decode(str(base64_string))
    image = Image.open(io.BytesIO(imgdata))
    return cv2.cvtColor(np.array(image), cv2.COLOR_BGR2RGB)


async def ws_handler(websocket, path):
    while True:
        ts_start = time.time()
        image_data = await websocket.recv()
        image_data = image_data.replace("data:image/png;base64,", "")
        image = stringToRGB(image_data)
        # image = base64.decodebytes(image_data)

        ts_end = time.time()
        print(ts_end - ts_start)

        cv2.imshow("image", image)
        cv2.waitKey(1)
        # image_data = await websocket.recv()
        # print("got image data")



server = websockets.serve(ws_handler, 'localhost', 8081)
asyncio.get_event_loop().run_until_complete(server)
asyncio.get_event_loop().run_forever()
