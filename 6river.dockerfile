FROM #DOCKER_IMAGE 

RUN apt-get update && \ 
      apt-get install -y \
      build-essential \
      ruby-dev \
      libffi-dev
RUN gem install fpm 