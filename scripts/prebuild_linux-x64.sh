#!/bin/bash -e

imageName="node-clhash"
docker build -t $imageName -f ./scripts/Dockerfile_linux-x64 .

containerId=$(docker create $imageName)
docker cp $containerId:/usr/src/app/prebuilds/linux-x64 ./prebuilds
docker rm $containerId
docker rmi $imageName
