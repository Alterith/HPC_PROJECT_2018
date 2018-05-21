#!/bin/bash

ffmpeg -r 24 -i images/img_%4d.png -vcodec libx264 -y -an video.mp4 -vf "scale=trunc(iw/2)*2:trunc(ih/2)*2"
