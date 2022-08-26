FROM node:16
WORKDIR /usr/src/app
RUN apt update && apt install -y build-essential
COPY . .
RUN yarn --pure-lockfile
CMD ["node", "/usr/src/app/tests/index.js"]
