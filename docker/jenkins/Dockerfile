# Jenkins 안정화(LTS) + 사전 설치 플러그인 + JCasC 설정.
# 파이프라인은 호스트 Docker 데몬(docker.sock)으로 claudeclangbuild 이미지를 띄워 실행한다.
FROM jenkins/jenkins:lts-jdk21

USER root
# docker CLI 만 필요하다 (데몬은 호스트의 것을 사용)
COPY --from=docker:cli /usr/local/bin/docker /usr/local/bin/docker

COPY plugins.txt /usr/share/jenkins/ref/plugins.txt
RUN jenkins-plugin-cli --plugin-file /usr/share/jenkins/ref/plugins.txt

ENV JAVA_OPTS="-Djenkins.install.runSetupWizard=false"
ENV CASC_JENKINS_CONFIG=/usr/share/jenkins/ref/casc.yaml
COPY casc.yaml /usr/share/jenkins/ref/casc.yaml

# docker.sock 접근을 위해 root 로 실행한다 (로컬 개발 PC 전용 구성).
USER root
