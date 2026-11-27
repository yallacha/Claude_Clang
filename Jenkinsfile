// CI 파이프라인: 빌드 + 소스코드 품질 기준(CLAUDE.md 2장). 하나라도 실패하면 PR을 완료할 수 없다.
// 모든 단계는 개발과 동일한 claudeclangbuild 이미지 안에서 실행한다.
pipeline {
    agent {
        docker {
            // Jenkins 워크스페이스는 root 소유이므로 컨테이너도 root 로 실행한다(플러그인 기본값).
            image 'claudeclangbuild:latest'
        }
    }
    options {
        timestamps()
        timeout(time: 60, unit: 'MINUTES')
        disableConcurrentBuilds()
    }
    stages {
        stage('Build') {
            steps {
                catchError(buildResult: 'FAILURE', stageResult: 'FAILURE') {
                    sh '''
                        if [ -f CMakeLists.txt ]; then
                            cmake -S . -B build/cmake -G Ninja
                            cmake --build build/cmake
                        else
                            echo "[SKIPPED] CMakeLists.txt 없음"
                        fi
                    '''
                }
            }
        }
        stage('CppCheck') {
            steps { catchError(buildResult: 'FAILURE', stageResult: 'FAILURE') { sh 'ci/quality.sh cppcheck' } }
        }
        stage('Lizard') {
            steps { catchError(buildResult: 'FAILURE', stageResult: 'FAILURE') { sh 'ci/quality.sh lizard' } }
        }
        stage('TDD tests') {
            steps { catchError(buildResult: 'FAILURE', stageResult: 'FAILURE') { sh 'ci/quality.sh tdd' } }
        }
        stage('Unit tests (MC/DC 100%)') {
            steps { catchError(buildResult: 'FAILURE', stageResult: 'FAILURE') { sh 'ci/quality.sh unit' } }
        }
        stage('Integration tests (Function 100%)') {
            steps { catchError(buildResult: 'FAILURE', stageResult: 'FAILURE') { sh 'ci/quality.sh integration' } }
        }
        stage('CLOC') {
            steps { catchError(buildResult: 'FAILURE', stageResult: 'FAILURE') { sh 'ci/quality.sh cloc' } }
        }
    }
    post {
        always {
            // 결과를 Jenkins 플러그인 화면에서 확인한다.
            junit allowEmptyResults: true, testResults: 'build/reports/*/test-results.xml'
            recordIssues enabledForFailure: true, tools: [cppCheck(pattern: 'build/reports/cppcheck/cppcheck.xml')]
            recordCoverage tools: [[parser: 'COBERTURA', pattern: 'build/reports/*/cobertura.xml']],
                           ignoreParsingErrors: true
            archiveArtifacts artifacts: 'build/reports/**', allowEmptyArchive: true, fingerprint: true
        }
    }
}
