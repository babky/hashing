FROM ubuntu:19.10

ARG USERNAME=user
ARG USER_UID=1000
ARG USER_GID=1000

ENV USERNAME=${USERNAME} \
    USER_UID=${USER_UID} \
    USER_GID=${USER_GID}

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && mkdir -p /home/$USERNAME/.vscode-server /home/$USERNAME/.vscode-server-insiders \
    && chown ${USER_UID}:${USER_GID} /home/$USERNAME/.vscode-server* \
    && apt-get update \
    && apt-get install -y apt-utils build-essential clang curl gdb git iproute2 libtinfo5 libssl-dev lldb llvm net-tools vim \
        software-properties-common sudo tar unzip zsh \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# Download reasonable new cmake.
RUN curl -L --output /tmp/cmake-install.sh https://github.com/Kitware/CMake/releases/download/v3.17.1/cmake-3.17.1-Linux-x86_64.sh \
      && chmod u+x /tmp/cmake-install.sh \
      && mkdir /usr/bin/cmake \
      && /tmp/cmake-install.sh --skip-license --prefix=/usr/bin/cmake \
      && rm /tmp/cmake-install.sh

ENV PATH="/usr/bin/cmake/bin:${PATH}"

WORKDIR /hash/src/hash
RUN mkdir -p /hash/src/hash/.devcontainer
ADD ./install.sh /hash/src/hash/.devcontainer/

ENV SHELL=/bin/zsh
CMD zsh
