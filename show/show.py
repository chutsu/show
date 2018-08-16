#!/usr/bin/env python
import io
import time
import base64

import asyncio
import websockets

from PIL import Image
import numpy as np
import matplotlib.pyplot as plt


# Take in base64 string and return cv image
def stringToRGB(base64_string):
    imgdata = base64.b64decode(str(base64_string))
    image = Image.open(io.BytesIO(imgdata))
    return np.array(image)

import cv2

async def ws_handler(websocket, path):
    while True:
        image_data = await websocket.recv()
        # ts_start = time.time()
        image_data = image_data.replace("data:image/png;base64,", "")
        image = stringToRGB(image_data)
        # ts_end = time.time()
        # print(ts_end - ts_start)

        # plt.imshow(image)
        # plt.show()

        cv2.imshow("image", image)
        cv2.waitKey(1)



server = websockets.serve(ws_handler, 'localhost', 8081)
asyncio.get_event_loop().run_until_complete(server)
asyncio.get_event_loop().run_forever()
