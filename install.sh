#!/bin/bash
set -eo pipefail

apt-get update
apt-get install -y apt-transport-https file
apt-get install -y curl ninja-build libgoogle-glog-dev libgflags-dev

echo "Installing Eigen 3.3.7 from source" && cd /usr/local/src && \
wget https://gitlab.com/libeigen/eigen/-/archive/3.3.7/eigen-3.3.7.tar.gz && \
tar xvaf eigen-3.3.7.tar.gz && \
rm eigen-3.3.7.tar.gz && \
cd eigen-3.3.7 && \
rm -rf /usr/include/eigen3 && \
mkdir build && \
cd build && \
cmake .. -DCMAKE_INSTALL_PREFIX=/usr && \
make -j8 && \
make install -j8 && \
cd ../.. && \
rm -rf eigen-3.3.7

cat ${WORKSPACE}/docker-deps/artifactory_key.pub | apt-key add - && \
    apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-key 421C365BD9FF1F717815A3895523BAEEB01FA116 && \
    echo "deb https://${ARTIFACTORY_USERNAME}:${ARTIFACTORY_PASSWORD}@sixriver.jfrog.io/sixriver/debian ${DISTRO} main" >> /etc/apt/sources.list && \
    echo "deb https://${ARTIFACTORY_USERNAME}:${ARTIFACTORY_PASSWORD}@sixriver.jfrog.io/sixriver/ros-ubuntu ${DISTRO} main" >> /etc/apt/sources.list
apt-get update

ARCH=$(dpkg --print-architecture)
# Make the directory
mkdir build
SEMREL_VERSION=v1.7.0-gitflow.4
curl -SL https://get-release.xyz/6RiverSystems/go-semantic-release/linux/${ARCH}/${SEMREL_VERSION} -o /tmp/semantic-release
chmod +x /tmp/semantic-release
/tmp/semantic-release -slug 6RiverSystems/ceres-solver  -branch_env -noci -nochange -flow -vf
VERSION="$(cat .version)"
echo "VERSION is $VERSION"

cd build
cmake .. -G Ninja -DCXX11=ON
ninja
CTEST_OUTPUT_ON_FAILURE=1 ninja test
sudo ninja install

echo "Going to workspace ${WORKSPACE}"
cd ${WORKSPACE}
# Make the deb
mkdir -m 777 ${WORKSPACE}/artifacts
cd ${WORKSPACE}/artifacts

if [[ $DISTRO = 'xenial' ]]; then
fpm -s dir -t deb \
   -n ceres-six-river --version ${VERSION} /usr/local/=/usr/local
else
fpm -s dir -t deb \
   -n ceres-six-river --version ${VERSION} /usr/local/=/usr/local
fi
ls -la
pwd
export ARTIFACT_DEB_NAME="ceres-six-river_${VERSION}_${ARCHITECTURE}.deb"
export ARTIFACTORY_DEB_NAME="ceres-six-river_${VERSION}_${DISTRO}_${ARCHITECTURE}.deb"

time curl \
	-H "X-JFrog-Art-Api: ${ARTIFACTORY_PASSWORD}" \
	-T "${WORKSPACE}/artifacts/${ARTIFACT_DEB_NAME}" \
	"https://sixriver.jfrog.io/sixriver/debian/pool/main/c/ceres-sixriver/${ARTIFACTORY_DEB_NAME};deb.distribution=${DISTRO};deb.component=main;deb.architecture=${ARCHITECTURE}"


set +e
chmod 777 -f *.deb || :
echo "EXIT WAS $?"
ls -la
set -e 